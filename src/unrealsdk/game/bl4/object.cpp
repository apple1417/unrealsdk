#include "unrealsdk/pch.h"
#include "unrealsdk/game/bl4/bl4.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unrealsdk.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2 && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {

#pragma region PathName

namespace {

// TODO: May want to move this type into a generic header at some point
// NOLINTNEXTLINE(readability-identifier-naming)
struct TStringBuilderBase_wchar_t {
    wchar_t* base;
    wchar_t* current;
    wchar_t* end;
    uint32_t is_dynamic;
};

using get_path_name_func = void (*)(const UObject* self,
                                    const UObject* stop_outer,
                                    TStringBuilderBase_wchar_t* str);
get_path_name_func get_path_name_ptr;

const constinit Pattern<39> GET_PATH_NAME_PATTERN{
    "41 56"                 // push r14
    "56"                    // push rsi
    "57"                    // push rdi
    "53"                    // push rbx
    "48 81 EC ????????"     // sub rsp, 00000088
    "4C 89 C6"              // mov rsi, r8
    "48 89 CF"              // mov rdi, rcx
    "48 8B 05 ????????"     // mov rax, [15112D940]
    "48 31 E0"              // xor rax, rsp
    "48 89 84 24 ????????"  // mov [rsp+00000080], rax
    "48 39 D1"              // cmp rcx, rdx
};

}  // namespace

void BL4Hook::find_get_path_name(void) {
    get_path_name_ptr = GET_PATH_NAME_PATTERN.sigscan_nullable<get_path_name_func>();
    LOG(MISC, "GetPathName: {:p}", reinterpret_cast<void*>(get_path_name_ptr));
}

std::wstring BL4Hook::uobject_path_name(const UObject* obj) const {
    const constexpr auto default_buf_size = 256;

    wchar_t buf[default_buf_size];
    TStringBuilderBase_wchar_t str{
        .base = &buf[0],
        .current = &buf[0],
        // Assuming this is an exclusive end, there's code like `if (end <= current + 4)`
        .end = &buf[default_buf_size],
        .is_dynamic = 0,
    };

    get_path_name_ptr(obj, nullptr, &str);

    std::wstring output{str.base, (size_t)(str.current - str.base)};
    if (str.is_dynamic != 0) {
        unrealsdk::u_free(str.base);
    }

    return output;
}

#pragma endregion

}  // namespace unrealsdk::game

#endif
