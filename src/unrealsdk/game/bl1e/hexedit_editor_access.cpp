#include "unrealsdk/pch.h"
#include "unrealsdk/game/bl1e/bl1e.h"

#include "unrealsdk/memory.h"
#include "unrealsdk/utils.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW && !defined(UNREALSDK_IMPORTING)

namespace unrealsdk::game {

using namespace unrealsdk::memory;

namespace {

constexpr std::chrono::seconds FALLBACK_DELAY{5};
bool should_perform_editor_patches{false};
bool should_use_static_delay{false};

void block_till_ready(void);

// NOTE
//  These patches alone *do not* make the editor work. Launching the game with -editor will crash
//  even after these patches have been applied. This is because there is a file system setup step
//  required. It needs the Master Shaders from the original game because it doesn't have them. This
//  is why I think these patches could be done outside of the SDK since that file copy step will
//  always be required.
//

// NOP's out a `GIsEditor = false` call that happens at startup; this one prevents the -editor flag
// from actually launching the editor.
void hexedit_prevent_iseditor_unsetting(void);

// When attempting to get into the editor the MMG Upsell system fails to initialise resulting in a
// crash at two places during startup. This patches those places out since they are not required for
// the editor to function.
//
// I can only assume this is related to their networking/ad service. Tracing around you can find
// these strings 'sparkenv=', 'steam', 'mopane', 'url_learn_more_loc_', 'disc_img_game_lg', and
// 'epic_sandbox_id' ; first 'crash' is actually just a breakpoint (int3) second is bad data due to
// the initialisation failing leaving the global pointer internal data in an invalid state.
void hexedit_disable_mmg_upsell(void);

// Since the editor is not being initialised properly the GCallbackQuery has a default
// implementation that does `return 0/false;` for all queries. We patch one of the usages of the
// callback query to ignore the result.
void hexedit_allow_savepackage(void);

void perform_hexedits() {
    hexedit_prevent_iseditor_unsetting();
    hexedit_disable_mmg_upsell();
    hexedit_allow_savepackage();
}

}  // namespace

void BL1EHook::hexedit_editor_access() {
    block_till_ready();

    // this check is required since these patches actually make the -editor flag redundant; It will
    // always open the editor if these patches are applied.
    if (should_perform_editor_patches) {
        perform_hexedits();
    }

    if (should_use_static_delay) {
        std::this_thread::sleep_for(FALLBACK_DELAY);
    }
}

namespace {

constexpr size_t ASM_CALL_INSTRUCTION_LENGTH = 6;
constexpr uint8_t ASM_NOOP = 0x90;

////////////////////////////////////////////////////////////////////////////////
// | IMPL |
////////////////////////////////////////////////////////////////////////////////

void hexedit_prevent_iseditor_unsetting(void) {
    // 89 35 ?? ?? ?? ?? 89 35 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B C6
    constexpr Pattern<26> preinit_unset_globals{
        "89 35 ?? ?? ?? ?? 89 35 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B C6"};

    // 0F 85 17 02 00 00 E8 0D 59 A6 FF 48 8B 48 54 83 79 5C 04
    auto* code = preinit_unset_globals.sigscan<uint8_t*>("GIsEditor = false patch");

    // MOV  GIsEditor, 0x0
    unlock_range(code, ASM_CALL_INSTRUCTION_LENGTH);
    std::memset(code, ASM_NOOP, ASM_CALL_INSTRUCTION_LENGTH);
}

void hexedit_disable_mmg_upsell(void) {
    // Its tempting to see if we can NOP the E8 ?? ?? ?? ?? since this'll prevent the initialisation
    // entirely. But does mean that the global pointer will be undefined or maybe null?
    constexpr Pattern<20> mmg_upsell_init{
        "CC E8 ?? ?? ?? ?? 85 C0 75 ?? 48 8D 15 ?? ?? ?? ?? 48 8B 0D"};
    auto* code = mmg_upsell_init.sigscan<uint8_t*>("mmg_upsell patch");

    constexpr size_t if_jump_index = 8;
    constexpr size_t while_jump_index = 44;
    constexpr uint8_t asm_jmp_bytecode = 0xEB;
    constexpr uint8_t asm_jmp_zero_bytecode = 0x74;

    // First jump is not taken if initialisation of the MMG fails; its the E8 ?? ?? ?? ?? sequence.
    // When launching with -editor this initialisation fails unsure why, didn't look into it. So we
    // just force the jump here since its an external non-editor related functionality.
    unlock_range(code + if_jump_index, 1);
    code[if_jump_index] = asm_jmp_bytecode;

    // it performs a busy-waiting loop but since the initialisation failed some of the data is bad
    // and that causes a GPF. So just don't let it get into that busy-waiting loop by forcing a jmp.
    // TODO: Don't really want another sigscan just for this since its basically right next to the
    //  one we just did but also don't want to extend the main signature to be bigger than it is.
    unlock_range(code + while_jump_index, 1);
    if (code[while_jump_index] == asm_jmp_zero_bytecode) {
        code[while_jump_index] = asm_jmp_bytecode;
    } else {
        LOG(ERROR, "expecting 0x74 at while jump index but got {:x}", code[while_jump_index]);
    }
}

void hexedit_allow_savepackage(void) {
    // For reference we are in:
    //   UBOOL UEditorEngine::Exec_Obj( const TCHAR* Str, FOutputDevice& Ar )
    //
    // we are patching:
    //
    //   if( !GCallbackQuery->Query(CALLBACK_AllowPackageSave, Pkg )) {
    //       return FALSE;
    //   }
    //
    // since the GCallbackQuery is default initialised with a return 0/false implementation for all
    //  its virtual functions.
    //
    constexpr Pattern<17> sig_query_callback{"48 8B 01 4C 8B 45 D7 41 8B D7 FF 50 10 85 C0 0F 84"};
    constexpr size_t offset = 15;
    constexpr size_t asm_jz_instruction_length = 6;

    auto* code = sig_query_callback.sigscan<uint8_t*>("editor save package patch") + offset;
    unlock_range(code, asm_jz_instruction_length);
    std::memset(code, ASM_NOOP, asm_jz_instruction_length);
}

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// | BLOCK TILL READY |
////////////////////////////////////////////////////////////////////////////////

namespace {

// the signature matches two things, both are in the same function; not sure on the name for this
// function as I don't know a whole lot about the startup process but its probably the same one used
// in BL1 ___tmainCRTStartup
constexpr Pattern<19> SIG_ENTRY_FUNCTION{
    "48 8B D8 48 83 38 00 74 ?? 48 8B C8 E8 ?? ?? ?? ?? 84 C0"};

std::atomic ready = false;
std::mutex ready_mutex{};
std::condition_variable ready_cv{};

// NOLINTBEGIN(readability-identifier-naming)

// GetCommandLineW should be an alternative to this
using IsDebuggerPresent_func = BOOL(WINAPI*)(void);
IsDebuggerPresent_func IsDebuggerPresent_ptr{nullptr};

BOOL WINAPI IsDebuggerPresent_hook() {
    const BOOL res = IsDebuggerPresent_ptr();

    if (ready.load() || SIG_ENTRY_FUNCTION.sigscan_nullable() == 0) {
        return res;
    }

    const std::scoped_lock lock{ready_mutex};
    ready.store(true);
    ready_cv.notify_all();
    return res;
}

// NOLINTEND(readability-identifier-naming)

void block_till_ready(void) {
    std::unique_lock lock{ready_mutex};

    {
        const utils::ThreadSuspender suspender{};

        std::string args{GetCommandLineA()};
        std::ranges::transform(args, args.begin(), ::tolower);
        should_perform_editor_patches = (args.find(" -editor") != std::string::npos);

        if (SIG_ENTRY_FUNCTION.sigscan_nullable() != 0) {
            LOG(INFO, "Entry signature already exists");
            // a bit excessive but might as well try the patches while we still have everything
            //  suspended.
            if (should_perform_editor_patches) {
                should_perform_editor_patches = false;
                perform_hexedits();
            }
            return;
        }

        MH_STATUS status = MH_OK;
        const auto hook_func = reinterpret_cast<uintptr_t>(&IsDebuggerPresent);
        status = MH_CreateHook(reinterpret_cast<LPVOID>(hook_func),
                               reinterpret_cast<LPVOID>(&IsDebuggerPresent_hook),
                               reinterpret_cast<LPVOID*>(&IsDebuggerPresent_ptr));

        if (status != MH_OK) {
            LOG(WARNING, "Failed to create IsDebuggerPresent hook: {:x}",
                static_cast<uint32_t>(status));
            should_perform_editor_patches = false;
            should_use_static_delay = true;
            return;
        }

        status = MH_EnableHook(reinterpret_cast<LPVOID>(hook_func));
        if (status != MH_OK) {
            LOG(WARNING, "Failed to enable IsDebuggerPresent hook: {:x}",
                static_cast<uint32_t>(status));
            should_perform_editor_patches = false;
            should_use_static_delay = true;
            return;
        }
    }

    should_use_static_delay = false;
    ready_cv.wait(lock, [] { return ready.load(); });
    // not going to disable the hook since I want to minimise the chance of the next hooks failing.
    // We should have more leeway than we will ever need but you never know. Its also not a function
    // that'll matter if it remains hooked.
}

}  // namespace

}  // namespace unrealsdk::game

#endif