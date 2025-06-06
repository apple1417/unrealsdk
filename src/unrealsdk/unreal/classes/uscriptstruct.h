#ifndef UNREALSDK_UNREAL_CLASSES_USCRIPTSTRUCT_H
#define UNREALSDK_UNREAL_CLASSES_USCRIPTSTRUCT_H

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/ustruct.h"

namespace unrealsdk::unreal {

#if defined(_MSC_VER) && UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW
#pragma pack(push, 0x4)
#endif
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#endif

namespace offsets::generic {

template <typename T>
class UScriptStruct : public T {
   public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    uint32_t StructFlags;
};

}  // namespace offsets::generic

class UScriptStruct : public UStruct {
   public:
    UScriptStruct() = delete;
    UScriptStruct(const UScriptStruct&) = delete;
    UScriptStruct(UScriptStruct&&) = delete;
    UScriptStruct& operator=(const UScriptStruct&) = delete;
    UScriptStruct& operator=(UScriptStruct&&) = delete;
    ~UScriptStruct() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_USCRIPTSTRUCT_FIELDS(X) X(uint32_t, StructFlags)

    UNREALSDK_DEFINE_FIELDS_HEADER(UScriptStruct, UNREALSDK_USCRIPTSTRUCT_FIELDS);
};

template <>
struct ClassTraits<UScriptStruct> {
    static inline const wchar_t* const NAME = L"ScriptStruct";
};

#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#if defined(_MSC_VER) && UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW
#pragma pack(pop)
#endif

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_USCRIPTSTRUCT_H */
