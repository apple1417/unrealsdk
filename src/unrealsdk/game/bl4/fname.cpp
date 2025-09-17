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

const constexpr Pattern<29> FNAMEPOOL_SIG{
    "80 3D ???????? 00"  // cmp byte ptr [1512A0C40], 00    <--- Initialized flag
    "75 ??"              // jne 1411575F3
    "48 8D 05 ????????"  // lea rax, [1512A0C80]            <--- FNamePool
    "48 89 CE"           // mov rsi, rcx
    "48 89 C1"           // mov rcx, rax
    "89 D7"              // mov edi, edx
    "E8 ????????"        // call 14114BE40                  <--- Init func
};
const constexpr auto FNAMEPOOL_INITIALIZED_OFFSET = 2;
const constexpr auto FNAMEPOOL_PTR_OFFSET = 12;

FNamePool* name_pool_ptr;

const constexpr Pattern<39> FNAME_FIND_OR_STORE_WSTRING{
    "41 57"                 // push r15
    "41 56"                 // push r14
    "56"                    // push rsi
    "57"                    // push rdi
    "53"                    // push rbx
    "48 81 EC ????0000"     // sub rsp, 00000440
    "48 89 CE"              // mov rsi, rcx
    "48 8B 05 ????????"     // mov rax, [15112D940]
    "48 31 E0"              // xor rax, rsp
    "48 89 84 24 ????????"  // mov [rsp+00000438], rax
    "48 63 42 ??"           // movsxd  rax, dword ptr [rdx+08]
};

struct FNameStringView {
    const wchar_t* str;
    uint32_t len;
    uint32_t non_ascii;
};

using fname_find_or_store_wstring_func = void (*)(FName* self,
                                                  const FNameStringView* str,
                                                  uint32_t find_name);
fname_find_or_store_wstring_func fname_find_or_store_wstring_ptr;

}  // namespace

void BL4Hook::find_fname_funcs(void) {
    auto name_pool_base = FNAMEPOOL_SIG.sigscan("FNamePool");
    name_pool_ptr = read_offset<decltype(name_pool_ptr)>(name_pool_base + FNAMEPOOL_PTR_OFFSET);
    LOG(MISC, "FNamePool: {:p}", reinterpret_cast<void*>(name_pool_ptr));

    // I tried manually initaliaing the pool, but it didn't seem to like it
    // Just wait for it to happen on the main thread instead
    auto name_pool_initialized =
        read_offset<volatile uint8_t*>(name_pool_base + FNAMEPOOL_INITIALIZED_OFFSET);
    while (*name_pool_initialized != 0) {
        const constexpr auto sleep_time = std::chrono::milliseconds{50};
        std::this_thread::sleep_for(sleep_time);
    }

    fname_find_or_store_wstring_ptr =
        FNAME_FIND_OR_STORE_WSTRING.sigscan_nullable<fname_find_or_store_wstring_func>();
    LOG(MISC, "FNameHelper::FindOrStoreWString: {:p}",
        reinterpret_cast<void*>(fname_find_or_store_wstring_ptr));
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

void BL4Hook::fname_init(FName* name, const wchar_t* str, int32_t number) const {
    const constexpr auto max_ascii_char = L'\x7F';
    const constexpr auto fname_add = 1;

    std::wstring_view wstr_view{str};
    FNameStringView fname_view{
        .str = str,
        .len = (uint32_t)wstr_view.size(),
        .non_ascii =
            std::ranges::any_of(wstr_view, [](wchar_t chr) { return chr > max_ascii_char; }) ? 1
                                                                                             : 0U,
    };
    fname_find_or_store_wstring_ptr(name, &fname_view, fname_add);

    if (number != 0) {
        name->number = number;
    }
}

}  // namespace unrealsdk::game

#endif
