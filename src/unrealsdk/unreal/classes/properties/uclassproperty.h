#ifndef UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZCLASSPROPERTY_H
#define UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZCLASSPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/properties/uobjectproperty.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"

namespace unrealsdk::unreal {

class UClass;

class ZClassProperty : public ZObjectProperty {
   public:
    ZClassProperty() = delete;
    ZClassProperty(const ZClassProperty&) = delete;
    ZClassProperty(ZClassProperty&&) = delete;
    ZClassProperty& operator=(const ZClassProperty&) = delete;
    ZClassProperty& operator=(ZClassProperty&&) = delete;
    ~ZClassProperty() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_ZCLASSPROPERTY_FIELDS(X) X(UClass*, MetaClass)

    UNREALSDK_DEFINE_FIELDS_HEADER(ZClassProperty, UNREALSDK_ZCLASSPROPERTY_FIELDS);
};

template <>
struct PropTraits<ZClassProperty> : public AbstractPropTraits<ZClassProperty> {
    using Value = UClass*;

    static Value get(const ZClassProperty* prop, uintptr_t addr, const UnrealPointer<void>& parent);
    static void set(const ZClassProperty* prop, uintptr_t addr, const Value& value);
};

template <>
struct ClassTraits<ZClassProperty> {
    static inline const wchar_t* const NAME = L"ClassProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZCLASSPROPERTY_H */
