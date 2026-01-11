#ifndef UNREALSDK_UNREAL_STRUCTS_FFIELD_H
#define UNREALSDK_UNREAL_STRUCTS_FFIELD_H

#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/structs/fname.h"
#include "unrealsdk/unreal/structs/tfieldvariant.h"

namespace unrealsdk::unreal {

class UObject;
class UStruct;
struct FField;

UNREALSDK_UNREAL_STRUCT_PADDING_PUSH()
// NOLINTBEGIN(readability-identifier-naming, cppcoreguidelines-pro-type-member-init)

struct FFieldClass {
    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_FFIELDCLASS_FIELDS(X) \
    X(FName, Name)                      \
    X(FFieldClass*, SuperField)         

    UNREALSDK_DEFINE_FIELDS_HEADER(FFieldClass, UNREALSDK_FFIELDCLASS_FIELDS);

    /**
     * @brief Checks if this class inherits from another.
     * @note Also returns true if this class *is* the given class.
     *
     * @param base_struct The base class to check if this inherits from.
     * @return True if this class is the given class, or inherits from it.
     */
    [[nodiscard]] bool inherits(const FFieldClass* base_class) const;
    [[nodiscard]] bool inherits(const UStruct* base_class) const;
};

struct FField {
    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_FFIELD_FIELDS(X) \
    X(FFieldClass*, Class)         \
    X(FFieldVariant, Owner)        \
    X(FField*, Next)               \
    X(FName, Name)

    UNREALSDK_DEFINE_FIELDS_HEADER(FField, UNREALSDK_FFIELD_FIELDS);

    /**
     * @brief Get the associated object's full path name.
     *
     * @return The full path name.
     */
    [[nodiscard]] std::wstring get_path_name(void) const;
};

template <>
struct ClassTraits<FField> {
    static inline const wchar_t* const NAME = L"Field";
};

// NOLINTEND(readability-identifier-naming, cppcoreguidelines-pro-type-member-init)
UNREALSDK_UNREAL_STRUCT_PADDING_POP()

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_STRUCTS_FFIELD_H */
