#ifndef UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZDELEGATEPROPERTY_H
#define UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZDELEGATEPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/uproperty.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/wrappers/bound_function.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"

namespace unrealsdk::unreal {

class UFunction;

class ZDelegateProperty : public ZProperty {
   public:
    ZDelegateProperty() = delete;
    ZDelegateProperty(const ZDelegateProperty&) = delete;
    ZDelegateProperty(ZDelegateProperty&&) = delete;
    ZDelegateProperty& operator=(const ZDelegateProperty&) = delete;
    ZDelegateProperty& operator=(ZDelegateProperty&&) = delete;
    ~ZDelegateProperty() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_ZDELEGATEPROPERTY_FIELDS(X) X(UFunction*, Signature)

    UNREALSDK_DEFINE_FIELDS_HEADER(ZDelegateProperty, UNREALSDK_ZDELEGATEPROPERTY_FIELDS);
};

template <>
struct PropTraits<ZDelegateProperty> : public AbstractPropTraits<ZDelegateProperty> {
    using Value = std::optional<BoundFunction>;

    static Value get(const ZDelegateProperty* prop,
                     uintptr_t addr,
                     const UnrealPointer<void>& parent);
    static void set(const ZDelegateProperty* prop, uintptr_t addr, const Value& value);
};

template <>
struct ClassTraits<ZDelegateProperty> {
    static inline const wchar_t* const NAME = L"DelegateProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZDELEGATEPROPERTY_H */
