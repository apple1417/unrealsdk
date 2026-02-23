#ifndef UNREALSDK_UNREAL_PROPERTIES_ZCOMPONENTPROPERTY_H
#define UNREALSDK_UNREAL_PROPERTIES_ZCOMPONENTPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/properties/zobjectproperty.h"

namespace unrealsdk::unreal {

class ZComponentProperty : public ZObjectProperty {
   public:
    ZComponentProperty() = delete;
    ZComponentProperty(const ZComponentProperty&) = delete;
    ZComponentProperty(ZComponentProperty&&) = delete;
    ZComponentProperty& operator=(const ZComponentProperty&) = delete;
    ZComponentProperty& operator=(ZComponentProperty&&) = delete;
    ~ZComponentProperty() = delete;
};

template <>
struct PropTraits<ZComponentProperty> : public PropTraits<ZObjectProperty> {};

template <>
struct ClassTraits<ZComponentProperty> {
    static inline const wchar_t* const NAME = L"ComponentProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_PROPERTIES_ZCOMPONENTPROPERTY_H */
