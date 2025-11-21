#include "unrealsdk/pch.h"

#include "unrealsdk/game/bl1e/bl1e.h"
#include "unrealsdk/logging.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/classes/uobject.h"
#include "unrealsdk/unreal/structs/fstring.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::unreal;
using namespace unrealsdk::memory;

namespace unrealsdk::game {

#pragma region ConstructObject

namespace {

using construct_obj_func = UObject* (*)(UClass* in_class,
                                        UObject* in_outer,
                                        FName in_name,
                                        UObject::object_flags_type in_flags,
                                        UObject* in_template,
                                        void* error,
                                        void* subobject_root,
                                        void* in_instance_graph);
construct_obj_func construct_obj_ptr;

constexpr Pattern<53> SIG_CONSTRUCT_OBJECT{
    "488BC44C89401857415641574883EC7048C740C8FEFFFFFF4889580848896810488970204D8BF1488BDA488BF1488B"
    "BC24C8000000"};

}  // namespace

void BL1EHook::find_construct_object(void) {
    construct_obj_ptr = SIG_CONSTRUCT_OBJECT.sigscan_nullable<construct_obj_func>();
    LOG(MISC, "StaticConstructObject: {:p}", reinterpret_cast<void*>(construct_obj_ptr));
}

UObject* BL1EHook::construct_object(UClass* cls,
                                    UObject* outer,
                                    const FName& name,
                                    uint64_t flags,
                                    UObject* template_obj) const {
    return construct_obj_ptr(cls, outer, name, flags, template_obj, nullptr, nullptr, nullptr);
}

#pragma endregion

#pragma region PathName

namespace {

#ifdef __MINGW32__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"  // thiscall on non-class
#endif

using get_path_name_func = void (*)(const UObject* self,
                                    const UObject* stop_outer,
                                    ManagedFString* str);

#ifdef __MINGW32__
#pragma GCC diagnostic pop
#endif

get_path_name_func get_path_name_ptr;

constexpr Pattern<48> SIG_GET_PATH_NAME{
    "4889742410574883EC20498BF0488BF9483BCA0F84990000004885C90F8490000000488B49404885C97452483BCA74"
    "4D"};

}  // namespace

void BL1EHook::find_get_path_name(void) {
    get_path_name_ptr = SIG_GET_PATH_NAME.sigscan_nullable<get_path_name_func>();
    LOG(MISC, "GetPathName: {:p}", reinterpret_cast<void*>(get_path_name_ptr));
}

std::wstring BL1EHook::uobject_path_name(const UObject* obj) const {
    ManagedFString str{};
    get_path_name_ptr(obj, nullptr, &str);
    return str;
}

#pragma endregion

#pragma region FindObject

namespace {

using static_find_object_func = UObject* (*)(const UClass* cls,
                                             const UObject* package,
                                             const wchar_t* str,
                                             bool exact_class);

static_find_object_func static_find_object_ptr;

constexpr Pattern<54> SIG_STATIC_FIND_OBJECT{
    "4055565741544155415641574883EC7048C7442440FEFFFFFF48899C24B0000000458BE9498BF04C8BFA4C8BE1833D"
    "2CD73402007413"};

}  // namespace

void BL1EHook::find_static_find_object(void) {
    static_find_object_ptr = SIG_STATIC_FIND_OBJECT.sigscan_nullable<static_find_object_func>();
    LOG(MISC, "StaticFindObject: {:p}", reinterpret_cast<void*>(static_find_object_ptr));
}

UObject* BL1EHook::find_object(UClass* cls, const std::wstring& name) const {
    return static_find_object_ptr(cls, nullptr, name.c_str(), false);
}

#pragma endregion

#pragma region LoadPackage

namespace {

using load_package_func = UObject* (*)(const UObject* outer, const wchar_t* name, uint32_t flags);
load_package_func load_package_ptr;

constexpr Pattern<43> SIG_LOAD_PACKAGE{
    "488BC44489401848894808535657415641574883EC6048C740A8FEFFFFFF0F2970C8418BD8488BFA488BF1"};

}  // namespace

void BL1EHook::find_load_package(void) {
    load_package_ptr = SIG_LOAD_PACKAGE.sigscan_nullable<load_package_func>();
    LOG(MISC, "LoadPackage: {:p}", reinterpret_cast<void*>(load_package_ptr));
}

[[nodiscard]] UObject* BL1EHook::load_package(const std::wstring& name, uint32_t flags) const {
    return load_package_ptr(nullptr, name.data(), flags);
}

#pragma endregion

}  // namespace unrealsdk::game

#endif
