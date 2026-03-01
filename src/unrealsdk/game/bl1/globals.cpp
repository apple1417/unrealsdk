#include "unrealsdk/pch.h"

#include "unrealsdk/game/bl1/bl1.h"
#include "unrealsdk/game/bl1/offsets.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/wrappers/gobjects.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {

namespace {

GObjects gobjects_wrapper{};

const constinit Pattern<18> GOBJECTS_SIG{
    "8B 0D {????????}"  // mov ecx, [01FB4DD8]
    "8B 04 ??"          // mov eax, [ecx+esi*4]
    "8B 50 ??"          // mov edx, [eax+0C]
    "21 58 ??"          // and [eax+08], ebx
    "89 50 ??"          // mov [eax+0C], edx
};

}  // namespace

void BL1Hook::find_gobjects(void) {
    auto gobjects_ptr = read_offset<GObjects::internal_type>(GOBJECTS_SIG.sigscan_nullable());
    LOG(MISC, "GObjects: {:p}", reinterpret_cast<void*>(gobjects_ptr));

    gobjects_wrapper = GObjects(gobjects_ptr);
}

const GObjects& BL1Hook::gobjects(void) const {
    return gobjects_wrapper;
};

namespace {

const constinit Pattern<21> GNAMES_SIG{
    "A1 {????????}"  // mov eax, [01FB4DA8]
    "8B 0C ??"       // mov ecx, [eax+esi*4]
    "68 ????????"    // push 00001000
    "6A ??"          // push 00
    "E8 ????????"    // call 005C21F0
    "5E"             // pop esi
};

TArray<bl1::FNameEntry*>* gnames_ptr;

}  // namespace

void BL1Hook::find_gnames(void) {
    gnames_ptr = read_offset<decltype(gnames_ptr)>(GNAMES_SIG.sigscan_nullable());
    LOG(MISC, "GNames: {:p}", reinterpret_cast<void*>(gnames_ptr));
}

[[nodiscard]] std::variant<const std::string_view, const std::wstring_view> BL1Hook::fname_get_str(
    const unreal::FName& name) const {
    auto entry = gnames_ptr->at(name.index);
    if ((entry->Index & FNameEntry::NAME_WIDE_MASK) != 0) {
        return std::wstring_view{&entry->Name.Wide[0]};
    }
    return std::string_view{&entry->Name.Ansi[0]};
}

}  // namespace unrealsdk::game

#endif
