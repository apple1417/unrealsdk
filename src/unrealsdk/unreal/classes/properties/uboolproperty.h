#ifndef UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZBOOLPROPERTY_H
#define UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZBOOLPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/uproperty.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"

namespace unrealsdk::unreal {

class ZBoolProperty : public ZProperty {
   public:
    ZBoolProperty() = delete;
    ZBoolProperty(const ZBoolProperty&) = delete;
    ZBoolProperty(ZBoolProperty&&) = delete;
    ZBoolProperty& operator=(const ZBoolProperty&) = delete;
    ZBoolProperty& operator=(ZBoolProperty&&) = delete;
    ~ZBoolProperty() = delete;

    using field_mask_type = UNREALSDK_ZBOOLPROPERTY_FIELD_MASK_TYPE;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_ZBOOLPROPERTY_FIELDS(X) X(field_mask_type, FieldMask)

    UNREALSDK_DEFINE_FIELDS_HEADER(ZBoolProperty, UNREALSDK_ZBOOLPROPERTY_FIELDS);
};

template <>
struct PropTraits<ZBoolProperty> : public AbstractPropTraits<ZBoolProperty> {
    using Value = bool;

    static Value get(const ZBoolProperty* prop, uintptr_t addr, const UnrealPointer<void>& parent);
    static void set(const ZBoolProperty* prop, uintptr_t addr, const Value& value);
};

template <>
struct ClassTraits<ZBoolProperty> {
    static inline const wchar_t* const NAME = L"BoolProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZBOOLPROPERTY_H */
