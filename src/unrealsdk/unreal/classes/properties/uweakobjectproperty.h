#ifndef UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZWEAKOBJECTPROPERTY_H
#define UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZWEAKOBJECTPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/properties/uobjectproperty.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"

namespace unrealsdk::unreal {

class UObject;

class ZWeakObjectProperty : public ZObjectProperty {
   public:
    ZWeakObjectProperty() = delete;
    ZWeakObjectProperty(const ZWeakObjectProperty&) = delete;
    ZWeakObjectProperty(ZWeakObjectProperty&&) = delete;
    ZWeakObjectProperty& operator=(const ZWeakObjectProperty&) = delete;
    ZWeakObjectProperty& operator=(ZWeakObjectProperty&&) = delete;
    ~ZWeakObjectProperty() = delete;
};

template <>
struct PropTraits<ZWeakObjectProperty> : public AbstractPropTraits<ZWeakObjectProperty> {
    using Value = UObject*;

    static Value get(const ZWeakObjectProperty* prop,
                     uintptr_t addr,
                     const UnrealPointer<void>& parent);
    static void set(const ZWeakObjectProperty* prop, uintptr_t addr, const Value& value);
};

template <>
struct ClassTraits<ZWeakObjectProperty> {
    static inline const wchar_t* const NAME = L"WeakObjectProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZWEAKOBJECTPROPERTY_H */
