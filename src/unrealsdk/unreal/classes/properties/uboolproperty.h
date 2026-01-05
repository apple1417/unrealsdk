#ifndef UNREALSDK_UNREAL_CLASSES_PROPERTIES_UBOOLPROPERTY_H
#define UNREALSDK_UNREAL_CLASSES_PROPERTIES_UBOOLPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/uproperty.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"

namespace unrealsdk::unreal {

class UBoolProperty : public UProperty {
   public:
    UBoolProperty() = delete;
    UBoolProperty(const UBoolProperty&) = delete;
    UBoolProperty(UBoolProperty&&) = delete;
    UBoolProperty& operator=(const UBoolProperty&) = delete;
    UBoolProperty& operator=(UBoolProperty&&) = delete;
    ~UBoolProperty() = delete;

    using field_mask_type = UNREALSDK_UBOOLPROPERTY_FIELD_MASK_TYPE;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_UBOOLPROPERTY_FIELDS(X) X(field_mask_type, FieldMask)

    UNREALSDK_DEFINE_FIELDS_HEADER(UBoolProperty, UNREALSDK_UBOOLPROPERTY_FIELDS);
};

template <>
struct PropTraits<UBoolProperty> : public AbstractPropTraits<UBoolProperty> {
    using Value = bool;

    static Value get(const UBoolProperty* prop, uintptr_t addr, const UnrealPointer<void>& parent);
    static void set(const UBoolProperty* prop, uintptr_t addr, const Value& value);
};

template <>
struct ClassTraits<UBoolProperty> {
    static inline const wchar_t* const NAME = L"BoolProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_PROPERTIES_UBOOLPROPERTY_H */
