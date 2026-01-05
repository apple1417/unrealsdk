#ifndef UNREALSDK_UNREAL_CLASSES_PROPERTIES_UCOMPONENTPROPERTY_H
#define UNREALSDK_UNREAL_CLASSES_PROPERTIES_UCOMPONENTPROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/properties/uobjectproperty.h"
#include "unrealsdk/unreal/prop_traits.h"

namespace unrealsdk::unreal {

class UComponentProperty : public UObjectProperty {
   public:
    UComponentProperty() = delete;
    UComponentProperty(const UComponentProperty&) = delete;
    UComponentProperty(UComponentProperty&&) = delete;
    UComponentProperty& operator=(const UComponentProperty&) = delete;
    UComponentProperty& operator=(UComponentProperty&&) = delete;
    ~UComponentProperty() = delete;
};

template <>
struct PropTraits<UComponentProperty> : public PropTraits<UObjectProperty> {};

template <>
struct ClassTraits<UComponentProperty> {
    static inline const wchar_t* const NAME = L"ComponentProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_PROPERTIES_UCOMPONENTPROPERTY_H */
