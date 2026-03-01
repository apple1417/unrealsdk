#ifndef UNREALSDK_UNREAL_PROPERTIES_ZTEXTPROPERTY_H
#define UNREALSDK_UNREAL_PROPERTIES_ZTEXTPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/properties/zproperty.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"

namespace unrealsdk::unreal {

class ZTextProperty : public ZProperty {
   public:
    ZTextProperty() = delete;
    ZTextProperty(const ZTextProperty&) = delete;
    ZTextProperty(ZTextProperty&&) = delete;
    ZTextProperty& operator=(const ZTextProperty&) = delete;
    ZTextProperty& operator=(ZTextProperty&&) = delete;
    ~ZTextProperty() = delete;
};

template <>
struct PropTraits<ZTextProperty> : public AbstractPropTraits<ZTextProperty> {
    using Value = std::wstring;

    static Value get(const ZTextProperty* prop, uintptr_t addr, const UnrealPointer<void>& parent);
    static void set(const ZTextProperty* prop, uintptr_t addr, const Value& value);
    static void destroy(const ZTextProperty* prop, uintptr_t addr);
};

template <>
struct ClassTraits<ZTextProperty> {
    static inline const wchar_t* const NAME = L"TextProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_PROPERTIES_ZTEXTPROPERTY_H */
