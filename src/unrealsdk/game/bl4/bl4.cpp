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

#pragma region FText::AsCultureInvariant

namespace {

using ftext_as_culture_invariant_func = void (*)(FText* self, const TemporaryFString* str);
ftext_as_culture_invariant_func ftext_as_culture_invariant_ptr;

const constinit Pattern<50> FTEXT_AS_CULTURE_INVARIANT_PATTERN{
    "56"           // push rsi
    "57"           // push rdi
    "48 83 EC 28"  // sub rsp, 28
    "48 89 CE"     // mov rsi, rcx
    "83 7A 08 01"  // cmp dword ptr [rdx+08], 01
    "7E ??"        // jle Borderlands4.exe+14553C701
    "48 89 F1"     // mov rcx, rsi
    "E8 ????????"  // call Borderlands4.exe+14000A394
    "48 8D 7E 08"  // lea rdi, [rsi+08]
    "8B 07"        // mov eax, [rdi]
    "83 C8 02"     // or eax, 02
    "89 46 08"     // mov [rsi+08], eax
    "48 89 F0"     // mov rax, rsi
    "48 83 C4 28"  // add rsp, 28
    "5F"           // pop rdi
    "5E"           // pop rsi
    "C3"           // ret
    "E8 ????????"  // call Borderlands4.exe+14554C72A
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
    ftext_as_culture_invariant_ptr(text, &str);
}

#pragma endregion

}  // namespace unrealsdk::game

#endif
