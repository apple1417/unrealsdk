#ifndef UNREALSDK_UNREAL_PROPERTIES_ZBYTEPROPERTY_H
#define UNREALSDK_UNREAL_PROPERTIES_ZBYTEPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/uenum.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/properties/copyable_property.h"

namespace unrealsdk::unreal {

class UEnum;

class ZByteProperty : public CopyableProperty<uint8_t> {
   public:
    ZByteProperty() = delete;
    ZByteProperty(const ZByteProperty&) = delete;
    ZByteProperty(ZByteProperty&&) = delete;
    ZByteProperty& operator=(const ZByteProperty&) = delete;
    ZByteProperty& operator=(ZByteProperty&&) = delete;
    ~ZByteProperty() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_ZBYTEPROPERTY_FIELDS(X) X(UEnum*, Enum)

    UNREALSDK_DEFINE_FIELDS_HEADER(ZByteProperty, UNREALSDK_ZBYTEPROPERTY_FIELDS);
};

template <>
struct PropTraits<ZByteProperty> : public PropTraits<CopyableProperty<uint8_t>> {};

template <>
struct ClassTraits<ZByteProperty> {
    static inline const wchar_t* const NAME = L"ByteProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_PROPERTIES_ZBYTEPROPERTY_H */
