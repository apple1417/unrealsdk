/*
====================================================================================================

This file contains forward declarations of all the C api functions used for the main `unrealsdk`
namespace. It is included directly in the other two files.

====================================================================================================
*/

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/offset_list.h"
#include "unrealsdk/unreal/structs/tarray.h"

namespace unrealsdk::unreal {

class GNames;
class GObjects;
class UClass;
class UEnum;
class UFunction;
struct FFrame;
struct FLazyObjectPtr;
struct FName;
struct FSoftObjectPtr;
struct FText;
struct TemporaryFString;

template <typename KeyType, typename ValueType>
struct TPair;

}  // namespace unrealsdk::unreal

using namespace unrealsdk::unreal;

namespace unrealsdk {

UNREALSDK_CAPI([[nodiscard]] bool, is_initialized);
UNREALSDK_CAPI([[nodiscard]] bool, is_console_ready);
UNREALSDK_CAPI([[nodiscard]] const GObjects*, gobjects);
UNREALSDK_CAPI([[nodiscard]] const GNames*, gnames);
UNREALSDK_CAPI([[nodiscard]] void*, u_malloc, size_t len);
UNREALSDK_CAPI([[nodiscard]] void*, u_realloc, void* original, size_t len);
UNREALSDK_CAPI(void, u_free, void* data);
UNREALSDK_CAPI([[nodiscard]] UObject*,
               construct_object,
               UClass* cls,
               UObject* outer,
               const FName* name = nullptr,
               uint64_t flags = 0,
               UObject* template_obj = nullptr);
UNREALSDK_CAPI([[nodiscard]] UObject*,
               find_object,
               UClass* cls,
               const wchar_t* name,
               size_t name_size);
UNREALSDK_CAPI([[nodiscard]] UObject*,
               load_package,
               const wchar_t* name,
               size_t size,
               uint32_t flags);

namespace internal {

UNREALSDK_CAPI(void, fname_init, FName* name, const wchar_t* str, int32_t number);
UNREALSDK_CAPI(void, fframe_step, FFrame* frame, UObject* obj, void* param);
UNREALSDK_CAPI(void, process_event, UObject* object, UFunction* function, void* params);
UNREALSDK_CAPI(void, uconsole_output_text, const wchar_t* str, size_t size);
UNREALSDK_CAPI([[nodiscard]] wchar_t*, uobject_path_name, const UObject* obj, size_t& size);
UNREALSDK_CAPI(void, ftext_as_culture_invariant, FText* text, TemporaryFString&& str);
UNREALSDK_CAPI(void, fsoftobjectptr_assign, FSoftObjectPtr* ptr, const unreal::UObject* obj);
UNREALSDK_CAPI(void, flazyobjectptr_assign, FLazyObjectPtr* ptr, const unreal::UObject* obj);
UNREALSDK_CAPI([[nodiscard]] const offsets::OffsetList*, get_offsets);

// Since we're not allowed to return a C++ type in an extern C, create a dummy C type to transfer
// all our values into
struct UEnumGetNamesCRet {
    void* data;
    int32_t count;
    int32_t max;
};
static_assert(sizeof(UEnumGetNamesCRet)
              == sizeof(unreal::TArray<unreal::TPair<unreal::FName, uint64_t>>));
static_assert(alignof(UEnumGetNamesCRet)
              == alignof(unreal::TArray<unreal::TPair<unreal::FName, uint64_t>>));
UNREALSDK_CAPI([[nodiscard]] UEnumGetNamesCRet, uenum_get_names, const unreal::UEnum* uenum);

}  // namespace internal

}  // namespace unrealsdk
