#include "unrealsdk/pch.h"

#include "unrealsdk/game/bl1e/bl1e.h"
#include "unrealsdk/logging.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/classes/uobject.h"
#include "unrealsdk/unreal/structs/fstring.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW64 && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::unreal;
using namespace unrealsdk::memory;

namespace unrealsdk::game {

#pragma region ConstructObject

namespace {

using construct_obj_func = UObject* (*)(UClass * in_class,
                                        UObject* in_outer,
                                        FName in_name,
                                        UObject::object_flags_type in_flags,
                                        UObject* in_template,
                                        void* error,
                                        void* subobject_root,
                                        void* in_instance_graph);
construct_obj_func construct_obj_ptr;

constexpr Pattern<16> SIG_CONSTRUCT_OBJECT{
    "48 8B C4"     // MOV   RAX,RSP
    "4C 89 40 18"  // MOV   qword ptr [RAX + local_res18],param_3
    "57"           // PUSH  RDI
    "41 56"        // PUSH  R14
    "41 57"        // PUSH  R15
    "48 83 EC 70"  // SUB   RSP,0x70
};

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

using get_path_name_func = void (*)(const UObject* self,
                                    const UObject* stop_outer,
                                    ManagedFString* str);

get_path_name_func get_path_name_ptr;

constexpr Pattern<25> SIG_GET_PATH_NAME{
    "48 89 74 24 10"  // MOV   qword ptr [RSP + local_res10],RSI
    "57"              // PUSH  RDI
    "48 83 EC 20"     // SUB   RSP,0x20
    "49 8B F0"        // MOV   RSI,ResultString
    "48 8B F9"        // MOV   RDI,this
    "48 3B CA"        // CMP   this,StopOuter
    "0F84 ????????"   // JZ    LAB_1401c02e2
};

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

constexpr Pattern<36> SIG_STATIC_FIND_OBJECT{
    "40 55"                       // PUSH  RBP
    "56"                          // PUSH  RSI
    "57"                          // PUSH  RDI
    "41 54"                       // PUSH  R12
    "41 55"                       // PUSH  R13
    "41 56"                       // PUSH  R14
    "41 57"                       // PUSH  R15
    "48 83 EC 70"                 // SUB   RSP,0x70
    "48 C7 44 24 40 FE FF FF FF"  // MOV   qword ptr [RSP + local_68],-0x2
    "48 89 9C 24 B0 00 00 00"     // MOV   qword ptr [RSP + local_res8],RBX
    "45 8B E9"                    // MOV   R13D,ExactClass
};

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

constexpr Pattern<22> SIG_LOAD_PACKAGE{
    "48 8B C4"     // MOV   RAX,RSP
    "44 89 40 18"  // MOV   dword ptr [RAX + local_res18],LoadFlags
    "48 89 48 08"  // MOV   qword ptr [RAX + local_res8],InOuter
    "53"           // PUSH  RBX
    "56"           // PUSH  RSI
    "57"           // PUSH  RDI
    "41 56"        // PUSH  R14
    "41 57"        // PUSH  R15
    "48 83 EC 60"  // SUB   RSP,0x60
};

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
