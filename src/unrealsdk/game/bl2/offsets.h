#ifndef UNREALSDK_GAME_BL2_OFFSETS_H
#define UNREALSDK_GAME_BL2_OFFSETS_H

#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/classes/properties/attribute_property.h"
#include "unrealsdk/unreal/classes/properties/persistent_object_ptr_property.h"
#include "unrealsdk/unreal/classes/properties/uarrayproperty.h"
#include "unrealsdk/unreal/classes/properties/ubyteproperty.h"
#include "unrealsdk/unreal/classes/properties/uclassproperty.h"
#include "unrealsdk/unreal/classes/properties/udelegateproperty.h"
#include "unrealsdk/unreal/classes/properties/uenumproperty.h"
#include "unrealsdk/unreal/classes/properties/uinterfaceproperty.h"
#include "unrealsdk/unreal/classes/properties/umulticastdelegateproperty.h"
#include "unrealsdk/unreal/classes/properties/uobjectproperty.h"
#include "unrealsdk/unreal/classes/properties/ustructproperty.h"
#include "unrealsdk/unreal/classes/uconst.h"
#include "unrealsdk/unreal/classes/uenum.h"
#include "unrealsdk/unreal/classes/ufield.h"
#include "unrealsdk/unreal/classes/uscriptstruct.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/structs/fname.h"
#include "unrealsdk/unreal/structs/tarray.h"

#if defined(UNREALSDK_FEAT_WILLOW_COMMON)

namespace unrealsdk::unreal {

struct FImplementedInterface;

}

namespace unrealsdk::game::bl2 {

#if defined(_MSC_VER) && UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW
#pragma pack(push, 0x4)
#endif
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#endif
// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init,
//             readability-identifier-naming,
//             readability-magic-numbers)

class UClass;
class UProperty;

namespace generic {

struct ObjectFlagsInternal {
#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW
    uint64_t Bits;
#else
    uint32_t Lower;
    uint32_t Upper;
#endif
};

// This is generic just so that we can reuse it in bl1+tps, but swap out the type of Class
// The usage of TPointer is for enhanced keeping the 4 byte alignment requirement but also keeps
// the 'actual' type known, the runtime will cast it regardless.
template <typename UClass>
class UObject {
   private:
    Pointer vftable;
    Pointer HashNext;

   public:
    ObjectFlagsInternal ObjectFlags;

   private:
    Pointer HashOuterNext;
    Pointer StateFrame;
    TPointer<UObject<UClass>> _Linker;
    Pointer _LinkerIndex;

   public:
    int32_t InternalIndex;

   private:
    int32_t NetIndex;

   public:
    TPointer<UObject<UClass>> Outer;
    unreal::FName Name;
    TPointer<UClass> Class;

   private:
    TPointer<UObject<UClass>> ObjectArchetype;
};

template <typename T>
class UFunction : public T {
   public:
    uint32_t FunctionFlags;

   private:
    uint16_t iNative;
    uint16_t RepOffset;
    unreal::FName FriendlyName;
    uint8_t OperPrecedence;

   public:
    uint8_t NumParams;
    uint16_t ParamsSize;
    uint16_t ReturnValueOffset;

   private:
    uint8_t UnknownData00[0x6];
    Pointer Func;
};

template <typename T>
class UEnum : public T {
   public:
    unreal::TArray<unreal::FName> Names;
};

template <typename T>
class UBoolProperty : public T {
   public:
    uint32_t FieldMask;
};

}  // namespace generic

using UObject = bl2::generic::UObject<UClass>;
using UField = unreal::offsets::generic::UField<UObject>;

class UProperty : public UField {
   public:
    int32_t ArrayDim;
    int32_t ElementSize;
    uint32_t PropertyFlags;

   private:
    uint8_t UnknownData00[0x14];

   public:
    int32_t Offset_Internal;
    TPointer<unreal::UProperty> PropertyLinkNext;

   private:
    uint8_t UnknownData01[0x18];
};

class UStruct : public UField {
   private:
    uint8_t UnknownData00[0x8];

   public:
    TPointer<UStruct> SuperField;
    TPointer<UField> Children;
    uint16_t PropertySize;

   private:
    uint8_t UnknownData01[0x1A];

   public:
    TPointer<UProperty> PropertyLink;

   private:
    uint8_t UnknownData02[0x10];

    unreal::TArray<TPointer<UObject>> ScriptObjectReferences;
};

class UClass : public UStruct {
    // Misc Fields I found within this block in BL2, but which we really don't care about

    // 0xE8: TArray<FRepRecord> ClassReps;
    // 0xF4: TArray<UField*> NetFields;
    // 0x100: TArray<FName> HideCategories;
    // 0x10C: TArray<FName> AutoExpandCategories;

   private:
    uint8_t UnknownData00[0xCC];

   public:
    TPointer<UObject> ClassDefaultObject;

   private:
    uint8_t UnknownData01[0x48];

   public:
    unreal::TArray<unreal::FImplementedInterface> Interfaces;
};

using UScriptStruct = unreal::offsets::generic::UScriptStruct<UStruct>;
using UFunction = bl2::generic::UFunction<UStruct>;
using UConst = unreal::offsets::generic::UConst<UField>;
using UEnum = bl2::generic::UEnum<UField>;

using UArrayProperty = unreal::offsets::generic::UArrayProperty<UProperty>;
using UBoolProperty = bl2::generic::UBoolProperty<UProperty>;
using UByteProperty = unreal::offsets::generic::UByteProperty<UProperty>;
using UDelegateProperty = unreal::offsets::generic::UDelegateProperty<UProperty>;
using UEnumProperty = unreal::offsets::generic::UEnumProperty<UProperty>;
class UFloatProperty : public UProperty {};
using UInterfaceProperty = unreal::offsets::generic::UInterfaceProperty<UProperty>;
class UIntProperty : public UProperty {};
using UMulticastDelegateProperty = unreal::offsets::generic::UMulticastDelegateProperty<UProperty>;
using UObjectProperty = unreal::offsets::generic::UObjectProperty<UProperty>;
using UStructProperty = unreal::offsets::generic::UStructProperty<UProperty>;

using UByteAttributeProperty = unreal::offsets::generic::GenericAttributeProperty<UByteProperty>;
using UClassProperty = unreal::offsets::generic::UClassProperty<UObjectProperty>;
using UFloatAttributeProperty = unreal::offsets::generic::GenericAttributeProperty<UFloatProperty>;
using UIntAttributeProperty = unreal::offsets::generic::GenericAttributeProperty<UIntProperty>;
using USoftClassProperty = unreal::offsets::generic::USoftClassProperty<UObjectProperty>;

// NOLINTEND(cppcoreguidelines-pro-type-member-init,
//           readability-identifier-naming,
//           readability-magic-numbers)
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#if defined(_MSC_VER) && UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW
#pragma pack(pop)
#endif

}  // namespace unrealsdk::game::bl2

#endif

#endif /* UNREALSDK_GAME_BL2_OFFSETS_H */
