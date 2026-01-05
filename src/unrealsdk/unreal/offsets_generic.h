#ifndef UNREALSDK_UNREAL_OFFSETS_GENERIC_H
#define UNREALSDK_UNREAL_OFFSETS_GENERIC_H

#include "unrealsdk/unreal/structs/fstring.h"

// This file contains a number of templates for simpler unreal types, which you can use to define
// offsets in a one-liner:
//     using UField = unreal::offsets::generic::UField<UObject>;

namespace unrealsdk::unreal {

class UArrayProperty;
class UClass;
class UEnum;
class UFunction;
class UProperty;
class UScriptStruct;

namespace offsets::generic {

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#endif
// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init,
//             readability-identifier-naming,
//             readability-magic-numbers)

template <typename T>
class GenericAttributeProperty : public T {
   public:
    unreal::UArrayProperty* ModifierStackProperty;
    GenericAttributeProperty<T>* OtherAttributeProperty;
};

template <typename T>
class UArrayProperty : public T {
   public:
    unreal::UProperty* Inner;
};

template <typename T>
class UByteProperty : public T {
   public:
    unreal::UEnum* Enum;
};

template <typename T>
class UClassProperty : public T {
   public:
    unreal::UClass* MetaClass;
};

template <typename T>
class UConst : public T {
   public:
    UnmanagedFString Value;
};

template <typename T>
class UDelegateProperty : public T {
   public:
    unreal::UFunction* Signature;
};

template <typename T>
class UEnumProperty : public T {
   public:
    unreal::UProperty* UnderlyingProp;
    unreal::UEnum* Enum;
};

template <typename T>
class UField : public T {
   public:
    UField* Next;
};
template <typename T>
class UInterfaceProperty : public T {
   public:
    unreal::UClass* InterfaceClass;
};

template <typename T>
class UMulticastDelegateProperty : public T {
   public:
    unreal::UFunction* Signature;
};

template <typename T>
class UObjectProperty : public T {
   public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    unreal::UClass* PropertyClass;
};

template <typename T>
class UScriptStruct : public T {
   public:
    uint32_t StructFlags;
};

template <typename T>
class USoftClassProperty : public T {
   public:
    unreal::UClass* MetaClass;
};

template <typename T>
class UStructProperty : public T {
   public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    unreal::UScriptStruct* Struct;
};

// NOLINTEND(cppcoreguidelines-pro-type-member-init,
//           readability-identifier-naming,
//           readability-magic-numbers)
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

}  // namespace offsets::generic

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_OFFSETS_GENERIC_H */
