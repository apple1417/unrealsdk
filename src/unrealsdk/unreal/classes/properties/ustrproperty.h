#ifndef UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZSTRPROPERTY_H
#define UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZSTRPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/uproperty.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"

namespace unrealsdk::unreal {

class ZStrProperty : public ZProperty {
   public:
    ZStrProperty() = delete;
    ZStrProperty(const ZStrProperty&) = delete;
    ZStrProperty(ZStrProperty&&) = delete;
    ZStrProperty& operator=(const ZStrProperty&) = delete;
    ZStrProperty& operator=(ZStrProperty&&) = delete;
    ~ZStrProperty() = delete;
};

template <>
struct PropTraits<ZStrProperty> : public AbstractPropTraits<ZStrProperty> {
    using Value = std::wstring;

    static Value get(const ZStrProperty* prop, uintptr_t addr, const UnrealPointer<void>& parent);
    static void set(const ZStrProperty* prop, uintptr_t addr, const Value& value);
    static void destroy(const ZStrProperty* prop, uintptr_t addr);
};

template <>
struct ClassTraits<ZStrProperty> {
    static inline const wchar_t* const NAME = L"StrProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_PROPERTIES_ZSTRPROPERTY_H */
