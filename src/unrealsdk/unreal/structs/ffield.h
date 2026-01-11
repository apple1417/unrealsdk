#ifndef UNREALSDK_UNREAL_STRUCTS_FFIELD_H
#define UNREALSDK_UNREAL_STRUCTS_FFIELD_H

#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/structs/fname.h"

namespace unrealsdk::unreal {

class UObject;
struct FField;

UNREALSDK_UNREAL_STRUCT_PADDING_PUSH()
// NOLINTBEGIN(readability-identifier-naming, cppcoreguidelines-pro-type-member-init)

struct FFieldClass {
    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_FFIELDCLASS_FIELDS(X) \
    X(FName, Name)                      \
    X(FFieldClass*, SuperField)         \
    X(FField*, DefaultObject)

    UNREALSDK_DEFINE_FIELDS_HEADER(FFieldClass, UNREALSDK_FFIELDCLASS_FIELDS);

    // TODO: we need an inherits method, that ideally somehow takes a UClass
    //       want `obj->Class->inherits(find_class<T>())` to work
};

struct FField {
    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_FFIELD_FIELDS(X) \
    X(FFieldClass*, Class)         \
    X(UObject*, Owner)             \
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

// NOLINTEND(readability-identifier-naming, cppcoreguidelines-pro-type-member-init)
UNREALSDK_UNREAL_STRUCT_PADDING_POP()

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_STRUCTS_FFIELD_H */
