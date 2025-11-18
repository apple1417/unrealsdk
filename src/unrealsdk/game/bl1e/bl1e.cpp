#include "unrealsdk/pch.h"
#include "unrealsdk/game/bl1e/bl1e.h"

#include "unrealsdk/hook_manager.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/structs/fframe.h"
#include "unrealsdk/version_error.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {
void BL1EHook::hook(void) {
    // hook_antidebug();

    hook_process_event();
    hook_call_function();

    find_gmalloc();
    find_gobjects();
    find_gnames();
    find_fname_init();
    find_fframe_step();
    find_construct_object();
    find_get_path_name();
    find_static_find_object();
    find_load_package();

    // hexedit_set_command();
    // hexedit_array_limit();
}

void BL1EHook::post_init(void) {
    inject_console();
}

namespace {

// clang-format off

const constinit Pattern<54> FNAME_INIT_SIG{
"4055565741544155415641574881ECE00C000048C7442428FEFFFFFF48899C24300D0000488B05456E34024833C448898424D00C0000"
};

const constinit Pattern<45> GNATIVES_SIG{ // TODO: validate
"48894424304C8D442430488B4224488BF9488B4A1C488BDA440FB60848FFC048894224418BC14C8D0D{3FDC2301}"
};

// clang-format on

#ifdef __MINGW32__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"  // thiscall on non-class
#endif

// NOLINTNEXTLINE(modernize-use-using)
typedef void(__thiscall* fframe_step_func)(UObject*, FFrame*, void*);
fframe_step_func** fframe_step_gnatives;

#ifdef __MINGW32__
#pragma GCC diagnostic pop
#endif
}  // namespace

void BL1EHook::find_fframe_step(void) {
    fframe_step_gnatives = GNATIVES_SIG.sigscan_nullable<fframe_step_func**>();
    LOG(MISC, "GNatives: {:p}", reinterpret_cast<void*>(fframe_step_gnatives));
}

void BL1EHook::fframe_step(FFrame* frame, UObject* obj, void* param) const {
    (*fframe_step_gnatives)[*frame->Code++](obj, frame, param);
}

namespace {

#ifdef __MINGW32__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"  // thiscall on non-class
#endif

// NOLINTNEXTLINE(modernize-use-using)
typedef void(__thiscall* fname_init_func)(FName* name,
                                          const wchar_t* str,
                                          int32_t number,
                                          int32_t find_type,
                                          int32_t split_name);

fname_init_func fname_init_ptr = nullptr;

#ifdef __MINGW32__
#pragma GCC diagnostic pop
#endif

}  // namespace

void BL1EHook::find_fname_init(void) {
    fname_init_ptr = FNAME_INIT_SIG.sigscan_nullable<fname_init_func>();
    LOG(MISC, "FName::Init: {:p}", reinterpret_cast<void*>(fname_init_ptr));
}

void BL1EHook::fname_init(FName* name, const wchar_t* str, int32_t number) const {
    fname_init_ptr(name, str, number, 1, 1);
}

void BL1EHook::ftext_as_culture_invariant(FText* /*text*/, TemporaryFString&& /*str*/) const {
    throw_version_error("FTexts are not implemented in UE3");
}

}  // namespace unrealsdk::game

#endif
