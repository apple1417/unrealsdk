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

#pragma region ConstructObject

namespace {

struct __declspec(align(16)) FStaticConstructObjectParameters {
    UClass* Class;
    UObject* Outer;
    FName Name;
    int SetFlags;
    int InternalSetFlags;
    bool bCopyTransientsFromClassDefaults;
    bool bAssumeTemplateIsArchetype;
    UObject* Template;
    void* InstanceGraph;
    void* ExternalPackage;
    void* PropertyInitCallback;
    void* SubobjectOverrides;
};

using construct_obj_func = UObject* (*)(FStaticConstructObjectParameters* params);
construct_obj_func construct_obj_ptr;

const constinit Pattern<41> CONSTRUCT_OBJECT_PATTERN{
    "41 56"                        // push r14
    "56"                           // push rsi
    "57"                           // push rdi
    "55"                           // push rbp
    "53"                           // push rbx
    "48 81 EC ?? ?? ?? ??"         // sub rsp, 280h
    "48 89 CE"                     // mov rsi, rcx
    "48 8B 05 ?? ?? ?? ??"         // mov rax, [rip+...]
    "48 31 E0"                     // xor rax, rsp
    "48 89 84 24 ?? ?? ?? ??"      // mov [rsp+2A8h], rax
    "48 8B 39"                     // mov rdi, [rcx]
    "48 8B 51 08"                  // mov rdx, [rcx+8]
};

}  // namespace

void BL4Hook::find_construct_object(void) {
    construct_obj_ptr = CONSTRUCT_OBJECT_PATTERN.sigscan_nullable<construct_obj_func>();
    LOG(MISC, "StaticConstructObject: {:p}", reinterpret_cast<void*>(construct_obj_ptr));
}

UObject* BL4Hook::construct_object(UClass* cls,
                                   UObject* outer,
                                   const FName& name,
                                   uint64_t flags,
                                   UObject* template_obj) const {
    if (flags > std::numeric_limits<uint32_t>::max()) {
        throw std::out_of_range("construct_object flags out of range, only 32-bits are supported");
    }

    FStaticConstructObjectParameters params;
    params.Class = cls;
    params.Outer = outer;
    params.Name = name;
    params.SetFlags = static_cast<uint32_t>(flags);
    params.InternalSetFlags = 0;
    params.Template = template_obj;
    return construct_obj_ptr(&params);
}

#pragma endregion

#pragma region StaticFindObject

namespace {

using static_find_object_safe_func = UObject* (*)(const UClass* cls,
                                                  intptr_t package,
                                                  const wchar_t* str,
                                                  uint32_t exact_class);
static_find_object_safe_func static_find_object_ptr;


const constinit Pattern<31> STATIC_FIND_OBJECT_PATTERN{
    "41 56"                 // push r14
    "56"                    // push rsi
    "57"                    // push rdi
    "53"                    // push rbx
    "48 83 EC ??"           // sub rsp, 38h
    "48 8B 05 ????????"     // mov rax, cs:security_cookie
    "48 31 E0"              // xor rax, rsp
    "48 89 44 24 ??"        // mov [rsp+58h+var_28], rax
    "F6 05 ??????????"      // test cs:1513B1940, 1
};

const constexpr intptr_t ANY_PACKAGE = -1;

}  // namespace

void BL4Hook::find_static_find_object(void) {
    static_find_object_ptr =
        STATIC_FIND_OBJECT_PATTERN.sigscan_nullable<static_find_object_safe_func>();
    LOG(MISC, "StaticFindObjectSafe: {:p}", reinterpret_cast<void*>(static_find_object_ptr));
}

UObject* BL4Hook::find_object(UClass* cls, const std::wstring& name) const {
    return static_find_object_ptr(cls, ANY_PACKAGE, name.c_str(), 0 /* false */);
}

#pragma endregion

#pragma region LoadPackage

namespace {

using load_package_func = UObject* (*)(const UObject* outer,
                                       const wchar_t* name,
                                       uint32_t flags,
                                       void* instancingContext,
                                       void* reader_override);
load_package_func load_package_ptr;

const constinit Pattern<26> LOAD_PACKAGE_PATTERN{
    "41 57"                 // push r15
    "41 56"                 // push r14
    "41 54"                 // push r12
    "56"                    // push rsi
    "57"                    // push rdi
    "55"                    // push rbp
    "53"                    // push rbx
    "48 81 EC ????????"     // sub rsp, 0B0h
    "4C 89 CB"              // mov rbx, r9
    "44 89 C6"              // mov esi, r8d
    "48 89 CF"              // mov rdi, rcx
};

}  // namespace

void BL4Hook::find_load_package(void) {
    load_package_ptr = LOAD_PACKAGE_PATTERN.sigscan_nullable<load_package_func>();
    LOG(MISC, "LoadPackage: {:p}", reinterpret_cast<void*>(load_package_ptr));
}

[[nodiscard]] UObject* BL4Hook::load_package(const std::wstring& name, uint32_t flags) const {
    return load_package_ptr(nullptr, name.data(), flags, nullptr, nullptr);
}

#pragma endregion

}  // namespace unrealsdk::game

#endif
