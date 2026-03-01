#ifndef UNREALSDK_PCH_H
#define UNREALSDK_PCH_H

// Include the C exports library first, so we can use it everywhere
// This file is purely macros, it doesn't rely on anything else
#include "unrealsdk/exports.h"

// Similarly, this is pure macros, and we want to access feature flags everywhere
#include "unrealsdk/flavour.h"

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
#include <variant>

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

// This is mostly just a sanity check of your build environment
#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK
static_assert(sizeof(uintptr_t) == sizeof(uint64_t),
              "Expected 64 bit pointers for Oak SDK flavour");
#elif UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2
static_assert(sizeof(uintptr_t) == sizeof(uint64_t),
              "Expected 64 bit pointers for Oak2 SDK flavour");
#elif UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW
// Willow supports both 32-bit and 64-bit pointers
#else
#error Unknown sdk flavour
#endif

#endif

#endif /* UNREALSDK_PCH_H */
