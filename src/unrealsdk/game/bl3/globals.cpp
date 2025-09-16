#include "unrealsdk/pch.h"

#include "unrealsdk/game/bl3/bl3.h"
#include "unrealsdk/game/bl3/offsets.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/structs/gnames.h"
#include "unrealsdk/unreal/wrappers/gobjects.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::unreal;
using namespace unrealsdk::memory;

namespace unrealsdk::game {

namespace {

GObjects gobjects_wrapper{};

const constinit Pattern<26> GOBJECTS_SIG{
    "48 8D 0D {????????}"  // lea rcx, [Borderlands3.exe+69EBDA0]
    "C6 05 ???????? 01"    // mov byte ptr [Borderlands3.exe+69EA290], 01
    "E8 ????????"          // call Borderlands3.exe+17854D0
    "C6 05 ???????? 01"    // mov byte ptr [Borderlands3.exe+64B78E0], 01
};

}  // namespace

void BL3Hook::find_gobjects(void) {
    auto gobjects_ptr = read_offset<GObjects::internal_type>(GOBJECTS_SIG.sigscan_nullable());
    LOG(MISC, "GObjects: {:p}", reinterpret_cast<void*>(gobjects_ptr));

    gobjects_wrapper = GObjects(gobjects_ptr);
}

const GObjects& BL3Hook::gobjects(void) const {
    return gobjects_wrapper;
};

namespace {

const constinit Pattern<27> GNAMES_SIG{
    "E8 ????????"          // call Borderlands3.exe+3DDBB7C
    "48 8B C3"             // mov rax, rbx
    "48 89 1D {????????}"  // mov [Borderlands3.exe+69E71E8], rbx
    "48 8B 5C 24 ??"       // mov rbx, [rsp+20]
    "48 83 C4 28"          // add rsp, 28
    "C3"                   // ret
    "33 DB"                // xor ebx, ebx
};
TStaticIndirectArrayThreadSafeRead_FNameEntry* gnames_ptr;

}  // namespace

void BL3Hook::find_gnames(void) {
    // Using plain `sigscan` since there's an extra level of indirection here, want to make sure to
    // print an error before we potentially dereference it
    gnames_ptr = *read_offset<decltype(gnames_ptr)*>(GNAMES_SIG.sigscan("GNames"));
    LOG(MISC, "GNames: {:p}", reinterpret_cast<void*>(gnames_ptr));
}

[[nodiscard]] std::variant<const std::string_view, const std::wstring_view> BL3Hook::fname_get_str(
    const unreal::FName& name) const {
    auto entry = reinterpret_cast<bl3::FNameEntry*>(gnames_ptr->at(name.index));
    if ((entry->Index & FNameEntry::NAME_WIDE_MASK) != 0) {
        return std::wstring_view{&entry->Name.Wide[0]};
    }
    return std::string_view{&entry->Name.Ansi[0]};
}

}  // namespace unrealsdk::game

#endif
