#include "unrealsdk/pch.h"
#include "unrealsdk/game/bl1/bl1.h"

#include "unrealsdk/hook_manager.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/structs/fframe.h"
#include "unrealsdk/version_error.h"

#if defined(UE3) && defined(ARCH_X86) && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {

// These could be defined in the class but since they are only used here this will do for now.
namespace {

std::atomic_bool bl1_has_initialised{false};

using Clock = std::chrono::steady_clock;

bool hook_init_func(void);
}  // namespace

void BL1Hook::hook(void) {
    hook_antidebug();

    if (!hook_init_func()) {
        return;
    }

    hook_process_event();
    hook_call_function();

    // Grabbing these asap seems fine
    find_fname_init();
    find_fframe_step();
    find_construct_object();
    find_get_path_name();
    find_static_find_object();
    find_load_package();

    // idk if these are required or if they are correct
    hexedit_set_command();
    hexedit_array_limit();
    hexedit_array_limit_message();

    // This ensures that the unrealscript is initialised when we exit/return
    while (!bl1_has_initialised.load(std::memory_order_relaxed)) {
        // NOLINTNEXTLINE(readability-magic-numbers)
        std::this_thread::sleep_for(std::chrono::milliseconds{50});
    }

    // Delayed until after initialisation to ensure we grab valid data; Should be able to avoid this
    // and grab earlier if we grab a pointer to the global not the value held by the global.
    find_gobjects();
    find_gnames();
    find_gmalloc();
}

void BL1Hook::post_init(void) {
    LOG(MISC, "Attaching Hooks!");
    inject_console();
}

// ############################################################################//
//  | FFRAME STEP |
// ############################################################################//

namespace {

// - NOTE -
// This is inlined so we have to manually re-implement FFrame::Step using GNatives.
const constinit Pattern<36> FFRAME_STEP_SIG{
    "74 ??"              // je borderlands.59CEA4
    "8B45 D4"            // mov eax,dword ptr ss:[ebp-2C]
    "830D ???????? 02"   // or dword ptr ds:[1F73BE8],2
    "0FB610"             // movzx edx,byte ptr ds:[eax]
    "8B1495 {????????}"  // mov edx,dword ptr ds:[edx*4+1F97A80]
    "57"                 // push edi
    "8D4D BC"            // lea ecx,dword ptr ss:[ebp-44]
    "40"                 // inc eax
    "51"                 // push ecx
    "8B4D EC"            // mov ecx,dword ptr ss:[ebp-14]
    "8945 D4"            // mov dword ptr ss:[ebp-2C],eax
    "FFD2"               // call edx
};

// RET 0x8; Callee cleans up the stack (8 bytes)
// NOLINTNEXTLINE(modernize-use-using)
typedef void(__stdcall* fframe_step_func)(FFrame*, void*);
fframe_step_func** fframe_step_gnatives;

}  // namespace

void BL1Hook::find_fframe_step(void) {
    fframe_step_gnatives = FFRAME_STEP_SIG.sigscan_nullable<fframe_step_func**>();

    if (fframe_step_gnatives == nullptr) {
        LOG(ERROR, "FFrame::Step(...), GNatives was null.");
        return;
    }

    LOG(MISC, "FFrame::Step: {:p}", reinterpret_cast<void*>(fframe_step_gnatives));
}

void BL1Hook::fframe_step(unreal::FFrame* frame, unreal::UObject* /*obj*/, void* param) const {
    ((*fframe_step_gnatives)[*frame->Code++])(frame, param);
}

// ############################################################################//
//  | FNAME INIT |
// ############################################################################//

#if defined(__MINGW32__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"  // thiscall on non-class
#endif

namespace {

const constinit Pattern<34> FNAME_INIT_SIG{
    "6A FF"            // push FFFFFFFF
    "68 ????????"      // push borderlands.18EB45B
    "64A1 00000000"    // mov eax,dword ptr fs:[0]
    "50"               // push eax
    "81EC 980C0000"    // sub esp,C98
    "A1 ????????"      // mov eax,dword ptr ds:[1F16980]
    "33C4"             // xor eax,esp
    "898424 940C0000"  // mov dword ptr ss:[esp+C94],eax
};

// NOLINTNEXTLINE(modernize-use-using)
typedef void(__thiscall* fname_init_func)(FName* name,
                                          const wchar_t* str,
                                          int32_t number,
                                          int32_t find_type,
                                          int32_t split_name);

fname_init_func fname_init_ptr = nullptr;

}  // namespace

void BL1Hook::find_fname_init(void) {
    fname_init_ptr = FNAME_INIT_SIG.sigscan_nullable<fname_init_func>();
    LOG(MISC, "FName::Init: {:p}", (void*)fname_init_ptr);
}

void BL1Hook::fname_init(unreal::FName* name, const wchar_t* str, int32_t number) const {
    fname_init_ptr(name, str, number, 1, 1);
}

#if defined(__MINGW32__)
#pragma GCC diagnostic pop
#endif

// ############################################################################//
//  | CUSTOM HOOKS |
// ############################################################################//

namespace {

// - NOTE -
// The init function is only used to delay initialisation of the SDK to ensure that we can proceed
// with injection at the right time. However, the Steams version of this function is different
// the core functionality though is the same.

const constinit Pattern<64> INIT_FUNC_STEAM_SIG{
    "6A FF"          // push FFFFFFFF
    "68 ????????"    // push borderlands.198D998
    "64A1 00000000"  // mov eax,dword ptr fs:[0]
    "50"             // push eax
    "83EC 3C"        // sub esp,3C
    "53"             // push ebx
    "55"             // push ebp
    "56"             // push esi
    "57"             // push edi
    "A1 ????????"    // mov eax,dword ptr ds:[1F131C0]
    "33C4"           // xor eax,esp
    "50"             // push eax
    "8D4424 50"      // lea eax,dword ptr ss:[esp+50]
    "64A3 00000000"  // mov dword ptr fs:[0],eax
    "8BD9"           // mov ebx,ecx
    "68 ????????"    // push borderlands.1CC3E78
    "E8 ????????"    // call borderlands.5C20F0
    "50"             // push eax
    "E8 ????????"    // call borderlands.5C2A80
    "83C4 08"        // add esp,8
    "85C0"           // test eax,eax
    "74 ??"          // je borderlands.138F06A
};

const constinit Pattern<45> INIT_FUNC_141_UDK_SIG{
    "6A FF"          // push FFFFFFFF
    "68 ????????"    // push <borderlands.sub_1991338>
    "64A1 00000000"  // mov eax,dword ptr fs:[0]
    "50"             // push eax
    "83EC 3C"        // sub esp,3C
    "53"             // push ebx
    "55"             // push ebp
    "56"             // push esi
    "57"             // push edi
    "A1 ????????"    // mov eax,dword ptr ds:[1F16980]
    "33C4"           // xor eax,esp
    "50"             // push eax
    "8D4424 50"      // lea eax,dword ptr ss:[esp+50]
    "64A3 00000000"  // mov dword ptr fs:[0],eax
    "8BD9"           // mov ebx,ecx
    "EB ??"          // jmp borderlands.13ADAC9
    "4D"             // dec ebp
    "61"             // popad
};

// Not sure if this is a __thiscall or an actual __fastcall; ecx is used.
// NOLINTNEXTLINE(modernize-use-using)
typedef void(__fastcall* init_function)(void* ecx, void* edx);

init_function init_func_ptr = nullptr;

void __fastcall detour_init_func(void* ecx, void* edx) {
    auto start = Clock::now();
    LOG(INFO, "Init function called");
    init_func_ptr(ecx, edx);
    LOG(INFO, "Init function took {}s to execute",
        std::chrono::duration<float>(Clock::now() - start).count());

    // When this is true the unrealscript game engine has been created
    bl1_has_initialised.store(true, std::memory_order_relaxed);
}

bool hook_init_func(void) {
    // - NOTE -
    // I don't think is actually has to be in a loop.
    //
    constexpr float init_func_timeout_seconds{15.0F};

    // Note: The time requested is still constrained to the OS; i.e., min on windows is 16ms
    // this should just be 'as fast as possible' so 20ms for all systems should suffice.
    constexpr std::chrono::milliseconds delay{20};

    LOG(INFO, "Attempting to hook init function with polling rate: {}ms", delay.count());

    auto start = Clock::now();

    while (true) {
        std::this_thread::sleep_for(delay);

        // Steam
        uintptr_t addr = INIT_FUNC_STEAM_SIG.sigscan_nullable();
        if (addr != 0) {
            if (!detour(addr, &detour_init_func, &init_func_ptr, "bl1_hook_steam_init_func")) {
                continue;
            }
            return true;
        }

        // UDK 141
        addr = INIT_FUNC_141_UDK_SIG.sigscan_nullable();
        if (addr != 0) {
            if (!detour(addr, &detour_init_func, &init_func_ptr, "bl1_hook_udk_init_func")) {
                continue;
            }
            return true;
        }

        // This should never really be hit adding it just incase though
        if (std::chrono::duration<float>(Clock::now() - start).count()
            > init_func_timeout_seconds) {
            LOG(ERROR, "It has taken too long to hook the init function; Aborting...");
            return false;
        }
    }
}

}  // namespace

// ############################################################################//
//  | NOT IMPLEMENTED |
// ############################################################################//

void BL1Hook::ftext_as_culture_invariant(unreal::FText* /*text*/,
                                         TemporaryFString&& /*str*/) const {
    throw_version_error("FTexts are not implemented in UE3");
}

}  // namespace unrealsdk::game

#endif
