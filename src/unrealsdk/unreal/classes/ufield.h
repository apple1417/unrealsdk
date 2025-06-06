#ifndef UNREALSDK_UNREAL_CLASSES_UFIELD_H
#define UNREALSDK_UNREAL_CLASSES_UFIELD_H

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/uobject.h"

namespace unrealsdk::unreal {

#if defined(_MSC_VER) && UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW
#pragma pack(push, 0x4)
#endif

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#endif

namespace offsets::generic {

template <typename T>
class UField : public T {
   public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    UField* Next;
};

}  // namespace offsets::generic

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

#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#if defined(_MSC_VER) && UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW
#pragma pack(pop)
#endif

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_UFIELD_H */
