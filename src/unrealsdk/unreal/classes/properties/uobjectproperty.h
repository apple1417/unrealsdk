#ifndef UNREALSDK_UNREAL_CLASSES_PROPERTIES_UOBJECTPROPERTY_H
#define UNREALSDK_UNREAL_CLASSES_PROPERTIES_UOBJECTPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/uproperty.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"

namespace unrealsdk::unreal {

class UClass;
class UObject;

class UObjectProperty : public UProperty {
   public:
    UObjectProperty() = delete;
    UObjectProperty(const UObjectProperty&) = delete;
    UObjectProperty(UObjectProperty&&) = delete;
    UObjectProperty& operator=(const UObjectProperty&) = delete;
    UObjectProperty& operator=(UObjectProperty&&) = delete;
    ~UObjectProperty() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_UOBJECTPROPERTY_FIELDS(X) X(UClass*, PropertyClass)

    UNREALSDK_DEFINE_FIELDS_HEADER(UObject, UNREALSDK_UOBJECTPROPERTY_FIELDS);
};

template <>
struct PropTraits<UObjectProperty> : public AbstractPropTraits<UObjectProperty> {
    using Value = UObject*;

    static Value get(const UObjectProperty* prop,
                     uintptr_t addr,
                     const UnrealPointer<void>& parent);
    static void set(const UObjectProperty* prop, uintptr_t addr, const Value& value);
};

template <>
struct ClassTraits<UObjectProperty> {
    static inline const wchar_t* const NAME = L"ObjectProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_PROPERTIES_UOBJECTPROPERTY_H */
