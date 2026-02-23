#ifndef UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZMULTICASTDELEGATEPROPERTY_H
#define UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZMULTICASTDELEGATEPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/uproperty.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/wrappers/bound_function.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"
#include "unrealsdk/unreal/wrappers/wrapped_multicast_delegate.h"

namespace unrealsdk::unreal {

class UFunction;

class ZMulticastDelegateProperty : public ZProperty {
   public:
    ZMulticastDelegateProperty() = delete;
    ZMulticastDelegateProperty(const ZMulticastDelegateProperty&) = delete;
    ZMulticastDelegateProperty(ZMulticastDelegateProperty&&) = delete;
    ZMulticastDelegateProperty& operator=(const ZMulticastDelegateProperty&) = delete;
    ZMulticastDelegateProperty& operator=(ZMulticastDelegateProperty&&) = delete;
    ~ZMulticastDelegateProperty() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_ZMULTICASTDELEGATEPROPERTY_FIELDS(X) X(UFunction*, Signature)

    UNREALSDK_DEFINE_FIELDS_HEADER(ZMulticastDelegateProperty,
                                   UNREALSDK_ZMULTICASTDELEGATEPROPERTY_FIELDS);
};

template <>
struct PropTraits<ZMulticastDelegateProperty>
    : public AbstractPropTraits<ZMulticastDelegateProperty> {
    using Value = WrappedMulticastDelegate;

    static Value get(const ZMulticastDelegateProperty* prop,
                     uintptr_t addr,
                     const UnrealPointer<void>& parent);
    static void set(const ZMulticastDelegateProperty* prop, uintptr_t addr, const Value& value);
    static void destroy(const ZMulticastDelegateProperty* prop, uintptr_t addr);
};

template <>
struct ClassTraits<ZMulticastDelegateProperty> {
    static inline const wchar_t* const NAME = L"MulticastDelegateProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZMULTICASTDELEGATEPROPERTY_H */
