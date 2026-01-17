#ifndef UNREALSDK_UNREAL_STRUCTS_FGBXDEFPTRPROPERTY_H
#define UNREALSDK_UNREAL_STRUCTS_FGBXDEFPTRPROPERTY_H

#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/uenum.h"
#include "unrealsdk/unreal/classes/uscriptstruct.h"
#include "unrealsdk/unreal/prop_traits.h"

namespace unrealsdk::unreal {

struct FGbxDefPtr {
    FName name;
    UScriptStruct* ref;
    void* instancePtr;
};

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
    using Value = FGbxDefPtr;

    static Value get(const FGbxDefPtrProperty* prop,
                     uintptr_t addr,
                     const UnrealPointer<void>& parent);
    static void set(const FGbxDefPtrProperty* prop, uintptr_t addr, const Value& value);
};

template <>
struct ClassTraits<FGbxDefPtrProperty> {
    static inline const wchar_t* const NAME = L"GbxDefPtrProperty";
};
}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_STRUCTS_FGBXDEFPTRPROPERTY_H */
