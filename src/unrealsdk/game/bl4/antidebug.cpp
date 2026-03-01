#include "unrealsdk/pch.h"
#include "unrealsdk/config.h"
#include "unrealsdk/game/bl4/bl4.h"
#include "unrealsdk/memory.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2 && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;

namespace unrealsdk::game {

namespace {

const constexpr auto ASSUMED_MIN_PAGE_SIZE = 0x1000;

/**
 * @brief Changes any PAGE_NOACCESS pages in the given range to PAGE_READONLY.
 *
 * @param base The base address of the range to scan.
 * @param size The size of the range to scan.
 */
void remove_no_access_pages(uintptr_t base, size_t size) {
    for (uintptr_t ptr = base; ptr < (base + size);) {
        MEMORY_BASIC_INFORMATION mem;
        if (VirtualQuery(reinterpret_cast<void*>(ptr), &mem, sizeof(mem)) == 0) {
            LOG(MISC, "Failed to query memory at {:012x}", ptr);
            ptr += ASSUMED_MIN_PAGE_SIZE;
            continue;
        }
        ptr = reinterpret_cast<uintptr_t>(mem.BaseAddress) + mem.RegionSize;

        if (mem.State != MEM_COMMIT || mem.Protect != PAGE_NOACCESS) {
            continue;
        }

        LOG(MISC, "Unlocking page at {:012x}", reinterpret_cast<uintptr_t>(mem.BaseAddress));

        DWORD old_protect{};
        if (VirtualProtect(mem.BaseAddress, mem.RegionSize, PAGE_READONLY, &old_protect) == 0) {
            LOG(ERROR, "Failed to unlock page: {:x}", GetLastError());
        }
    }
}

/**
 * @brief Restores functions which've had anti-debug patches applied.
 */
void restore_antidebugd_functions(void) {
    auto ntdll = GetModuleHandleA("ntdll.dll");
    if (ntdll == nullptr) {
        LOG(DEV_WARNING, "Couldn't find ntdll.dll, couldn't restore anti-debug");
        return;
    }

    auto dbg_break_point = reinterpret_cast<uint8_t*>(GetProcAddress(ntdll, "DbgBreakPoint"));
    if (dbg_break_point != nullptr) {
        // This function is usually just:
        //   int 3
        //   ret
        // They patch it to:
        //   ret
        //   ret

        // Since these are both single byte instructions, can easily safely restore it
        unlock_range(dbg_break_point, 1);
        *dbg_break_point = 0xCC;  // NOLINT(readability-magic-numbers)
    }

    auto dbg_ui_remote_breakin =
        reinterpret_cast<uint8_t*>(GetProcAddress(ntdll, "DbgUiRemoteBreakin"));
    if (dbg_ui_remote_breakin != nullptr) {
        // This function usually starts with:
        //   sub rsp, 28
        // Again, they just patch the first byte to be a ret - which ends up mangling this (4-byte)
        // instruction.
        // Since this function is bigger, it's a little more sketchy restoring it. However, I think
        // it's very likely it will always start with sub to make stack space, and even if that
        // changes a REX prefix is still quite likely, so we should be able to get away with this.
        unlock_range(dbg_ui_remote_breakin, 1);
        *dbg_ui_remote_breakin = 0x48;  // NOLINT(readability-magic-numbers)
    }
}

const constexpr Pattern<13> SYMBIOTE_ENTRY_POINT_PATTERN{
    "E8 ????????"  // call 168C5A9D9
    "55"           // push rbp
    "48 8B EC"     // mov rbp, rsp
    "48 83 E4 F0"  // and rsp, -10
};

[[noreturn]] void symbiote_hook(void) {
    // In case these get edited between init and this call?
    restore_antidebugd_functions();

    // Since this is a hook, the only way we're running this is if we are the symbiote thread
    LOG(MISC, "Killing symbiote thread");
    TerminateThread(GetCurrentThread(), 0);

    // Extra safety?
    while (true) {}
}

}  // namespace

void BL4Hook::hook_antidebug(void) {
    // There's a couple of no-access pages in the exe range. They seem to correspond to the .impdata
    // and .trace sections - I don't think these are explicitly meant as antidebug, but they block
    // our sigscans. Set them to read only before we do anything else.
    auto [base, size] = get_exe_range();
    remove_no_access_pages(base, size);

    // Note that disabling symbiote is known to cause a crash on SQ - hence disabled by default
    if (!unrealsdk::config::get_bool("unrealsdk.bl4_debug.enabled").value_or(false)) {
        return;
    }

    // The symbiote thread watches for us to revert their anti-debug changes.
    // Block it so that we can actually do so.
    const uintptr_t symbiote_entry_point =
        unrealsdk::config::get_int("unrealsdk.bl4_debug.symbiote_addr")
            .value_or(SYMBIOTE_ENTRY_POINT_PATTERN.sigscan("symbiote entry point"));
    // Don't care to keep the original around
    // NOLINTNEXTLINE(misc-const-correctness) // clang-tidy 21 false positive?
    void (*symbiote_ptr)(void) = nullptr;
    detour(symbiote_entry_point, symbiote_hook, &symbiote_ptr, "symbiote thread");

    restore_antidebugd_functions();

    if (unrealsdk::config::get_bool("unrealsdk.bl4_debug.wait_for_debugger").value_or(false)) {
        LOG(DEV_WARNING, "Waiting for debugger to attach");
        while (IsDebuggerPresent() == 0) {
            // NOLINTNEXTLINE(readability-magic-numbers)
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        LOG(DEV_WARNING, "Got debugger");

        // Seems to still be something detecting breakpoints and killing everything
        // DebugBreak();
    }
}

}  // namespace unrealsdk::game

#endif
