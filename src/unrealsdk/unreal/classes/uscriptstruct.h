#ifndef UNREALSDK_UNREAL_CLASSES_USCRIPTSTRUCT_H
#define UNREALSDK_UNREAL_CLASSES_USCRIPTSTRUCT_H

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/ustruct.h"

namespace unrealsdk::unreal {

class UScriptStruct : public UStruct {
   public:
    UScriptStruct() = delete;
    UScriptStruct(const UScriptStruct&) = delete;
    UScriptStruct(UScriptStruct&&) = delete;
    UScriptStruct& operator=(const UScriptStruct&) = delete;
    UScriptStruct& operator=(UScriptStruct&&) = delete;
    ~UScriptStruct() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_USCRIPTSTRUCT_FIELDS(X) X(uint32_t, StructFlags)

    UNREALSDK_DEFINE_FIELDS_HEADER(UScriptStruct, UNREALSDK_USCRIPTSTRUCT_FIELDS);
};

template <>
struct ClassTraits<UScriptStruct> {
    static inline const wchar_t* const NAME = L"ScriptStruct";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_USCRIPTSTRUCT_H */
