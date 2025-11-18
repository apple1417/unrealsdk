#include "unrealsdk/pch.h"

#include "unrealsdk/game/bl1e/bl1e.h"
#include "unrealsdk/hook_manager.h"
#include "unrealsdk/memory.h"

#include "unrealsdk/unreal/wrappers/gobjects.h"
#include "unrealsdk/unreal/wrappers/gnames.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {

namespace {

////////////////////////////////////////////////////////////////////////////////
// | GLOBALS |
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// | MEMORY SIGNATURES |
////////////////////////////////////////////////////////////////////////////////

// clang-format off

const constinit Pattern<68> SIG_PROCESS_EVENT{
    "4055415641574881EC90000000488D6C242048C74560FEFFFFFF48899D900000004889B5980000004889BDA00000004C89A5A8000000488B05130A3A024833C548894568"
};

const constinit Pattern<64> SIG_CALL_FUNCTION{
    "405553565741544155415641574881ECA8040000488D6C242048C74568FEFFFFFF488B05E8A83A024833C5488985700400004D8BF94C894D60498BF04C894500"
};

const constinit Pattern<39> SIG_GOBJECTS{
    "4C8D0D49BB820141B856020000488D15681E8301488D0DA91E8301E8DCBA0A{004C8B0505534D02}"
};

const constinit Pattern<37> SIG_GNAMES{
    "488B0D69973502488B01448D04DD0000000041B908000000488BD5FF5010488905{9B8F4102}"
};

const constinit Pattern<37> SIG_GMALLOC{
    "488B0D{69973502}488B01448D04DD0000000041B908000000488BD5FF50104889059B8F4102"
};

const constinit Pattern<53> SIG_CONSTRUCT_OBJECT{
    "488BC44C89401857415641574883EC7048C740C8FEFFFFFF4889580848896810488970204D8BF1488BDA488BF1488BBC24C8000000"
};

const constinit Pattern<48> SIG_GET_PATH_NAME{
    "4889742410574883EC20498BF0488BF9483BCA0F84990000004885C90F8490000000488B49404885C97452483BCA744D"
};

const constinit Pattern<54> SIG_STATIC_FIND_OBJECT{
    "4055565741544155415641574883EC7048C7442440FEFFFFFF48899C24B0000000458BE9498BF04C8BFA4C8BE1833D2CD73402007413"
};

const constinit Pattern<43> SIG_LOAD_PACKAGE{
    "488BC44489401848894808535657415641574883EC6048C740A8FEFFFFFF0F2970C8418BD8488BFA488BF1"
};

// clang-format on

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// | HOOKS |
////////////////////////////////////////////////////////////////////////////////

void BL1EHook::hook_antidebug() {}

void BL1EHook::hexedit_set_command() {}

void BL1EHook::hexedit_array_limit() {}

void BL1EHook::hook_process_event() {
    uintptr_t addr = SIG_PROCESS_EVENT.sigscan("ProcessEvent");
    LOG(INFO, "ProcessEvent found at {:p}", reinterpret_cast<void*>(addr));
}

void BL1EHook::hook_call_function() {
    uintptr_t addr = SIG_CALL_FUNCTION.sigscan("CallFunction");
    LOG(INFO, "CallFunction found at {:p}", reinterpret_cast<void*>(addr));
}

void BL1EHook::find_gobjects() {
    uintptr_t addr = SIG_GOBJECTS.sigscan("GObjects");
    LOG(INFO, "GObjects found at {:p}", reinterpret_cast<void*>(addr));
}

void BL1EHook::find_gnames() {
    uintptr_t addr = SIG_GNAMES.sigscan("GNames");
    LOG(INFO, "GNames found at {:p}", reinterpret_cast<void*>(addr));
}

void BL1EHook::find_gmalloc() {
    uintptr_t addr = SIG_GMALLOC.sigscan("GMalloc");
    LOG(INFO, "GMalloc found at {:p}", reinterpret_cast<void*>(addr));
}

void BL1EHook::find_construct_object() {
    uintptr_t addr = SIG_CONSTRUCT_OBJECT.sigscan("ConstructObject");
    LOG(INFO, "ConstructObject found at {:p}", reinterpret_cast<void*>(addr));
}

void BL1EHook::find_get_path_name() {
    uintptr_t addr = SIG_GET_PATH_NAME.sigscan("GetPathName");
    LOG(INFO, "GetPathName found at {:p}", reinterpret_cast<void*>(addr));
}

void BL1EHook::find_static_find_object() {
    uintptr_t addr = SIG_STATIC_FIND_OBJECT.sigscan("StaticFindObject");
    LOG(INFO, "StaticFindObject found at {:p}", reinterpret_cast<void*>(addr));
}

void BL1EHook::find_load_package() {
    uintptr_t addr = SIG_LOAD_PACKAGE.sigscan("LoadPackage");
    LOG(INFO, "LoadPackage found at {:p}", reinterpret_cast<void*>(addr));
}

void BL1EHook::inject_console() {}

////////////////////////////////////////////////////////////////////////////////
// | PUBLIC API |
////////////////////////////////////////////////////////////////////////////////

#pragma warning(push)
#pragma warning(disable : 4100)

bool BL1EHook::is_console_ready() const {
    return false;
}

const GObjects& BL1EHook::gobjects() const {
    throw std::runtime_error{"not implemented"};
}

const GNames& BL1EHook::gnames() const {
    throw std::runtime_error{"not implemented"};
}

void* BL1EHook::u_malloc(size_t len) const {
    throw std::runtime_error{"not implemented"};
}

void* BL1EHook::u_realloc(void* original, size_t len) const {
    throw std::runtime_error{"not implemented"};
}

void BL1EHook::u_free(void* data) const {
    throw std::runtime_error{"not implemented"};
}

UObject* BL1EHook::construct_object(UClass* cls,
                                    UObject* outer,
                                    const FName& name,
                                    uint64_t flags,
                                    UObject* template_obj) const {
    throw std::runtime_error{"not implemented"};
}

UObject* BL1EHook::find_object(UClass* cls, const std::wstring& name) const {
    throw std::runtime_error{"not implemented"};
}

UObject* BL1EHook::load_package(const std::wstring& name, uint32_t flags) const {
    throw std::runtime_error{"not implemented"};
}

void BL1EHook::process_event(UObject* object, UFunction* func, void* params) const {
    throw std::runtime_error{"not implemented"};
}

void BL1EHook::uconsole_output_text(const std::wstring& str) const {
    // todo: implement
}

std::wstring BL1EHook::uobject_path_name(const UObject* obj) const {
    throw std::runtime_error{"not implemented"};
}

void BL1EHook::fsoftobjectptr_assign(FSoftObjectPtr* ptr, const UObject* obj) const {
    throw std::runtime_error{"not implemented"};
}

void BL1EHook::flazyobjectptr_assign(FLazyObjectPtr* ptr, const UObject* obj) const {
    throw std::runtime_error{"not implemented"};
}

#pragma warning(pop)

}  // namespace unrealsdk::game

#endif