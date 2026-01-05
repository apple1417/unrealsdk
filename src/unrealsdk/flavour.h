#ifndef UNREALSDK_FLAVOUR_H
#define UNREALSDK_FLAVOUR_H

// Everything in here is defined as a macro so that it can be used in preprocessor if needed
// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#define UNREALSDK_FLAVOUR_WILLOW 1
#define UNREALSDK_FLAVOUR_OAK 2
#define UNREALSDK_FLAVOUR_OAK2 3

#define UNREALSDK_GOBJECTS_FORMAT_TARRAY 1
#define UNREALSDK_GOBJECTS_FORMAT_FUOBJECTARRAY 2

// Surely these names will age well...
#define UNREALSDK_ENUM_FORMAT_UE3 1  // TArray of raw names
#define UNREALSDK_ENUM_FORMAT_UE4 2  // Array of type-prefixed name-value pairs

#define UNREALSDK_FIMPLEMENTEDINTERFACE_FORMAT_UE3 1
#define UNREALSDK_FIMPLEMENTEDINTERFACE_FORMAT_UE4 2

// =================================================================================================
#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW
// =================================================================================================

#define UNREALSDK_HAS_FTEXT false
#define UNREALSDK_HAS_OPTIONAL_FUNC_PARAMS true
#define UNREALSDK_USTRUCT_HAS_ALIGNMENT false

#define UNREALSDK_ENUM_FORMAT UNREALSDK_ENUM_FORMAT_UE3
#define UNREALSDK_FIMPLEMENTEDINTERFACE_FORMAT UNREALSDK_FIMPLEMENTEDINTERFACE_FORMAT_UE3
#define UNREALSDK_GOBJECTS_FORMAT UNRUNREALSDK_GOBJECTS_FORMAT_TARRAY

#define UNREALSDK_UBOOLPROPERTY_FIELD_MASK_TYPE uint32_t
#define UNREALSDK_UOBJECT_FLAGS_TYPE uint64_t
#define UNREALSDK_UPROPERTY_FLAGS_TYPE uint32_t
#define UNREALSDK_USTRUCT_PROPERTY_SIZE_TYPE uint16_t

#define UNREALSDK_DEFAULT_POST_EDIT_CHANGE_CHAIN_PROPERTY_VF_IDX 18
#define UNREALSDK_DEFAULT_POST_EDIT_CHANGE_PROPERTY_VF_IDX 19

// =================================================================================================
#elif UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK
// =================================================================================================

#define UNREALSDK_HAS_FTEXT true
#define UNREALSDK_HAS_OPTIONAL_FUNC_PARAMS false
#define UNREALSDK_USTRUCT_HAS_ALIGNMENT true

#define UNREALSDK_ENUM_FORMAT UNREALSDK_ENUM_FORMAT_UE4
#define UNREALSDK_FIMPLEMENTEDINTERFACE_FORMAT UNREALSDK_FIMPLEMENTEDINTERFACE_FORMAT_UE4
#define UNREALSDK_GOBJECTS_FORMAT UNREALSDK_GOBJECTS_FORMAT_FUOBJECTARRAY

#define UNREALSDK_UBOOLPROPERTY_FIELD_MASK_TYPE uint8_t
#define UNREALSDK_UOBJECT_FLAGS_TYPE uint32_t
#define UNREALSDK_UPROPERTY_FLAGS_TYPE uint64_t
#define UNREALSDK_USTRUCT_PROPERTY_SIZE_TYPE int32_t

#define UNREALSDK_DEFAULT_POST_EDIT_CHANGE_CHAIN_PROPERTY_VF_IDX 77
#define UNREALSDK_DEFAULT_POST_EDIT_CHANGE_PROPERTY_VF_IDX 78

// =================================================================================================
#elif UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2
// =================================================================================================

#define UNREALSDK_HAS_FTEXT true
#define UNREALSDK_HAS_OPTIONAL_FUNC_PARAMS false
#define UNREALSDK_USTRUCT_HAS_ALIGNMENT true

#define UNREALSDK_ENUM_FORMAT UNREALSDK_ENUM_FORMAT_UE4
#define UNREALSDK_FIMPLEMENTEDINTERFACE_FORMAT UNREALSDK_FIMPLEMENTEDINTERFACE_FORMAT_UE4
#define UNREALSDK_GOBJECTS_FORMAT UNREALSDK_GOBJECTS_FORMAT_FUOBJECTARRAY

#define UNREALSDK_UBOOLPROPERTY_FIELD_MASK_TYPE uint8_t
#define UNREALSDK_UOBJECT_FLAGS_TYPE uint32_t
#define UNREALSDK_UPROPERTY_FLAGS_TYPE uint64_t
#define UNREALSDK_USTRUCT_PROPERTY_SIZE_TYPE int32_t

#define UNREALSDK_DEFAULT_POST_EDIT_CHANGE_CHAIN_PROPERTY_VF_IDX 77
#define UNREALSDK_DEFAULT_POST_EDIT_CHANGE_PROPERTY_VF_IDX 78

// =================================================================================================
#else
#error Unknown SDK flavour
#endif
// =================================================================================================

// This strictly relies on the gobjects format, so defining here rather than having a copy for each
#define UNREALSDK_HAS_NATIVE_WEAK_POINTERS \
    (UNREALSDK_GOBJECTS_FORMAT != 0        \
     && UNREALSDK_GOBJECTS_FORMAT != UNREALSDK_GOBJECTS_FORMAT_TARRAY)

// This is only somewhat related, but it does depend on architecture which is flavour-specific :)

/**
 * @fn UNREALSDK_UNREAL_STRUCT_PADDING_PUSH
 * @brief Pushes any required pragmas to make struct layouts unreal-compatible.
 */
/**
 * @fn UNREALSDK_UNREAL_STRUCT_PADDING_POP
 * @brief Pops any pragmas set by UNREAKSDK_UNREAL_STRUCT_PADDING_PUSH()
 */
#if defined(_MSC_VER) && UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW
// 32-bit msvc aligns everything to 8 by default
#define UNREALSDK_UNREAL_STRUCT_PADDING_PUSH() _Pragma("pack(push, 0x4)")
#define UNREALSDK_UNREAL_STRUCT_PADDING_POP() _Pragma("pack(pop)")
#else
#define UNREALSDK_UNREAL_STRUCT_PADDING_PUSH()
#define UNREALSDK_UNREAL_STRUCT_PADDING_POP()
#endif

// NOLINTEND(cppcoreguidelines-macro-usage)

#endif /* UNREALSDK_FLAVOUR_H */
