#ifndef UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZOBJECTPROPERTY_H
#define UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZOBJECTPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/uproperty.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"

namespace unrealsdk::unreal {

class UClass;
class UObject;

class ZObjectProperty : public ZProperty {
   public:
    ZObjectProperty() = delete;
    ZObjectProperty(const ZObjectProperty&) = delete;
    ZObjectProperty(ZObjectProperty&&) = delete;
    ZObjectProperty& operator=(const ZObjectProperty&) = delete;
    ZObjectProperty& operator=(ZObjectProperty&&) = delete;
    ~ZObjectProperty() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_ZOBJECTPROPERTY_FIELDS(X) X(UClass*, PropertyClass)

    UNREALSDK_DEFINE_FIELDS_HEADER(UObject, UNREALSDK_ZOBJECTPROPERTY_FIELDS);
};

template <>
struct PropTraits<ZObjectProperty> : public AbstractPropTraits<ZObjectProperty> {
    using Value = UObject*;

    static Value get(const ZObjectProperty* prop,
                     uintptr_t addr,
                     const UnrealPointer<void>& parent);
    static void set(const ZObjectProperty* prop, uintptr_t addr, const Value& value);
};

template <>
struct ClassTraits<ZObjectProperty> {
    static inline const wchar_t* const NAME = L"ObjectProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZOBJECTPROPERTY_H */
