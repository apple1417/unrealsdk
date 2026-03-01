#ifndef UNREALSDK_UNREAL_PROPERTIES_ZINTERFACEPROPERTY_H
#define UNREALSDK_UNREAL_PROPERTIES_ZINTERFACEPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/properties/zproperty.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"

namespace unrealsdk::unreal {

class UClass;
class UObject;

class ZInterfaceProperty : public ZProperty {
   public:
    ZInterfaceProperty() = delete;
    ZInterfaceProperty(const ZInterfaceProperty&) = delete;
    ZInterfaceProperty(ZInterfaceProperty&&) = delete;
    ZInterfaceProperty& operator=(const ZInterfaceProperty&) = delete;
    ZInterfaceProperty& operator=(ZInterfaceProperty&&) = delete;
    ~ZInterfaceProperty() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_ZINTERFACEPROPERTY_FIELDS(X) X(UClass*, InterfaceClass)

    UNREALSDK_DEFINE_FIELDS_HEADER(ZInterfaceProperty, UNREALSDK_ZINTERFACEPROPERTY_FIELDS);
};

template <>
struct PropTraits<ZInterfaceProperty> : public AbstractPropTraits<ZInterfaceProperty> {
    using Value = UObject*;

    static Value get(const ZInterfaceProperty* prop,
                     uintptr_t addr,
                     const UnrealPointer<void>& parent);
    static void set(const ZInterfaceProperty* prop, uintptr_t addr, const Value& value);
};

template <>
struct ClassTraits<ZInterfaceProperty> {
    static inline const wchar_t* const NAME = L"InterfaceProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_PROPERTIES_ZINTERFACEPROPERTY_H */
