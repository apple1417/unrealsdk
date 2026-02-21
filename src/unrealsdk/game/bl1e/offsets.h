#ifndef UNREALSDK_GAME_BL1E_OFFSETS_H
#define UNREALSDK_GAME_BL1E_OFFSETS_H

#include "unrealsdk/pch.h"
#include "unrealsdk/game/bl1/offsets.h"
#include "unrealsdk/game/bl2/offsets.h"
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
#include "unrealsdk/unreal/classes/uscriptstruct.h"
#include "unrealsdk/unreal/offsets.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW64

namespace unrealsdk::unreal {
struct FImplementedInterface;
}

namespace unrealsdk::game::bl1e {

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#endif

// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init,
//             readability-identifier-naming,
//             readability-magic-numbers)

class UClass;
class UStruct;

using UObject = bl2::generic::UObject<UClass>;

// Size=104
class UField : public UObject {
   public:
    TPointer<UField> Next;
};

// Size=184b
class UProperty : public UField {
   public:
    int32_t ArrayDim;        // 104b
    int32_t ElementSize;     // 108b
    uint32_t PropertyFlags;  // 112b
    uint8_t UnknownData00[0x18];
    int32_t Offset_Internal;               // 140b
    TPointer<UProperty> PropertyLinkNext;  // 144b
    uint8_t UnknownData01[0x20];
};

// Size=208
class UStruct : public UField {
   public:
    uint8_t UnknownData00[0x10];
    TPointer<UStruct> SuperField;  // 120b
    TPointer<UField> Children;     // 128b
    uint16_t PropertySize;         // 136b
    uint8_t UnknownData01[0x26];
    TPointer<UProperty> PropertyLink;  // 176b
    uint8_t UnknownData02[0x8];
    unreal::TArray<TPointer<UObject>> ScriptObjectReferences;  // 192b
};

// Size=1000 ish ( atleast based on struct size for Core.Class )
class UClass : public UStruct {
   public:
    uint8_t UnknownData00[0x104];
    TPointer<UObject> ClassDefaultObject;  // 468b
    uint8_t UnknownData01[0x60];
    unreal::TArray<unreal::FImplementedInterface> Interfaces;  // 572b
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

}  // namespace unrealsdk::game::bl1e

#endif

#endif /* UNREALSDK_GAME_BL1_OFFSETS_H */
