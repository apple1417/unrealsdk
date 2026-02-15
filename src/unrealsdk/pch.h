#ifndef UNREALSDK_PCH_H
#define UNREALSDK_PCH_H

// Include the C exports library first, so we can use it everywhere
// This file is purely macros, it doesn't rely on anything else
#include "unrealsdk/exports.h"

// The flavour macros are used in ifdefs everywhere.
// `UNREALSDK_FLAVOUR` should be defined to be equal to one of them.
// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define UNREALSDK_FLAVOUR_WILLOW 1
#define UNREALSDK_FLAVOUR_WILLOW64 2
#define UNREALSDK_FLAVOUR_OAK 3
// NOLINTEND(cppcoreguidelines-macro-usage)

#define WIN32_LEAN_AND_MEAN
#define WIN32_NO_STATUS
#define NOGDI
#define NOMINMAX
#include <windows.h>
#include <winternl.h>

#undef WIN32_NO_STATUS
#include <ntstatus.h>

#include <processthreadsapi.h>
#ifdef __MINGW32__
// MinGW doesn't define this yet, stub it out - it's only used for debug
// NOLINTNEXTLINE(readability-identifier-naming)
#define SetThreadDescription(x, y)
#endif

#include <tlhelp32.h>

#include <MinHook.h>

#ifdef __cplusplus
#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <charconv>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <cwctype>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>

// This file is mostly just here so that the `LOG` macro is automatically available everywhere
// It only includes library headers, so is also ok to include
#include "unrealsdk/logging.h"

using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::int8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::uint8_t;

// Awaiting better compiler support to replace these with std::float32_t
// NOLINTBEGIN(readability-magic-numbers)
static_assert(std::numeric_limits<float>::is_iec559 && std::numeric_limits<float>::digits == 24,
              "float is not ieee 32-bit");
static_assert(std::numeric_limits<double>::is_iec559 && std::numeric_limits<double>::digits == 53,
              "double is not ieee 64-bit");
// NOLINTEND(readability-magic-numbers)

using float32_t = float;
using float64_t = double;

namespace unrealsdk {

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW || UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW64
static constexpr auto EXPECTED_POINTER_ALIGNMENT = 4;
#elif UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK
static constexpr auto EXPECTED_POINTER_ALIGNMENT = sizeof(void*);
#endif

// N Byte aligned pointer wrapper structure
template <class T>
struct alignas(EXPECTED_POINTER_ALIGNMENT) TPointer {
    static constexpr auto PTR_SIZE = sizeof(void*);
    std::array<uint8_t, PTR_SIZE> block;

    TPointer() noexcept { set<std::nullptr_t>(nullptr); }
    TPointer(std::nullptr_t) noexcept { set<std::nullptr_t>(nullptr); }

    template <class R = T>
    R* get() const noexcept {
        void* ptr{};
        std::memcpy(&ptr, block.data(), PTR_SIZE);
        return static_cast<R*>(ptr);
    }

    template <class U>
    void set(const U* ptr) noexcept {
        std::memcpy(block.data(), &ptr, PTR_SIZE);
    }

    TPointer& operator=(const T* ptr) noexcept {
        set(ptr);
        return *this;
    }

    template <class U = T>
        requires(!std::is_void_v<U>)
    const U& operator*(int /*tag*/) const noexcept {
        return *get();
    }

    template <class U = T>
        requires(!std::is_void_v<U>)
    U& operator*(int /*tag*/) noexcept {
        return *get();
    };

    TPointer operator++() noexcept {
        TPointer self = *this;
        set(get() + 1);
        return self;
    }

    TPointer operator++(int) noexcept {
        set(get() + 1);
        return *this;
    }

    operator const T*() const noexcept { return get(); }
    operator T*() noexcept { return get(); }

    bool operator==(std::nullptr_t) const noexcept { return get() == nullptr; }
    bool operator!=(std::nullptr_t) const noexcept { return get() != nullptr; }
    bool operator==(auto ptr) const noexcept { return get() == ptr; }
    bool operator!=(auto ptr) const noexcept { return get() != ptr; }

    template <class R = T>
    const R* operator->() const noexcept {
        return get<R>();
    }

    template <class R = T>
    R* operator->() noexcept {
        return get<R>();
    }
};

// generic any pointer
using Pointer = TPointer<void>;

}  // namespace unrealsdk

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW

static_assert(sizeof(uintptr_t) == sizeof(uint32_t),
              "Expected 32 bit pointers for Willow SDK flavour");

#elif UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK || UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW64
static_assert(sizeof(uintptr_t) == sizeof(uint64_t),
              "Expected 64 bit pointers for current flavour");
#else
#error Unknown sdk flavour
#endif

#endif

#endif /* UNREALSDK_PCH_H */
