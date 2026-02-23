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

class ZGbxDefPtrProperty : public ZProperty {
   public:
    ZGbxDefPtrProperty() = delete;
    ZGbxDefPtrProperty(const ZGbxDefPtrProperty&) = delete;
    ZGbxDefPtrProperty(ZGbxDefPtrProperty&&) = delete;
    ZGbxDefPtrProperty& operator=(const ZGbxDefPtrProperty&) = delete;
    ZGbxDefPtrProperty& operator=(ZGbxDefPtrProperty&&) = delete;
    ~ZGbxDefPtrProperty() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_ZGBXDEFPTRPROPERTY_FIELDS(X) X(UScriptStruct*, Struct)

    UNREALSDK_DEFINE_FIELDS_HEADER(ZGbxDefPtrProperty, UNREALSDK_ZGBXDEFPTRPROPERTY_FIELDS);
};

template <>
struct PropTraits<ZGbxDefPtrProperty> : public AbstractPropTraits<ZGbxDefPtrProperty> {
    // TODO: we may need to switch to a wrapper type if ownership matters
    using Value = FGbxDefPtr;

    static Value get(const ZGbxDefPtrProperty* prop,
                     uintptr_t addr,
                     const UnrealPointer<void>& parent);
    static void set(const ZGbxDefPtrProperty* prop, uintptr_t addr, const Value& value);
    static void destroy(const ZGbxDefPtrProperty* prop, uintptr_t addr);
};

template <>
struct ClassTraits<ZGbxDefPtrProperty> {
    static inline const wchar_t* const NAME = L"GbxDefPtrProperty";
};
}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_STRUCTS_FGBXDEFPTRPROPERTY_H */
