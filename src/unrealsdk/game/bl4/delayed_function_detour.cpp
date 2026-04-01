#include "unrealsdk/pch.h"

#include "unrealsdk/game/bl4/bl4.h"
#include "unrealsdk/memory.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2

namespace unrealsdk::game::bl4 {

namespace {

using DLLSafeTargetResolver = utils::DLLSafeCallback<DelayedDetourTargetResolver>;

[[nodiscard]] bool is_executable_page(DWORD protect) {
    constexpr DWORD executable_mask =
        PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
    return (protect & PAGE_GUARD) == 0 && (protect & PAGE_NOACCESS) == 0
           && (protect & executable_mask) != 0;
}

#ifndef UNREALSDK_IMPORTING
std::mutex active_workers_mutex{};
std::unordered_set<std::string> active_workers{};

void clear_active_worker(const std::string& worker_name) {
    std::lock_guard lock{active_workers_mutex};
    active_workers.erase(worker_name);
}
#endif

}  // namespace

#ifdef UNREALSDK_SHARED
UNREALSDK_CAPI([[nodiscard]] bool, bl4_is_executable_address, uintptr_t addr);
UNREALSDK_CAPI(bool,
               bl4_delayed_function_detour,
               const char* name,
               size_t name_size,
               DLLSafeTargetResolver&& target_resolver,
               void* detour_func,
               void** original_func,
               const char* hook_name,
               size_t hook_name_size,
               int64_t poll_interval_ms,
               int64_t max_runtime_ms);
#endif

#ifndef UNREALSDK_IMPORTING
UNREALSDK_CAPI([[nodiscard]] bool, bl4_is_executable_address, uintptr_t addr) {
    if (addr == 0) {
        return false;
    }

    MEMORY_BASIC_INFORMATION mbi{};
    if (VirtualQuery(reinterpret_cast<void*>(addr), &mbi, sizeof(mbi)) != sizeof(mbi)) {
        return false;
    }

    return mbi.State == MEM_COMMIT && is_executable_page(mbi.Protect);
}

UNREALSDK_CAPI(bool,
               bl4_delayed_function_detour,
               const char* name,
               size_t name_size,
               DLLSafeTargetResolver&& target_resolver,
               void* detour_func,
               void** original_func,
               const char* hook_name,
               size_t hook_name_size,
               int64_t poll_interval_ms,
               int64_t max_runtime_ms) {
    if (name == nullptr || name_size == 0 || detour_func == nullptr || original_func == nullptr
        || hook_name == nullptr || hook_name_size == 0) {
        return false;
    }

    if (*original_func != nullptr) {
        return true;
    }

    auto worker_name = std::string{name, name_size};
    auto hook_name_view = std::string_view{hook_name, hook_name_size};

    {
        std::lock_guard lock{active_workers_mutex};
        if (active_workers.contains(worker_name)) {
            return true;
        }
        active_workers.emplace(worker_name);
    }

    auto poll_interval = std::chrono::milliseconds{std::max<int64_t>(poll_interval_ms, 1)};
    auto max_runtime =
        std::chrono::milliseconds{std::max<int64_t>(max_runtime_ms, poll_interval.count())};

    LOG(MISC, "Delaying detour for '{}'", worker_name);

    std::thread{
        [worker_name = std::move(worker_name),
         target_resolver = std::move(target_resolver), detour_func, original_func,
         hook_name = std::string{hook_name_view}, poll_interval, max_runtime]() mutable {
            try {
                const auto deadline = std::chrono::steady_clock::now() + max_runtime;
                while (std::chrono::steady_clock::now() < deadline) {
                    if (*original_func != nullptr) {
                        clear_active_worker(worker_name);
                        return;
                    }

                    auto target = target_resolver();
                    if (is_executable_address(target)
                        && memory::detour(target, detour_func, original_func, hook_name)) {
                        // LOG(MISC, "Delayed detour for '{}' finished successfully", worker_name);
                        clear_active_worker(worker_name);
                        return;
                    }

                    std::this_thread::sleep_for(poll_interval);
                }

                LOG(WARNING, "Delayed detour for '{}' timed out after {} ms", worker_name,
                    max_runtime.count());

            } catch (const std::exception& ex) {
                LOG(ERROR, "An exception occurred while running delayed detour for '{}': {}",
                    worker_name, ex.what());
            } catch (...) {
                LOG(ERROR, "An unknown exception occurred while running delayed detour for '{}'",
                    worker_name);
            }

            clear_active_worker(worker_name);
        }}
        .detach();

    return true;
}
#endif

}  // namespace unrealsdk::game::bl4

#endif
