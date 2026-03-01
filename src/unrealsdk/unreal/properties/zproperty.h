#ifndef UNREALSDK_UNREAL_PROPERTIES_ZPROPERTY_H
#define UNREALSDK_UNREAL_PROPERTIES_ZPROPERTY_H

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
class ZProperty : public FField {
#else
class ZProperty : public UField {
#endif

   public:
#if UNREALSDK_HAS_OPTIONAL_FUNC_PARAMS
    static constexpr auto PROP_FLAG_OPTIONAL = 0x10;
#endif
    static constexpr auto PROP_FLAG_PARAM = 0x80;
    static constexpr auto PROP_FLAG_OUT = 0x100;
    static constexpr auto PROP_FLAG_RETURN = 0x400;

    ZProperty() = delete;
    ZProperty(const ZProperty&) = delete;
    ZProperty(ZProperty&&) = delete;
    ZProperty& operator=(const ZProperty&) = delete;
    ZProperty& operator=(ZProperty&&) = delete;
    ~ZProperty() = delete;

    using property_flags_type = UNREALSDK_ZPROPERTY_FLAGS_TYPE;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_ZPROPERTY_FIELDS(X)     \
    X(int32_t, ArrayDim)                  \
    X(int32_t, ElementSize)               \
    X(property_flags_type, PropertyFlags) \
    X(int32_t, Offset_Internal)           \
    X(ZProperty*, PropertyLinkNext)

    UNREALSDK_DEFINE_FIELDS_HEADER(ZProperty, UNREALSDK_ZPROPERTY_FIELDS);
};

template <>
struct ClassTraits<ZProperty> {
    static inline const wchar_t* const NAME = L"Property";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_PROPERTIES_ZPROPERTY_H */
