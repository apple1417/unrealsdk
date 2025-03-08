#ifndef UNREALSDK_UNREAL_CLASSES_UCONST_H
#define UNREALSDK_UNREAL_CLASSES_UCONST_H

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/ufield.h"
#include "unrealsdk/unreal/structs/fstring.h"

namespace unrealsdk::unreal {

namespace offsets::generic {

template <typename T>
class UConst : public T {
   public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    UnmanagedFString Value;
};

}  // namespace offsets::generic

class UConst : public UField {
   public:
    UConst() = delete;
    UConst(const UConst&) = delete;
    UConst(UConst&&) = delete;
    UConst& operator=(const UConst&) = delete;
    UConst& operator=(UConst&&) = delete;
    ~UConst() = delete;

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_UCONST_FIELDS(X) X(UnmanagedFString, Value, , )

    // NOLINTNEXTLINE(readability-identifier-naming)
    UNREALSDK_DEFINE_FIELDS_HEADER(UConst, UNREALSDK_UCONST_FIELDS);

   private:
    // NOLINTNEXTLINE(readability-identifier-naming)
    UnmanagedFString Value_internal;
};

template <>
struct ClassTraits<UConst> {
    static inline const wchar_t* const NAME = L"Const";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_UCONST_H */
