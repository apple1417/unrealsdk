#ifndef UNREALSDK_UNREAL_PROPERTIES_ZARRAYPROPERTY_H
#define UNREALSDK_UNREAL_PROPERTIES_ZARRAYPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/properties/zproperty.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"
#include "unrealsdk/unreal/wrappers/wrapped_array.h"

namespace unrealsdk::unreal {

class ZArrayProperty : public ZProperty {
   public:
    ZArrayProperty() = delete;
    ZArrayProperty(const ZArrayProperty&) = delete;
    ZArrayProperty(ZArrayProperty&&) = delete;
    ZArrayProperty& operator=(const ZArrayProperty&) = delete;
    ZArrayProperty& operator=(ZArrayProperty&&) = delete;
    ~ZArrayProperty() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_ZARRAYPROPERTY_FIELDS(X) X(ZProperty*, Inner)

    UNREALSDK_DEFINE_FIELDS_HEADER(ZArrayProperty, UNREALSDK_ZARRAYPROPERTY_FIELDS);
};

template <>
struct PropTraits<ZArrayProperty> : public AbstractPropTraits<ZArrayProperty> {
    using Value = WrappedArray;

    static Value get(const ZArrayProperty* prop, uintptr_t addr, const UnrealPointer<void>& parent);
    static void set(const ZArrayProperty* prop, uintptr_t addr, const Value& value);
    static void destroy(const ZArrayProperty* prop, uintptr_t addr);
};

template <>
struct ClassTraits<ZArrayProperty> {
    static inline const wchar_t* const NAME = L"ArrayProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_PROPERTIES_ZARRAYPROPERTY_H */
