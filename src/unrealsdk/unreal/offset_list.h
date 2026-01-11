#ifndef UNREALSDK_UNREAL_OFFSET_LIST_H
#define UNREALSDK_UNREAL_OFFSET_LIST_H

#include "unrealsdk/pch.h"

// Note: this header needs to pull in almost all unreal classes, and their offset definitions
// Refrain from including it in other headers
#include "unrealsdk/unreal/classes/properties/attribute_property.h"
#include "unrealsdk/unreal/classes/properties/persistent_object_ptr_property.h"
#include "unrealsdk/unreal/classes/properties/uarrayproperty.h"
#include "unrealsdk/unreal/classes/properties/uboolproperty.h"
#include "unrealsdk/unreal/classes/properties/ubyteproperty.h"
#include "unrealsdk/unreal/classes/properties/uclassproperty.h"
#include "unrealsdk/unreal/classes/properties/udelegateproperty.h"
#include "unrealsdk/unreal/classes/properties/uenumproperty.h"
#include "unrealsdk/unreal/classes/properties/uinterfaceproperty.h"
#include "unrealsdk/unreal/classes/properties/umulticastdelegateproperty.h"
#include "unrealsdk/unreal/classes/properties/uobjectproperty.h"
#include "unrealsdk/unreal/classes/properties/ustructproperty.h"
#include "unrealsdk/unreal/classes/uclass.h"
#include "unrealsdk/unreal/classes/uconst.h"
#include "unrealsdk/unreal/classes/uenum.h"
#include "unrealsdk/unreal/classes/ufield.h"
#include "unrealsdk/unreal/classes/ufunction.h"
#include "unrealsdk/unreal/classes/uobject.h"
#include "unrealsdk/unreal/classes/uproperty.h"
#include "unrealsdk/unreal/classes/uscriptstruct.h"
#include "unrealsdk/unreal/classes/ustruct.h"
#include "unrealsdk/unreal/structs/ffield.h"
#include "unrealsdk/unreal/structs/fframe.h"
#include "unrealsdk/unreal/structs/fname.h"
#include "unrealsdk/unreal/structs/gnames.h"

namespace unrealsdk::unreal::offsets {

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#define UNREALSDK__DYNAMIC_OFFSET_TYPES(X) \
    X(UArrayProperty)                      \
    X(UBoolProperty)                       \
    X(UByteAttributeProperty)              \
    X(UByteProperty)                       \
    X(UClass)                              \
    X(UClassProperty)                      \
    X(UConst)                              \
    X(UDelegateProperty)                   \
    X(UEnum)                               \
    X(UEnumProperty)                       \
    X(UField)                              \
    X(UFloatAttributeProperty)             \
    X(UFunction)                           \
    X(UIntAttributeProperty)               \
    X(UInterfaceProperty)                  \
    X(UMulticastDelegateProperty)          \
    X(UObject)                             \
    X(UObjectProperty)                     \
    X(UProperty)                           \
    X(UScriptStruct)                       \
    X(USoftClassProperty)                  \
    X(UStruct)                             \
    X(UStructProperty)                     \
    X(FNameEntry)                          \
    X(FFrame)                              \
    X(FFieldClass)                         \
    X(FField)

#define UNREALSDK_OFFSETS__DEFINE_OFFSET_LIST_MEMBERS(name) unrealsdk::unreal::name::Offsets name;
#define UNREALSDK_OFFSETS__NESTED_FROM_TYPE(name) \
    unrealsdk::unreal::name::Offsets::from<typename T::name>(),
#define UNREALSDK_OFFSETS__NESTED_FROM_NAMESPACE(name) \
    unrealsdk::unreal::name::Offsets::from<name>(),

// NOLINTEND(cppcoreguidelines-macro-usage)

// Since this type is shared between dlls, also force consistent padding between compilers, even
// though this isn't an unreal type
UNREALSDK_UNREAL_STRUCT_PADDING_PUSH()

struct OffsetList {
    // NOLINTNEXTLINE(readability-identifier-naming)
    UNREALSDK__DYNAMIC_OFFSET_TYPES(UNREALSDK_OFFSETS__DEFINE_OFFSET_LIST_MEMBERS)

    /**
     * @brief Creates a full offset list based off of the templated type.
     * @note Assumes all the game specific types are subtypes - i.e. `T::UObject` is valid.
     *
     * @tparam T The templated type holding all the game-specific types.
     * @return A new offset list.
     */
    template <typename T>
    static constexpr OffsetList from(void) {
        return {UNREALSDK__DYNAMIC_OFFSET_TYPES(UNREALSDK_OFFSETS__NESTED_FROM_TYPE)};
    }
};

UNREALSDK_UNREAL_STRUCT_PADDING_POP()

/**
 * @brief Macro to create a full offset list within the current namespace.
 * @note Assumes all the game specific types are directly accessible - i.e just `UObject` is valid.
 *
 * @return A new offset list.
 */
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define OFFSET_LIST_FROM_NAMESPACE()                                              \
    unrealsdk::unreal::offsets::OffsetList {                                      \
        UNREALSDK__DYNAMIC_OFFSET_TYPES(UNREALSDK_OFFSETS__NESTED_FROM_NAMESPACE) \
    }

}  // namespace unrealsdk::unreal::offsets

#endif /* UNREALSDK_UNREAL_OFFSET_LIST_H */
