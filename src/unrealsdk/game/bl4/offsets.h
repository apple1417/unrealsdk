#ifndef UNREALSDK_GAME_BL4_OFFSETS_H
#define UNREALSDK_GAME_BL4_OFFSETS_H

#include "unrealsdk/game/bl3/offsets.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2

namespace unrealsdk::game::bl4 {

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#endif
// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init,
//             readability-identifier-naming,
//             readability-magic-numbers)

using namespace unrealsdk::game::bl3;

struct FNameEntry {
    static const constexpr auto META_SIZE_BIT_OFFSET = 6;

    union {
        uint8_t Flags;
        uint16_t Metadata;
    };
    unreal::FNameEntry::name_union Name;
};

struct FField;
struct FFieldClass {
   public:
    unreal::FName Name;

   private:
    uint64_t Id;
    uint64_t CastFlags;
    uint32_t ClassFlags;

   public:
    FFieldClass* SuperField;
    FField* DefaultObject;
};

struct FField {
   private:
    uintptr_t* vftable;

   public:
    FFieldClass* Class;
    UObject* Owner;
    FField* Next;
    unreal::FName Name;

   private:
    uint64_t FlagsPrivate;
};

struct UProperty : public FField {
   public:
    int32_t ArrayDim;
    int32_t ElementSize;
    uint64_t PropertyFlags;

   private:
    uint16_t RepIndex;
    uint8_t BlueprintReplicationCondition;

   public:
    int32_t Offset_Internal;

   private:
    unreal::FName RepNotifyFunc;

   public:
    UProperty* PropertyLinkNext;

   private:
    UProperty* NextRef;
    UProperty* DestructorLinkNext;
    UProperty* PostConstructLinkNext;
};

using UArrayProperty = unreal::offsets::generic::UArrayProperty<UProperty>;
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

class UBoolProperty : public UProperty {
   private:
    uint8_t FieldSize;
    uint8_t ByteOffset;
    uint8_t ByteMask;

   public:
    uint8_t FieldMask;
};

// NOLINTEND(cppcoreguidelines-pro-type-member-init,
//           readability-identifier-naming,
//           readability-magic-numbers)
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

}  // namespace unrealsdk::game::bl4

#endif

#endif /* UNREALSDK_GAME_BL4_OFFSETS_H */
