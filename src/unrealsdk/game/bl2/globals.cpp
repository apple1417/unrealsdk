#include "unrealsdk/pch.h"

#include "unrealsdk/game/bl2/bl2.h"
#include "unrealsdk/game/bl2/offsets.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/structs/gnames.h"
#include "unrealsdk/unreal/wrappers/gobjects.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {

namespace {

GObjects gobjects_wrapper{};

const constinit Pattern<17> GOBJECTS_SIG{
    "8B 0D {????????}"  // mov ecx, [Borderlands2.exe+1682BD0]
    "8B 04 ??"          // mov eax, [ecx+esi*4]
    "8B 40 ??"          // mov eax, [eax+08]
    "25 00020000"       // and eax, 00000200
};

}  // namespace

void BL2Hook::find_gobjects(void) {
    auto gobjects_ptr = read_offset<GObjects::internal_type>(GOBJECTS_SIG.sigscan_nullable());
    LOG(MISC, "GObjects: {:p}", reinterpret_cast<void*>(gobjects_ptr));

    gobjects_wrapper = GObjects(gobjects_ptr);
}

const GObjects& BL2Hook::gobjects(void) const {
    return gobjects_wrapper;
};

namespace {

const constinit Pattern<10> GNAMES_SIG{
    "A3 {????????}"  // mov [BorderlandsPreSequel.exe+1520214], eax
    "8B 45 ??"       // mov eax, [ebp+10]
    "89 03"          // mov [ebx], eax
};
TArray<bl2::FNameEntry*>* gnames_ptr;

}  // namespace

void BL2Hook::find_gnames(void) {
    gnames_ptr = read_offset<decltype(gnames_ptr)>(GNAMES_SIG.sigscan_nullable());
    LOG(MISC, "GNames: {:p}", reinterpret_cast<void*>(gnames_ptr));
}

[[nodiscard]] std::variant<const std::string_view, const std::wstring_view> BL2Hook::fname_get_str(
    const unreal::FName& name) const {
    auto entry = gnames_ptr->at(name.index);
    if ((entry->Index & FNameEntry::NAME_WIDE_MASK) != 0) {
        return std::wstring_view{&entry->Name.Wide[0]};
    }
    return std::string_view{&entry->Name.Ansi[0]};
}

}  // namespace unrealsdk::game

#endif
