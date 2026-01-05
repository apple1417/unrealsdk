#include "unrealsdk/pch.h"
#include "unrealsdk/game/bl4/bl4.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/classes/uobject.h"
#include "unrealsdk/unreal/structs/fframe.h"
#include "unrealsdk/unreal/structs/fname.h"
#include "unrealsdk/unreal/structs/fstring.h"
#include "unrealsdk/unreal/structs/ftext.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2 && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::unreal;
using namespace unrealsdk::memory;

namespace unrealsdk::game {
void BL4Hook::hook(void) {
    hook_antidebug();
    hook_call_function();
    hook_process_event();

    find_fname_funcs();
    find_gobjects();
    find_gmalloc();
    find_get_path_name();

    find_construct_object();
    find_static_find_object();
    find_load_package();
    find_fframe_step();
}

void BL4Hook::post_init(void) {
    inject_console();
}

#pragma region FFrame::Step

namespace {

using gnatives_func = void (*)(FFrame* stack, UObject* obj, void* param);

gnatives_func* gnatives_table_ptr;

const constinit Pattern<19> GNATIVES_PTR{
    "4C 8D 0D {????????}"  // lea r9, [Borderlands4.exe+C5CBDB0]
    "41 FF 14 C1"          // call qword ptr [r9+rax*8]
    "48 83 C4 ??"          // add rsp, 20
    "80 7D F8 01"          // cmp byte ptr [rbp-08], 01
};

}  // namespace

void BL4Hook::find_fframe_step(void) {
    auto gnative_inst = GNATIVES_PTR.sigscan("gnative");
    gnatives_table_ptr = read_offset<decltype(gnatives_table_ptr)>(gnative_inst);
    LOG(MISC, "FFrame::Step: {:p}", reinterpret_cast<void*>(gnatives_table_ptr));
}

void BL4Hook::fframe_step(FFrame* frame, UObject* obj, void* param) const {
    auto curr_native = *frame->Code();
    frame->Code()++;
    gnatives_table_ptr[curr_native](frame, obj, param);
}

#pragma endregion

}  // namespace unrealsdk::game

#endif
