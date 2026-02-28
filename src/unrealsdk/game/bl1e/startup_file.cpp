#include "unrealsdk/pch.h"

#include "unrealsdk/config.h"
#include "unrealsdk/game/bl1e/bl1e.h"
#include "unrealsdk/game/bl1e/offsets.h"
#include "unrealsdk/logging.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/classes/uobject.h"
#include "unrealsdk/unreal/find_class.h"
#include "unrealsdk/unreal/structs/fstring.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::unreal;
using namespace unrealsdk::memory;

namespace unrealsdk::game {

////////////////////////////////////////////////////////////////////////////////
// | LOAD PACKAGE |
////////////////////////////////////////////////////////////////////////////////

namespace {

bool should_set_startup_linker{false};
bool startup_file_loaded{false};

void hook_for_startup_bypass(void);

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

UObject* load_package_hook(const UObject* outer, const wchar_t* name, uint32_t flags) {
    static auto bypass_startup_file =
        config::get_bool("unrealsdk.bypass_startup_file").value_or(false);

    if (!bypass_startup_file) {
        return load_package_ptr(outer, name, flags);
    }

    constexpr std::wstring_view startup_name{L"Startup"};
    const bool is_startup = !startup_file_loaded && startup_name == name;
    should_set_startup_linker = is_startup;
    // NOLINTNEXTLINE(misc-const-correctness) // clang-tidy 21 false positive
    UObject* ptr = load_package_ptr(outer, name, flags);

    if (is_startup) {
        startup_file_loaded = true;
    }

    should_set_startup_linker = false;
    return ptr;
}

}  // namespace

void BL1EHook::find_load_package(void) {
    hook_for_startup_bypass();
    detour(SIG_LOAD_PACKAGE, load_package_hook, &load_package_ptr, "LoadPackage");
    LOG(MISC, "LoadPackage: {:p}", reinterpret_cast<void*>(load_package_ptr));
}

[[nodiscard]] UObject* BL1EHook::load_package(const std::wstring& name, uint32_t flags) const {
    return load_package_ptr(nullptr, name.data(), flags);
}

namespace {

////////////////////////////////////////////////////////////////////////////////
// | CLASS / STRUCTURES |
////////////////////////////////////////////////////////////////////////////////

UNREALSDK_UNREAL_STRUCT_PADDING_PUSH()
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#endif
// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init,
//             readability-identifier-naming,
//             readability-magic-numbers)

struct FObjectResource {
    FName ObjectName;
    int32_t OuterIndex;
};

struct FObjectExport : FObjectResource {
    int32_t ClassIndex;  // 12b
    uint8_t UnknownData00[0x20];
    UObject* _Object;  // 48b
    uint8_t UnknownData01[0x2C];
};

struct ULinkerLoad;
struct FObjectImport : FObjectResource {
    uint8_t UnknownData00[0x24];
};

struct ExportTable {
    static constexpr int32_t MAX_ITEMS_PER_CHUNK = 655;
    static constexpr int32_t MAX_CHUNKS = 64;  // idk what the actual limit is
    struct Chunk {
        FObjectExport* Data;
        int32_t Count;
    };
    // Not all chunks are allocated Chunk{nullptr, 0} is a common value
    Chunk Chunks[MAX_CHUNKS];

    FObjectExport* at(int32_t flat_index) {
        const int32_t chunk = flat_index / MAX_ITEMS_PER_CHUNK;
        const int32_t index = flat_index % MAX_ITEMS_PER_CHUNK;

        if (chunk >= MAX_CHUNKS) {
            throw std::runtime_error(std::format("invalid chunk {} >= {}", chunk, MAX_CHUNKS));
        }

        if (index >= MAX_ITEMS_PER_CHUNK) {
            throw std::runtime_error(
                std::format("invalid index {} >= {}", index, MAX_ITEMS_PER_CHUNK));
        }

        return &Chunks[chunk].Data[index];
    }
};

struct ULinkerLoad {
    uint8_t UnknownData00[0xF8];
    TArray<FName> NameMap;            // 248
    TArray<FObjectImport> ImportMap;  // 264
    ExportTable ExportTable;          // 280

    ULinkerLoad() = delete;
    ~ULinkerLoad() = delete;
    ULinkerLoad(const ULinkerLoad&) = delete;
    ULinkerLoad(ULinkerLoad&&) = delete;
    ULinkerLoad& operator=(const ULinkerLoad&) = delete;
    ULinkerLoad& operator=(ULinkerLoad&&) = delete;
};

// NOLINTEND(cppcoreguidelines-pro-type-member-init,
//           readability-identifier-naming,
//           readability-magic-numbers)
#ifdef __clang__
#pragma clang diagnostic pop
#endif
UNREALSDK_UNREAL_STRUCT_PADDING_POP()

////////////////////////////////////////////////////////////////////////////////
// | SIGNATURES |
////////////////////////////////////////////////////////////////////////////////

constexpr Pattern<47> SIG_CREATE_EXPORT{
    "89 54 24 10"              // MOV   dword ptr [RSP + local_res10]
    "48 89 4C 24 08"           // MOV   qword ptr [RSP + local_res8]
    "55"                       // PUSH  RBP
    "53"                       // PUSH  RBX
    "56"                       // PUSH  RSI
    "57"                       // PUSH  RDI
    "41 54"                    // PUSH  R12
    "41 55"                    // PUSH  R13
    "41 56"                    // PUSH  R14
    "41 57"                    // PUSH  R15
    "48 8D 6C 24 E1"           // LEA   RBP=>local_5f,[RSP + -0x1f]
    "48 81 EC C8 00 00 00"     // SUB   RSP,0xc8
    "48 C7 45 07 FE FF FF FF"  // MOV   qword ptr [RBP + local_58],-0x2
    "44 8B F2"                 // MOV   R14D,Index
    "4C 8B E9"                 // MOV   R13,this
};

constexpr Pattern<29> SIG_STATIC_LOAD_OBJECT{
    "48 8B C4"              // MOV   RAX,RSP
    "4C 89 48 20"           // MOV   qword ptr [RAX + local_res20]
    "4C 89 40 18"           // MOV   qword ptr [RAX + local_res18]
    "48 89 50 10"           // MOV   qword ptr [RAX + local_res10]
    "48 89 48 08"           // MOV   qword ptr [RAX + local_res8]
    "53"                    // PUSH  RBX
    "56"                    // PUSH  RSI
    "57"                    // PUSH  RDI
    "48 81 EC 10 01 00 00"  // SUB   RSP,0x110
};

constexpr Pattern<46> SIG_GET_EXPORT_PATH_NAME{
    "48 8B C4"                 // MOV   RAX,RSP
    "4C 89 48 20"              // MOV   qword ptr [RAX + local_res20]
    "48 89 50 10"              // MOV   qword ptr [RAX + local_res10]
    "48 89 48 08"              // MOV   qword ptr [RAX + local_res8]
    "55"                       // PUSH  RBP
    "56"                       // PUSH  RSI
    "57"                       // PUSH  RDI
    "41 54"                    // PUSH  R12
    "41 55"                    // PUSH  R13
    "41 56"                    // PUSH  R14
    "41 57"                    // PUSH  R15
    "48 8D 6C 24 B0"           // LEA   RBP=>local_88,[RSP + -0x50]
    "48 81 EC 50 01 00 00"     // SUB   RSP,0x150
    "48 C7 45 30 FE FF FF FF"  // MOV   qword ptr [RBP + local_58],-0x2
};

// NOLINTBEGIN(readability-identifier-naming)
using create_export_func = UObject* (*)(ULinkerLoad * self, int32_t index);
create_export_func create_export_ptr{nullptr};

using get_export_path_name_func = void (*)(ULinkerLoad* self,
                                           ManagedFString* storage,
                                           int32_t Index,
                                           wchar_t* FakeRoot,
                                           bool bResolveForcedExports);
get_export_path_name_func get_export_path_name_ptr{nullptr};

using static_load_object_func = UObject* (*)(UClass * ObjectClass,
                                             UObject* InOuter,
                                             wchar_t* InName,
                                             wchar_t* Filename,
                                             uint32_t LoadFlags,
                                             void* Sandbox,
                                             bool bAllowObjectReconciliation);
static_load_object_func static_load_object_ptr{nullptr};
// NOLINTEND(readability-identifier-naming)

UObject* create_export_hook(ULinkerLoad* self, int32_t index) {
    static uintptr_t startup_linker{0};

    // Some hacks here, we want to intercept all CreateExport calls for the Startup package linker
    // so that we can forward it to load the object from its actual package. But since this function
    // gets called with different linkers ( even when loading the startup file ) we only want to
    // effect the startup linker everything else should fallthrough normally.
    auto self_intptr = reinterpret_cast<uintptr_t>(self);
    if (should_set_startup_linker && startup_linker == 0) {
        startup_linker = self_intptr;
        should_set_startup_linker = false;
    }

    if (startup_file_loaded || self_intptr != startup_linker) {
        return create_export_ptr(self, index);
    }

    // has this object been loaded already?
    // NOTE: We don't set the _Object because that *will* cause a Linker mismatch error. Only time
    //  this is set is by the game itself.
    const FObjectExport* obj = self->ExportTable.at(index);
    if (obj->_Object != nullptr) {
        return create_export_ptr(self, index);
    }

    UClass* export_class{nullptr};

    if (obj->ClassIndex == 0) {
        export_class = find_class(L"Core.Class"_fn);
    } else {
        FName cls{};

        if (obj->ClassIndex < 0) {
            cls = self->ImportMap.at(-obj->ClassIndex - 1).ObjectName;
        } else {
            cls = self->ExportTable.at(obj->ClassIndex - 1)->ObjectName;
        }

        export_class = find_class(cls);
        if (export_class == nullptr) {
            return create_export_ptr(self, index);
        }
    }

    ManagedFString storage{};
    get_export_path_name_ptr(self, &storage, index, nullptr, false);

    // If prefixed with "Startup." remove it
    constexpr std::wstring_view startup_name = L"Startup.";
    const std::wstring_view obj_path{storage.data, storage.data + storage.count};
    size_t offset{0};
    if (obj_path.starts_with(startup_name)) {
        offset = startup_name.size();
    }

    UObject* loaded_obj = static_load_object_ptr(export_class, nullptr, storage.data + offset,
                                                 nullptr, 0, nullptr, false);

    // Not sure if these need to be kept alive or not, think normally the startup objects are never
    // unloaded.
    if (loaded_obj != nullptr) {
        constexpr auto keep_alive_willow = 0x4000;
        loaded_obj->ObjectFlags() |= keep_alive_willow;
        return loaded_obj;
    }

    return create_export_ptr(self, index);
}

void hook_for_startup_bypass(void) {
    static_load_object_ptr =
        SIG_STATIC_LOAD_OBJECT.sigscan<static_load_object_func>("StaticLoadObject");

    // could probably implement this manually
    get_export_path_name_ptr =
        SIG_GET_EXPORT_PATH_NAME.sigscan<get_export_path_name_func>("GetExportPathName");

    // ULinkerLoad::CreateExport
    detour(SIG_CREATE_EXPORT, create_export_hook, &create_export_ptr, "CreateExport");
}

}  // namespace

}  // namespace unrealsdk::game

#endif
