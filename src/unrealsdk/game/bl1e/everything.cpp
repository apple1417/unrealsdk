#include "unrealsdk/pch.h"
#include "unrealsdk/game/bl1e/bl1e.h"

#include "unrealsdk/hook_manager.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/alignment.h"
#include "unrealsdk/unreal/structs/fstring.h"
#include "unrealsdk/unreal/wrappers/gnames.h"
#include "unrealsdk/unreal/wrappers/gobjects.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {

namespace {

////////////////////////////////////////////////////////////////////////////////
// | GLOBALS |
////////////////////////////////////////////////////////////////////////////////

std::recursive_mutex mutex{};

GObjects gobjects_wrapper{};
GNames gnames_wrapper{};

struct FMalloc;
struct FMallocVFtable {
    // TODO: Made the assumption these are using size_t not uint32_t but didn't check it
    void* exec;
    void* (*u_malloc)(FMalloc* self, size_t len, size_t align);
    void* (*u_realloc)(FMalloc* self, void* original, size_t len, size_t align);
    void* (*u_free)(FMalloc* self, void* data);
};
struct FMalloc {
    FMallocVFtable* vftable;
};

FMalloc** gmalloc_ptr{nullptr};

// NOLINTNEXTLINE(modernize-use-using)
typedef void (*get_path_name_func)(const UObject* self,
                                   const UObject* stop_outer,
                                   ManagedFString* str);

get_path_name_func get_path_name_ptr{nullptr};

// NOLINTNEXTLINE(modernize-use-using)
typedef UObject* (*static_find_object_func)(const UClass* cls,
                                            const UObject* package,
                                            const wchar_t* str,
                                            uint32_t exact_class);

static_find_object_func static_find_object_ptr{nullptr};

// NOLINTNEXTLINE(modernize-use-using)
typedef UObject* (*construct_obj_func)(UClass* cls,
                                       UObject* outer,
                                       FName name,
                                       uint64_t flags,
                                       UObject* template_obj,
                                       void* error_output_device,
                                       void* instance_graph,
                                       uint32_t assume_template_is_archetype);
construct_obj_func construct_obj_ptr;

using load_package_func = UObject* (*)(const UObject* outer, const wchar_t* name, uint32_t flags);
load_package_func load_package_ptr;

// NOLINTNEXTLINE(modernize-use-using)
typedef void (*process_event_func)(UObject* obj, UFunction* func, void* params, void*);

process_event_func process_event_func_ptr{nullptr};

void _hook_process_event(UObject* obj, UFunction* func, void* params, void* null) {
    std::lock_guard guard{mutex};
    process_event_func_ptr(obj, func, params, null);

    static int count{0};
    if (count < 5000) {
        LOG(INFO, L"{} - {}", obj->get_path_name(), func->get_path_name());
        ++count;
    }
}

// NOLINTNEXTLINE(modernize-use-using)
typedef void (*call_function_func)(UObject* obj, FFrame* stack, void* params, UFunction* func);

call_function_func call_function_func_ptr{nullptr};

void _hook_call_function(UObject* obj, FFrame* stack, void* params, UFunction* func) {
    std::lock_guard guard{mutex};
    call_function_func_ptr(obj, stack, params, func);

    static int count{0};
    if (count < 5000) {
        LOG(INFO, L"{} - {}", obj->get_path_name(), func->get_path_name());
        ++count;
    }

    if (count == 5000) {
        ++count;
        for (size_t i = 0; i < gnames().size(); ++i) {
            FNameEntry* entry = gnames().at(i);

            if (entry == nullptr) {
                LOG(INFO, "{:>4} ~ {}", i, "NULL");
            } else if (entry->is_wide()) {
                LOG(INFO, L"{:>4} ~ {}", i, entry->WideName);
            } else {
                LOG(INFO, "{:>4} ~ {}", i, entry->AnsiName);
            }
        }
    }

    // if (count < 25000) {
    //     LOG(INFO, L"{}, {}", obj->get_path_name(), func->get_path_name());
    //     ++count;
    // }

    // if (count == 100) {
    //     for (size_t i = 0; i < gobjects().size(); ++i) {
    //         const UObject* const gobj = gobjects().obj_at(i);
    //         if (gobj == nullptr) {
    //             LOG(INFO, L"{:>4} - NULL", i);
    //         } else {
    //             LOG(INFO, L"{:>4} - {}", i, gobj->get_path_name());
    //         }
    //     }
    // }
}

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

const constinit Pattern<25> SIG_GOBJECTS{
    "E8 ?? ?? ?? ?? 4C 8B 05 {????????} 49 8B 0C D8 48 8B 57 0C 48 85 51 10 0F"
};

const constinit Pattern<25> SIG_GNAMES{
    // E8 ?? ?? ?? ?? 48 8B 05 {?? ?? ?? ??} 48 89 3C D8 49 8B CC FF ?? ?? ?? 75 ??
    "E8 ?? ?? ?? ?? 48 8B 05 {?? ?? ?? ??} 48 89 3C D8 49 8B CC FF ?? ?? ?? 75 ??"
};

const constinit Pattern<37> SIG_GMALLOC{
    // "48 8B 0D 69 97 35 02 48 8B 01 44 8D 04 DD 00 00 00 00 41 B9 08 00 00 00 48 8B D5 FF 50 10 48 89 05 9B 8F 41 02"
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
    detour(SIG_PROCESS_EVENT, _hook_process_event, &process_event_func_ptr, "ProcessEvent");
    LOG(INFO, "ProcessEvent found at {:p}", reinterpret_cast<void*>(process_event_func_ptr));
}

void BL1EHook::hook_call_function() {
    detour(SIG_CALL_FUNCTION, _hook_call_function, &call_function_func_ptr, "CallFunction");
    LOG(INFO, "CallFunction found at {:p}", reinterpret_cast<void*>(call_function_func_ptr));
}

void BL1EHook::find_gobjects() {
    auto gobjects_ptr = read_offset<GObjects::internal_type>(SIG_GOBJECTS.sigscan_nullable());
    LOG(INFO, "GGObjects found at {:p}", reinterpret_cast<void*>(gobjects_ptr));
    gobjects_wrapper = GObjects{gobjects_ptr};
}

void BL1EHook::find_gnames() {
    auto* gnames_ptr = read_offset<GNames::internal_type>(SIG_GNAMES.sigscan_nullable());
    LOG(INFO, "GNames found at {:p}", reinterpret_cast<void*>(gnames_ptr));
    gnames_wrapper = GNames{gnames_ptr};
}

void BL1EHook::find_gmalloc() {
    gmalloc_ptr = read_offset<FMalloc**>(SIG_GMALLOC.sigscan_nullable());
    LOG(MISC, "GMalloc*: {:p}", reinterpret_cast<void*>(gmalloc_ptr));

    if (gmalloc_ptr != nullptr) {
        LOG(MISC, "GMalloc: {:p}", reinterpret_cast<void*>(*gmalloc_ptr));
    }
}

void BL1EHook::find_construct_object() {
    construct_obj_ptr = SIG_CONSTRUCT_OBJECT.sigscan<construct_obj_func>("ConstructObject");
    LOG(INFO, "ConstructObject found at {:p}", reinterpret_cast<void*>(construct_obj_ptr));
}

void BL1EHook::find_get_path_name() {
    get_path_name_ptr = SIG_GET_PATH_NAME.sigscan<get_path_name_func>("GetPathName");
    LOG(INFO, "GetPathName found at {:p}", reinterpret_cast<void*>(get_path_name_ptr));
}

void BL1EHook::find_static_find_object() {
    static_find_object_ptr = SIG_STATIC_FIND_OBJECT.sigscan_nullable<static_find_object_func>();
    LOG(MISC, "StaticFindObject: {:p}", reinterpret_cast<void*>(static_find_object_ptr));
}

void BL1EHook::find_load_package() {
    load_package_ptr = SIG_LOAD_PACKAGE.sigscan<load_package_func>("LoadPackage");
    LOG(INFO, "LoadPackage found at {:p}", reinterpret_cast<void*>(load_package_ptr));
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
    return gobjects_wrapper;
}

const GNames& BL1EHook::gnames() const {
    return gnames_wrapper;
}

void* BL1EHook::u_malloc(size_t len) const {
    if (gmalloc_ptr == nullptr) {
        throw std::runtime_error("tried allocating memory while gmalloc was still null!");
    }
    auto gmalloc = *gmalloc_ptr;
    auto ret = gmalloc->vftable->u_malloc(gmalloc, len, get_malloc_alignment(len));
    memset(ret, 0, len);
    return ret;
}

void* BL1EHook::u_realloc(void* original, size_t len) const {
    if (gmalloc_ptr == nullptr) {
        throw std::runtime_error("tried allocating memory while gmalloc was still null!");
    }
    auto gmalloc = *gmalloc_ptr;
    return gmalloc->vftable->u_realloc(gmalloc, original, len, get_malloc_alignment(len));
}

void BL1EHook::u_free(void* data) const {
    if (gmalloc_ptr == nullptr) {
        throw std::runtime_error("tried allocating memory while gmalloc was still null!");
    }
    auto gmalloc = *gmalloc_ptr;
    gmalloc->vftable->u_free(gmalloc, data);
}

UObject* BL1EHook::construct_object(UClass* cls,
                                    UObject* outer,
                                    const FName& name,
                                    uint64_t flags,
                                    UObject* template_obj) const {
    return construct_obj_ptr(cls, outer, name, flags, template_obj, nullptr, nullptr, 0);
}

UObject* BL1EHook::find_object(UClass* cls, const std::wstring& name) const {
    return static_find_object_ptr(cls, nullptr, name.c_str(), 0);
}

UObject* BL1EHook::load_package(const std::wstring& name, uint32_t flags) const {
    return load_package_ptr(nullptr, name.data(), flags);
}

void BL1EHook::process_event(UObject* object, UFunction* func, void* params) const {
    process_event_func_ptr(object, func, params, nullptr);
}

void BL1EHook::uconsole_output_text(const std::wstring& str) const {
    // placeholder because clangformat is assssssssssssssssss
}

std::wstring BL1EHook::uobject_path_name(const UObject* obj) const {
    ManagedFString str{};
    get_path_name_ptr(obj, nullptr, &str);
    return str;
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