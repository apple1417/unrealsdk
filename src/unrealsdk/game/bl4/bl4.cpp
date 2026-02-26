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
    find_ftext_as_culture_invariant();
}

void BL4Hook::post_init(void) {
    inject_console();
}

#pragma region FFrame::Step

namespace {

using gnatives_func = void (*)(UObject* obj, FFrame* stack, void* param);

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
    gnatives_table_ptr[curr_native](obj, frame, param);
}

#pragma endregion

#pragma region FText::AsCultureInvariant

namespace {

using ftext_as_culture_invariant_func = void (*)(FText* self, const wchar_t* str);
ftext_as_culture_invariant_func ftext_as_culture_invariant_ptr;

const constinit Pattern<30> FTEXT_AS_CULTURE_INVARIANT_PATTERN{
    "56"                 // push rsi
    "48 83 EC ??"        // sub rsp, 40
    "48 89 CE"           // mov rsi, rcx
    "48 8B 05 ????????"  // mov rax, [Borderlands4.exe+C372940]
    "48 31 E0"           // xor rax, rsp
    "48 89 44 24 ??"     // mov [rsp+38], rax
    "48 89 54 24 ??"     // mov [rsp+28], rdx
    "31 C0"              // xor eax, eax
};

}  // namespace

void BL4Hook::find_ftext_as_culture_invariant(void) {
    ftext_as_culture_invariant_ptr =
        FTEXT_AS_CULTURE_INVARIANT_PATTERN.sigscan_nullable<ftext_as_culture_invariant_func>();
    LOG(MISC, "FText::AsCultureInvariant: {:p}",
        reinterpret_cast<void*>(ftext_as_culture_invariant_ptr));
}

// This is fine, since we consume it when calling the native function
// The rvalue will live for the lifetime of this function call
// NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
void BL4Hook::ftext_as_culture_invariant(FText* text, TemporaryFString&& str) const {
    ftext_as_culture_invariant_ptr(text, str.data);
}

#pragma endregion

}  // namespace unrealsdk::game

#endif
