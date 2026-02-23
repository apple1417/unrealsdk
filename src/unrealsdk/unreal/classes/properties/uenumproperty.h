#ifndef UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZENUMPROPERTY_H
#define UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZENUMPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/uproperty.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"

namespace unrealsdk::unreal {

class UEnum;

class ZEnumProperty : public ZProperty {
   public:
    ZEnumProperty() = delete;
    ZEnumProperty(const ZEnumProperty&) = delete;
    ZEnumProperty(ZEnumProperty&&) = delete;
    ZEnumProperty& operator=(const ZEnumProperty&) = delete;
    ZEnumProperty& operator=(ZEnumProperty&&) = delete;
    ~ZEnumProperty() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_ZENUMPROPERTY_FIELDS(X) \
    X(ZProperty*, UnderlyingProp)         \
    X(UEnum*, Enum)

    UNREALSDK_DEFINE_FIELDS_HEADER(ZEnumProperty, UNREALSDK_ZENUMPROPERTY_FIELDS);
};

template <>
struct PropTraits<ZEnumProperty> : public AbstractPropTraits<ZEnumProperty> {
    using Value = int64_t;

    static Value get(const ZEnumProperty* prop, uintptr_t addr, const UnrealPointer<void>& parent);
    static void set(const ZEnumProperty* prop, uintptr_t addr, const Value& value);
};

template <>
struct ClassTraits<ZEnumProperty> {
    static inline const wchar_t* const NAME = L"EnumProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZENUMPROPERTY_H */
