#ifndef UNREALSDK_UNREAL_CLASSES_UFIELD_H
#define UNREALSDK_UNREAL_CLASSES_UFIELD_H

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/uobject.h"

namespace unrealsdk::unreal {

class UField : public UObject {
   public:
    UField() = delete;
    UField(const UField&) = delete;
    UField(UField&&) = delete;
    UField& operator=(const UField&) = delete;
    UField& operator=(UField&&) = delete;
    ~UField() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_UFIELD_FIELDS(X) X(UField*, Next)

    UNREALSDK_DEFINE_FIELDS_HEADER(UField, UNREALSDK_UFIELD_FIELDS);
};

template <>
struct ClassTraits<UField> {
    static inline const wchar_t* const NAME = L"Field";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_UFIELD_H */
