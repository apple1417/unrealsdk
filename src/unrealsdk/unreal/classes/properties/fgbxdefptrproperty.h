#ifndef UNREALSDK_UNREAL_STRUCTS_FGBXDEFPTRPROPERTY_H
#define UNREALSDK_UNREAL_STRUCTS_FGBXDEFPTRPROPERTY_H

#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/uproperty.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/structs/fgbxdefptr.h"

namespace unrealsdk::unreal {

/*
========================================
THIS PROPERTY TYPE IS STILL EXPERIMENTAL
========================================

It may currently leak memory and/or cause crashes, and it's semantics are all liable to change
*/

class FGbxDefPtrProperty : public UProperty {
   public:
    FGbxDefPtrProperty() = delete;
    FGbxDefPtrProperty(const FGbxDefPtrProperty&) = delete;
    FGbxDefPtrProperty(FGbxDefPtrProperty&&) = delete;
    FGbxDefPtrProperty& operator=(const FGbxDefPtrProperty&) = delete;
    FGbxDefPtrProperty& operator=(FGbxDefPtrProperty&&) = delete;
    ~FGbxDefPtrProperty() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_FGBXDEFPTRPROPERTY_FIELDS(X) X(UScriptStruct*, Struct)

    UNREALSDK_DEFINE_FIELDS_HEADER(FGbxDefPtrProperty, UNREALSDK_FGBXDEFPTRPROPERTY_FIELDS);
};

template <>
struct PropTraits<FGbxDefPtrProperty> : public AbstractPropTraits<FGbxDefPtrProperty> {
    // TODO: we may need to switch to a wrapper type if ownership matters
    using Value = FGbxDefPtr;

    static Value get(const FGbxDefPtrProperty* prop,
                     uintptr_t addr,
                     const UnrealPointer<void>& parent);
    static void set(const FGbxDefPtrProperty* prop, uintptr_t addr, const Value& value);
    static void destroy(const FGbxDefPtrProperty* prop, uintptr_t addr);
};

template <>
struct ClassTraits<FGbxDefPtrProperty> {
    static inline const wchar_t* const NAME = L"GbxDefPtrProperty";
};
}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_STRUCTS_FGBXDEFPTRPROPERTY_H */
