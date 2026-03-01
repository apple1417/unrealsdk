#include "unrealsdk/pch.h"

#include "unrealsdk/config.h"
#include "unrealsdk/logging.h"
#include "unrealsdk/unrealsdk.h"
#include "unrealsdk/utils.h"

namespace unrealsdk::logging {

namespace {

#ifndef UNREALSDK_IMPORTING
// We push log messages into a queue, to be written by another thread
// This means we need to own the strings - the raw LogMessage is just a reference type for callbacks
struct OwnedLogMessage : public LogMessage {
   private:
    std::string msg_str;
    std::string location_str;

   public:
    OwnedLogMessage(const LogMessage& log)
        : LogMessage(log),
          msg_str{log.msg, log.msg_size},
          location_str{log.location, log.location_size} {}

    /**
     * @brief Fills in the string pointers and decays back into a raw log message.
     *
     * @return A pointer to this message.
     */
    LogMessage* as_ptr(void) {
        this->msg = this->msg_str.data();
        this->msg_size = this->msg_str.size();
        this->location = this->location_str.data();
        this->location_size = this->location_str.size();
        return this;
    }
};

std::mutex pending_messages_mutex{};
std::queue<OwnedLogMessage> pending_messages{};
std::condition_variable pending_messages_available{};

Level unreal_console_level = Level::DEFAULT_CONSOLE_LEVEL;
std::unique_ptr<std::ostream> log_file_stream;

HANDLE external_console_handle = nullptr;
bool external_console_colour = false;

std::mutex callback_mutex{};
std::vector<log_callback> all_log_callbacks{};

#pragma region Logger Thread

[[noreturn]] void logger_thread(void) {
    SetThreadDescription(GetCurrentThread(), L"unrealsdk logger");

    while (true) {
        std::unique_lock<std::mutex> pending_messages_lock(pending_messages_mutex);
        if (pending_messages.empty()) {
            pending_messages_available.wait(pending_messages_lock,
                                            []() { return !pending_messages.empty(); });
        }

        {
            const std::scoped_lock callback_lock(callback_mutex);
            while (!pending_messages.empty()) {
                auto msg = std::move(pending_messages.front());
                pending_messages.pop();

                pending_messages_lock.unlock();

                auto ptr = msg.as_ptr();
                for (const auto& callback : all_log_callbacks) {
                    callback(ptr);
                }

                pending_messages_lock.lock();
            }
        }
    }
}

#pragma endregion
#endif

#pragma region Conversions

/**
 * @brief Gets the current unix time in milliseconds.
 *
 * @return The unix time milliseconds.
 */
uint64_t unix_ms_now(void) {
    auto time = std::chrono::system_clock::now();
    return std::chrono::round<std::chrono::milliseconds>(time.time_since_epoch()).count();
}

#ifndef UNREALSDK_IMPORTING
/**
 * @brief Gets a system clock time point from unix time milliseconds.
 *
 * @param unix_time_ms The unix time milliseconds.
 * @return The system clock time point.
 */
std::chrono::sys_time<std::chrono::milliseconds> time_from_unix_ms(uint64_t unix_time_ms) {
    return std::chrono::round<std::chrono::milliseconds>(
        std::chrono::system_clock::time_point{std::chrono::milliseconds{unix_time_ms}});
}

/**
 * @brief Gets the name of a log level.
 *
 * @param level The log level
 * @return The level's name.
 */
std::string get_level_name(Level level) {
    switch (level) {
        default:
        case Level::ERROR:
            return "ERR";
        case Level::WARNING:
            return "WARN";
        case Level::INFO:
            return "INFO";
        case Level::DEV_WARNING:
            return "DWRN";
        case Level::MISC:
            return "MISC";
    }
}

/**
 * @brief Gets a log level from it's string representation.
 *
 * @param str The string.
 * @return The parsed log level, or `Level::INVALID`.
 */
Level get_level_from_string(std::string_view str) {
    if (str.empty()) {
        return Level::INVALID;
    }

    // Doing it properly is a bit more complex, just check the first char
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    switch (std::toupper(str[0])) {
        case 'E':
            return Level::ERROR;
        case 'W':
            return Level::WARNING;
        case 'I':
            return Level::INFO;
        case 'D':
            return Level::DEV_WARNING;
        case 'M':
            return Level::MISC;
        default:
            return Level::INVALID;
    }
}
#endif

#pragma endregion

#pragma region Formatting
#ifndef UNREALSDK_IMPORTING

const std::string TRUNCATION_PREFIX = "~ ";

/**
 * @brief Truncates leading chunks of a string until it fits under a max width.
 * @note Will return strings longer than the max width if it can't cleanly chunk them.
 *
 * @param str The string to truncate.
 * @param separators The characters to use as separators between chunks.
 * @param max_width The maximum width of the string.
 * @return The truncated string.
 */
std::string truncate_leading_chunks(const std::string&& str,
                                    std::string_view separators,
                                    size_t max_width) {
    auto width = str.size();
    size_t start_pos = 0;
    while (width > max_width) {
        auto next_separator_char = str.find_first_of(separators, start_pos);
        if (next_separator_char == std::string::npos) {
            break;
        }
        auto next_regular_char = str.find_first_not_of(separators, next_separator_char);
        if (next_regular_char == std::string::npos) {
            break;
        }

        // The first time we truncate something, we know we now need to add the prefix on, so
        // subtract it from max width
        if (start_pos == 0) {
            max_width -= TRUNCATION_PREFIX.size();
        }

        width -= (next_regular_char - start_pos);
        start_pos = next_regular_char;
    }

    if (start_pos == 0) {
        return str;
    }

    return TRUNCATION_PREFIX + str.substr(start_pos);
}

// Doing some stringification to generate the format strings with correct widths
// NOLINTBEGIN(cppcoreguidelines-macro-usage)

constexpr std::string_view LOG_HEADER =
    // clang-format off
// 2026-02-26 19:48:33.235Z   520c                                    unrealsdk::init@59   INFO|
  "date       time          thread                                           location@line    v|\n";
// clang-format on

#define DATE_WIDTH 10
#define TIME_WIDTH 12
#define TIMESTAMP_WIDTH 23  // Can't calculate since we stringify this value
#define THREAD_WIDTH 6      // Arguably should be 8, could be any DWORD
#define LOCATION_WIDTH 50
#define LINE_WIDTH 4
#define LEVEL_WIDTH 4

#define RESET_COLOUR "\x1B[0m"
#define TIMESTAMP_COLOUR "\x1B[0;32m"
#define THREAD_COLOUR "\x1B[0;35m"
#define LOCATION_COLOUR "\x1B[0;36m"
#define ERROR_COLOUR "\x1B[0;31m"
#define WARNING_COLOUR "\x1B[0;33m"
#define INFO_COLOUR RESET_COLOUR
#define DEV_WARNING_COLOUR "\x1B[0;38;5;172m"
#define MISC_COLOUR "\x1B[0;38;5;246m"

#define STR_INNER(x) #x
#define STR(x) STR_INNER(x)

#define TIMESTAMP_FORMAT_STR(n) "{" STR(n) ":>" STR(TIMESTAMP_WIDTH) "%F %T}Z"
#define THREAD_FORMAT_STR(n) "{" STR(n) ":>" STR(THREAD_WIDTH) "x}"
#define LOCATION_FORMAT_STR(n) "{" STR(n) ":>" STR(LOCATION_WIDTH) "}"
#define LINE_FORMAT_STR(n) "{" STR(n) ":<" STR(LINE_WIDTH) "d}"
#define LEVEL_FORMAT_STR(n) "{" STR(n) ":>" STR(LEVEL_WIDTH) "}"

#define BASE_FORMAT_STR            /* format */ \
    TIMESTAMP_FORMAT_STR(0)        /* format */ \
    " " THREAD_FORMAT_STR(1)       /* format */ \
        " " LOCATION_FORMAT_STR(2) /* format */ \
        "@" LINE_FORMAT_STR(3)     /* format */ \
        " " LEVEL_FORMAT_STR(4)    /* format */ \
        "| {5}\n"

#define COLOURFUL_FORMAT_STR                                    \
    TIMESTAMP_COLOUR TIMESTAMP_FORMAT_STR(0)       /* format */ \
        " " THREAD_COLOUR THREAD_FORMAT_STR(1)     /* format */ \
        " " LOCATION_COLOUR LOCATION_FORMAT_STR(2) /* format */ \
        RESET_COLOUR "@{6}" LINE_FORMAT_STR(3)     /* format */ \
        " {7}" LEVEL_FORMAT_STR(4)                 /* format */ \
        RESET_COLOUR "| {7}{5}" RESET_COLOUR "\n"

// NOLINTEND(cppcoreguidelines-macro-usage)

/**
 * @brief Formats a log message following our internal style.
 *
 * @param msg The log message.
 * @return The formatted message
 */
std::string format_message(const LogMessage& msg) {
    return std::format(BASE_FORMAT_STR, time_from_unix_ms(msg.unix_time_ms), msg.thread_id,
                       truncate_leading_chunks(msg.location, "\\/:", LOCATION_WIDTH), msg.line,
                       get_level_name(msg.level), std::string{msg.msg, msg.msg_size});
}

/**
 * @brief Formats a log message, colouring it with ansi escape sequences.
 *
 * @param msg The log message.
 * @return The formatted message
 */
std::string format_colourful_message(const LogMessage& msg) {
    // Pyunrealsdk reports a line of -1 when it doesn't know the location
    // Figure it's neat to highlight that as an error
    std::string_view line_colour = msg.line > 0 ? LOCATION_COLOUR : ERROR_COLOUR;

    std::string_view log_level_colour;
    switch (msg.level) {
        case Level::ERROR:
            log_level_colour = ERROR_COLOUR;
            break;
        case Level::WARNING:
            log_level_colour = WARNING_COLOUR;
            break;
        case Level::INFO:
        default:
            log_level_colour = INFO_COLOUR;
            break;
        case Level::DEV_WARNING:
            log_level_colour = DEV_WARNING_COLOUR;
            break;
        case Level::MISC:
            log_level_colour = MISC_COLOUR;
            break;
    }

    return std::format(COLOURFUL_FORMAT_STR, time_from_unix_ms(msg.unix_time_ms), msg.thread_id,
                       truncate_leading_chunks(msg.location, "\\/:", LOCATION_WIDTH), msg.line,
                       get_level_name(msg.level), std::string{msg.msg, msg.msg_size}, line_colour,
                       log_level_colour);
}

#endif
#pragma endregion

#pragma region Built-in Logger
#ifndef UNREALSDK_IMPORTING

void builtin_logger(const LogMessage* msg) {
    if (msg == nullptr) {
        return;
    }

    if (unreal_console_level != Level::INVALID && unreal_console_level <= msg->level) {
        unrealsdk::internal::uconsole_output_text(utils::widen({msg->msg, msg->msg_size}));
    }

    if (external_console_handle != nullptr || log_file_stream) {
        auto formatted = format_message(*msg);

        if (external_console_handle != nullptr) {
            if (external_console_colour) {
                auto colourful = format_colourful_message(*msg);
                WriteFile(external_console_handle, colourful.c_str(),
                          static_cast<DWORD>(colourful.size()), nullptr, nullptr);
            } else {
                WriteFile(external_console_handle, formatted.c_str(),
                          static_cast<DWORD>(formatted.size()), nullptr, nullptr);
            }
        }

        if (log_file_stream) {
            *log_file_stream << formatted << std::flush;
        }
    }
}

#endif
#pragma endregion

}  // namespace

#pragma region Public Interface Implementations
#ifndef UNREALSDK_IMPORTING
namespace impl {
namespace {

void enqueue_log_msg(const LogMessage& log) {
    {
        const std::scoped_lock lock(pending_messages_mutex);
        pending_messages.emplace(log);
    }
    pending_messages_available.notify_all();
}

bool set_console_level(Level level) {
    if (Level::MIN > level || level > Level::MAX) {
        LOG(ERROR, "Log level out of range: {}", (uint8_t)level);
        return false;
    }
    unreal_console_level = level;
    return true;
}

void add_callback(log_callback callback) {
    const std::scoped_lock lock(callback_mutex);

    all_log_callbacks.push_back(callback);
}

void remove_callback(log_callback callback) {
    const std::scoped_lock lock(callback_mutex);

    auto [begin, end] = std::ranges::remove(all_log_callbacks, callback);
    all_log_callbacks.erase(begin, end);
}

}  // namespace
}  // namespace impl

void init(const std::filesystem::path& file, bool unreal_console) {
    static bool initialized = false;
    if (initialized) {
        return;
    }
    initialized = true;

    // Start the logger thread first thing
    std::thread(logger_thread).detach();

    if (unreal_console) {
        auto config_level_str = config::get_str("unrealsdk.console_log_level");
        if (config_level_str.has_value()) {
            auto config_level = get_level_from_string(*config_level_str);
            if (config_level != Level::INVALID) {
                unreal_console_level = config_level;
            }
        }
    } else {
        unreal_console_level = Level::INVALID;
    }

    if (!file.empty()) {
        log_file_stream = std::make_unique<std::ofstream>(file, std::ofstream::trunc);
        *log_file_stream << LOG_HEADER << std::flush;
    }

    // Add the builtin logger now, after initializing the above two streams, so that the external
    // console error messages actually have somewhere to go
    impl::add_callback(&builtin_logger);

#ifdef NDEBUG
    if (config::get_bool("unrealsdk.external_console").value_or(false))
#endif
    {
        if (AllocConsole() != 0) {
            external_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
            if (external_console_handle == nullptr) {
                LOG(ERROR, "Failed to get handle to external console!");
            }
        } else {
            LOG(ERROR, "Failed to initialize external console!");
        }

        external_console_colour =
            config::get_bool("unrealsdk.external_console_colour").value_or([]() {
                // Wine's console apparently doesn't properly support ansi escape codes, but still
                // makes the set mode call succeed, so if we detect wine just disable it by default
                auto ntdll = GetModuleHandle("ntdll");
                if (ntdll == nullptr) {
                    return true;
                }
                return GetProcAddress(ntdll, "wine_get_version") == nullptr;
            }());

        if (external_console_colour) {
            SetConsoleMode(external_console_handle,
                           ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT);
        }
    }
}
#endif
#pragma endregion

// =================================================================================================

#pragma region C API Wrappers

#ifdef UNREALSDK_SHARED
UNREALSDK_CAPI(void, enqueue_log_msg, const LogMessage* log);
#endif
#ifndef UNREALSDK_IMPORTING
UNREALSDK_CAPI(void, enqueue_log_msg, const LogMessage* log) {
    impl::enqueue_log_msg(*log);
}
#endif
void log(Level level, std::string_view msg, std::string_view location, int line) {
    // Important: get time asap
    auto now = unix_ms_now();
    // Rest can be in any order
    const LogMessage log{.unix_time_ms = now,
                         .level = level,
                         .msg = msg.data(),
                         .msg_size = msg.size(),
                         .location = location.data(),
                         .location_size = location.size(),
                         .line = line,
                         .thread_id = GetCurrentThreadId()};
    UNREALSDK_MANGLE(enqueue_log_msg)(&log);
}
void log(Level level, std::wstring_view msg, std::string_view location, int line) {
    auto now = unix_ms_now();

    auto narrow = utils::narrow(msg);
    const LogMessage log{.unix_time_ms = now,
                         .level = level,
                         .msg = narrow.data(),
                         .msg_size = narrow.size(),
                         .location = location.data(),
                         .location_size = location.size(),
                         .line = line,
                         .thread_id = GetCurrentThreadId()};
    UNREALSDK_MANGLE(enqueue_log_msg)(&log);
}

#ifdef UNREALSDK_SHARED
UNREALSDK_CAPI(bool, set_console_level, Level level);
#endif
#ifndef UNREALSDK_IMPORTING
UNREALSDK_CAPI(bool, set_console_level, Level level) {
    return impl::set_console_level(level);
}
#endif
bool set_console_level(Level level) {
    return UNREALSDK_MANGLE(set_console_level)(level);
}

#ifdef UNREALSDK_SHARED
UNREALSDK_CAPI(void, add_callback, log_callback callback);
#endif
#ifndef UNREALSDK_IMPORTING
UNREALSDK_CAPI(void, add_callback, log_callback callback) {
    impl::add_callback(callback);
}
#endif
void add_callback(log_callback callback) {
    UNREALSDK_MANGLE(add_callback)(callback);
}

#ifdef UNREALSDK_SHARED
UNREALSDK_CAPI(void, remove_callback, log_callback callback);
#endif
#ifndef UNREALSDK_IMPORTING
UNREALSDK_CAPI(void, remove_callback, log_callback callback) {
    impl::remove_callback(callback);
}
#endif
void remove_callback(log_callback callback) {
    UNREALSDK_MANGLE(remove_callback)(callback);
}

#pragma endregion

}  // namespace unrealsdk::logging
