#include "unrealsdk/pch.h"
#include "unrealsdk/game/bl4/bl4.h"
#include "unrealsdk/game/bl4/offsets.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/structs/gnames.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2 && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {

namespace {

const constexpr Pattern<32> GNAMES_SIG{
    "48 8D 05 ????????"   // lea rax, [1512A0C80]           <---
    "48 89 CE"            // mov rsi, rcx
    "48 89 C1"            // mov rcx, rax
    "89 D7"               // mov edi, edx
    "E8 ????????"         // call 14114BE40
    "89 FA"               // mov edx, edi
    "48 89 F1"            // mov rcx, rsi
    "C6 05 ???????? 01",  // mov byte ptr [1512A0C40], 01
    3};
FNamePool* name_pool_ptr;

// TOOD: not sure this is the right func, crashes
const constexpr Pattern<34> FNAME_INIT_SIG{
    "41 57 41 56 56 57 53 48 83 EC 40 48 89 CE 48 8B 05 ?? ?? ?? ?? 48 31 E0 48 89 44 24 ?? 48 85 "
    "D2 0F 84"};

using fname_init_func = void (*)(FName* self, const wchar_t* str, int32_t number);
fname_init_func fname_init_ptr;

}  // namespace

void BL4Hook::find_fname_funcs(void) {
    name_pool_ptr = read_offset<decltype(name_pool_ptr)>(GNAMES_SIG.sigscan_nullable());
    LOG(MISC, "FNamePool: {:p}", reinterpret_cast<void*>(name_pool_ptr));

    fname_init_ptr = FNAME_INIT_SIG.sigscan_nullable<fname_init_func>();
    LOG(MISC, "FName::Init: {:p}", reinterpret_cast<void*>(fname_init_ptr));
}

std::variant<const std::string_view, const std::wstring_view> BL4Hook::fname_get_str(
    const unreal::FName& name) const {
    auto entry = reinterpret_cast<bl4::FNameEntry*>(name_pool_ptr->at(name.index));
    size_t size = entry->Metadata >> bl4::FNameEntry::META_SIZE_BIT_OFFSET;

    if ((entry->Metadata & FNameEntry::NAME_WIDE_MASK) != 0) {
        return std::wstring_view{&entry->Name.Wide[0], size};
    }
    return std::string_view{&entry->Name.Ansi[0], size};
}

// void BL4Hook::fname_init(unreal::FName* name, const wchar_t* str, int32_t number) const {
//     fname_init_ptr(name, str, number);
// }

}  // namespace unrealsdk::game

#endif
