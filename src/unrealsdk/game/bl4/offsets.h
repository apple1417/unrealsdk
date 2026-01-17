#ifndef UNREALSDK_GAME_BL4_OFFSETS_H
#define UNREALSDK_GAME_BL4_OFFSETS_H

#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/offsets_generic.h"
#include "unrealsdk/unreal/structs/fname.h"
#include "unrealsdk/unreal/structs/gnames.h"
#include "unrealsdk/unreal/structs/tpair.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2

namespace unrealsdk::unreal {

struct FImplementedInterface;

}  // namespace unrealsdk::unreal

namespace unrealsdk::game::bl4 {

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#endif
// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init,
//             readability-identifier-naming,
//             readability-magic-numbers)

class UClass;
class UProperty;
struct FField;

using FFieldClass = unreal::offsets::generic::FFieldClass;                // Validated
struct FField : public unreal::offsets::generic::FField<FFieldClass> {};  // Validated

class UObject {  // Validated
   private:
    uintptr_t* vftable;

   public:
    int32_t ObjectFlags;
    int32_t InternalIndex;
    UClass* Class;
    unreal::FName Name;
    UObject* Outer;
};

using UField = unreal::offsets::generic::UField<UObject>;  // validated

class UStruct : public UField {
   private:
    uint8_t UnknownData00[0x10];

   public:
    UStruct* SuperField;
    UField* Children;
    FField* ChildProperties;
    int32_t PropertySize;
    int32_t MinAlignment;

   private:
    unreal::TArray<uint8_t> Script;

   public:
    UProperty* PropertyLink;

   private:  // Validated up to this point
    UProperty* RefLink;
    UProperty* DestructorLink;
    UProperty* PostConstructLink;
    unreal::TArray<UObject*> ScriptObjectReferences;
};

class UClass : public UStruct {
   private:
    uint8_t UnknownData00[0x70];

   public:
    UObject* ClassDefaultObject;

   private:
    uint8_t UnknownData01[0xA0];

   public:
    unreal::TArray<unreal::FImplementedInterface> Interfaces;
};

using UScriptStruct = unreal::offsets::generic::UScriptStruct<UStruct>;

class UFunction : public UStruct {
   public:
    uint32_t FunctionFlags;
    uint8_t NumParams;
    uint16_t ParamsSize;
    uint16_t ReturnValueOffset;

   private:
    uint16_t RPCId;
    uint16_t RPCResponseId;
    UProperty* FirstPropertyToInit;
    UFunction* EventGraphFunction;
    int32_t EventGraphCallOffset;
    void* Func;
};

using UConst = unreal::offsets::generic::UConst<UField>;

class UEnum : public UField {
   private:
    unreal::UnmanagedFString CppType;

   public:
    unreal::TArray<unreal::TPair<unreal::FName, uint64_t>> Names;

   private:
    int64_t CppForm;
};

class UProperty : public FField {
   public:
    int32_t ArrayDim;
    int32_t ElementSize;
    uint64_t PropertyFlags;

   private:
    uint16_t RepIndex;
    uint8_t BlueprintReplicationCondition;
    uint8_t UnknownData00[1];

   public:
    int32_t Offset_Internal;

   public:
    UProperty* PropertyLinkNext;

   private:
    UProperty* NextRef;
    UProperty* DestructorLinkNext;
    UProperty* PostConstructLinkNext;
    unreal::FName RepNotifyFunc;
};

class UArrayProperty : public UProperty {  // validated
   public:
    uint64_t ArrayFlags;
    UProperty* Inner;
};
using UByteProperty = unreal::offsets::generic::UByteProperty<UProperty>;
using UDelegateProperty = unreal::offsets::generic::UDelegateProperty<UProperty>;
using UEnumProperty = unreal::offsets::generic::UEnumProperty<UProperty>;
class UFloatProperty : public UProperty {};
using UInterfaceProperty = unreal::offsets::generic::UInterfaceProperty<UProperty>;
class UIntProperty : public UProperty {};
using UMulticastDelegateProperty = unreal::offsets::generic::UMulticastDelegateProperty<UProperty>;
using UObjectProperty = unreal::offsets::generic::UObjectProperty<UProperty>;
using UStructProperty = unreal::offsets::generic::UStructProperty<UProperty>;  // validated

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

struct FNameEntry {
    static const constexpr auto META_SIZE_BIT_OFFSET = 6;

    union {
        uint8_t Flags;
        uint16_t Metadata;
    };
    unreal::FNameEntry::name_union Name;
};  // Validated

struct FFrame {
   private:
    void* VfTable;
    uint32_t bSuppressEventTag;
    uint32_t bAutoEmitLineTerminator;

   public:
    UFunction* Node;
    UObject* Object;
    uint8_t* Code;

   private:
    void* Locals;
    UProperty* LastProperty;
    void* LastPropertyAddress;

   public:
    FFrame* PreviousFrame;

   private:
    void* OutParams;
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
