#ifndef UNREALSDK_UNREAL_CLASSES_PROPERTIES_UINTERFACEPROPERTY_H
#define UNREALSDK_UNREAL_CLASSES_PROPERTIES_UINTERFACEPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/uproperty.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"

namespace unrealsdk::unreal {

class UClass;
class UObject;

class UInterfaceProperty : public UProperty {
   public:
    UInterfaceProperty() = delete;
    UInterfaceProperty(const UInterfaceProperty&) = delete;
    UInterfaceProperty(UInterfaceProperty&&) = delete;
    UInterfaceProperty& operator=(const UInterfaceProperty&) = delete;
    UInterfaceProperty& operator=(UInterfaceProperty&&) = delete;
    ~UInterfaceProperty() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_UINTERFACEPROPERTY_FIELDS(X) X(UClass*, InterfaceClass)

    UNREALSDK_DEFINE_FIELDS_HEADER(UInterfaceProperty, UNREALSDK_UINTERFACEPROPERTY_FIELDS);
};

template <>
struct PropTraits<UInterfaceProperty> : public AbstractPropTraits<UInterfaceProperty> {
    using Value = UObject*;

    static Value get(const UInterfaceProperty* prop,
                     uintptr_t addr,
                     const UnrealPointer<void>& parent);
    static void set(const UInterfaceProperty* prop, uintptr_t addr, const Value& value);
};

template <>
struct ClassTraits<UInterfaceProperty> {
    static inline const wchar_t* const NAME = L"InterfaceProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_PROPERTIES_UINTERFACEPROPERTY_H */
