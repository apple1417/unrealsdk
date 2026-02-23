#ifndef UNREALSDK_UNREAL_PROPERTIES_ZSTRUCTPROPERTY_H
#define UNREALSDK_UNREAL_PROPERTIES_ZSTRUCTPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/properties/zproperty.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"
#include "unrealsdk/unreal/wrappers/wrapped_struct.h"

namespace unrealsdk::unreal {

class UScriptStruct;

class ZStructProperty : public ZProperty {
   public:
    ZStructProperty() = delete;
    ZStructProperty(const ZStructProperty&) = delete;
    ZStructProperty(ZStructProperty&&) = delete;
    ZStructProperty& operator=(const ZStructProperty&) = delete;
    ZStructProperty& operator=(ZStructProperty&&) = delete;
    ~ZStructProperty() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_ZSTRUCTPROPERTY_FIELDS(X) X(UScriptStruct*, Struct)

    UNREALSDK_DEFINE_FIELDS_HEADER(ZStructProperty, UNREALSDK_ZSTRUCTPROPERTY_FIELDS);
};

template <>
struct PropTraits<ZStructProperty> : public AbstractPropTraits<ZStructProperty> {
    using Value = WrappedStruct;

    static Value get(const ZStructProperty* prop,
                     uintptr_t addr,
                     const UnrealPointer<void>& parent);
    static void set(const ZStructProperty* prop, uintptr_t addr, const Value& value);
    static void destroy(const ZStructProperty* prop, uintptr_t addr);
};

template <>
struct ClassTraits<ZStructProperty> {
    static inline const wchar_t* const NAME = L"StructProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_PROPERTIES_ZSTRUCTPROPERTY_H */
