#include "unrealsdk/pch.h"
#include "unrealsdk/game/bl1e/bl1e.h"

#include "unrealsdk/memory.h"
#include "unrealsdk/utils.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW && !defined(UNREALSDK_IMPORTING)

namespace unrealsdk::game {

using namespace unrealsdk::memory;

namespace {

// NOTE
//  These patches alone *do not* make the editor work. Launching the game with -editor will crash
//  even after these patches have been applied. This is because there is a file system setup step
//  required for the editor to launch at all. It needs the Master Shaders from the original game
//  because it doesn't have them. This is why I think these patches can be done outside of the SDK
//  since that file copy step will always be required.
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

}  // namespace

void BL1EHook::hexedit_editor_access() {
    constexpr size_t arbitrary_limit = 100;

    // TODO: A more suitable solution would be similar to steamdrm.cpp for BL1
    // TODO: The waiting here is unrelated to what we actually want to do in this function...
    // TODO: A separate DLL for these patch might be more suitable since only mod developers
    //  actually care about accessing the editor. But we can also just check if the -editor argument
    //  has been provided and if so conditionally perform these patches.
    // -
    // We are just waiting for `void entry(void)` to be decrypted at which point we suspend all
    // threads and then perform our patches.
    constexpr Pattern<20> test{"48 83 EC 28 E8 ?? ?? ?? ?? 48 83 C4 28 E9 ?? ?? ?? ?? CC CC"};
    for (size_t i = 0; i < arbitrary_limit; ++i) {
        if (test.sigscan_nullable() != 0) {
            // in order of priority
            const utils::ThreadSuspender suspend{};

            std::string args{GetCommandLineA()};
            std::ranges::transform(args, args.begin(), ::tolower);

            if (args.find_first_of(" -editor") == std::string::npos) {
                LOG(INFO, "-editor argument not provided, skipping editor related patches");
                return;
            }

            LOG(INFO, "patching unreal editor");
            hexedit_prevent_iseditor_unsetting();
            hexedit_disable_mmg_upsell();
            hexedit_allow_savepackage();
            return;
        }
        std::this_thread::yield();
    }
}

namespace {

constexpr size_t asm_call_instruction_length = 6;
constexpr uint8_t asm_noop = 0x90;

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
    unlock_range(code, asm_call_instruction_length);
    std::memset(code, asm_noop, asm_call_instruction_length);
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
    //  one we just did but also don't want to extend the main signature to be bigger than it is;
    //  this approach is not ideal either. Would like to atleast check the byte being overwritten.
    unlock_range(code + while_jump_index, 1);
    if (code[while_jump_index] == asm_jmp_zero_bytecode) {
        code[while_jump_index] = asm_jmp_bytecode;
    } else {
        LOG(ERROR, "expecting 0x74 at while jump index but got 0x{:#}", code[while_jump_index]);
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
    // since the GCallbackQuery is default initialised with a return false; implemntation
    //
    constexpr Pattern<17> sig_query_callback{"48 8B 01 4C 8B 45 D7 41 8B D7 FF 50 10 85 C0 0F 84"};
    constexpr size_t offset = 15;
    constexpr size_t asm_jz_instruction_length = 6;

    auto* code = sig_query_callback.sigscan<uint8_t*>("editor save package patch") + offset;
    unlock_range(code, asm_jz_instruction_length);
    std::memset(code, asm_noop, asm_jz_instruction_length);
}

}  // namespace

}  // namespace unrealsdk::game

#endif