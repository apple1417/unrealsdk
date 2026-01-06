#ifndef UNREALSDK_UNREAL_CLASSES_UPROPERTY_H
#define UNREALSDK_UNREAL_CLASSES_UPROPERTY_H

#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/structs/fname.h"

#if UNREALSDK_PROPERTIES_ARE_FFIELD
#include "unrealsdk/unreal/structs/ffield.h"
#else
#include "unrealsdk/unreal/classes/ufield.h"
#endif

namespace unrealsdk::unreal {

#if UNREALSDK_PROPERTIES_ARE_FFIELD
class UProperty : public FField {
#else
class UProperty : public UField {
#endif

   public:
#if UNREALSDK_HAS_OPTIONAL_FUNC_PARAMS
    static constexpr auto PROP_FLAG_OPTIONAL = 0x10;
#endif
    static constexpr auto PROP_FLAG_PARAM = 0x80;
    static constexpr auto PROP_FLAG_OUT = 0x100;
    static constexpr auto PROP_FLAG_RETURN = 0x400;

    UProperty() = delete;
    UProperty(const UProperty&) = delete;
    UProperty(UProperty&&) = delete;
    UProperty& operator=(const UProperty&) = delete;
    UProperty& operator=(UProperty&&) = delete;
    ~UProperty() = delete;

    using property_flags_type = UNREALSDK_UPROPERTY_FLAGS_TYPE;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_UPROPERTY_FIELDS(X)     \
    X(int32_t, ArrayDim)                  \
    X(int32_t, ElementSize)               \
    X(property_flags_type, PropertyFlags) \
    X(int32_t, Offset_Internal)           \
    X(UProperty*, PropertyLinkNext)

    UNREALSDK_DEFINE_FIELDS_HEADER(UProperty, UNREALSDK_UPROPERTY_FIELDS);
};

template <>
struct ClassTraits<UProperty> {
    static inline const wchar_t* const NAME = L"Property";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_UPROPERTY_H */
