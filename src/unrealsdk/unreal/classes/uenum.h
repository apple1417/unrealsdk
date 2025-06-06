#ifndef UNREALSDK_UNREAL_CLASSES_UENUM_H
#define UNREALSDK_UNREAL_CLASSES_UENUM_H

#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/ufield.h"
#include "unrealsdk/unreal/structs/fname.h"
#include "unrealsdk/unreal/structs/fstring.h"
#include "unrealsdk/unreal/structs/tarray.h"
#include "unrealsdk/unreal/structs/tpair.h"

namespace unrealsdk::unreal {

#if defined(_MSC_VER) && UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW
#pragma pack(push, 0x4)
#endif

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#endif

class UEnum : public UField {
   public:
    UEnum() = delete;
    UEnum(const UEnum&) = delete;
    UEnum(UEnum&&) = delete;
    UEnum& operator=(const UEnum&) = delete;
    UEnum& operator=(UEnum&&) = delete;
    ~UEnum() = delete;

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW
    using names_type = TArray<FName>;
#elif UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK
    using names_type = TArray<TPair<FName, uint64_t> >;
#else
#error Unknown sdk flavour
#endif

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_UENUM_FIELDS(X) X(names_type, Names)

    UNREALSDK_DEFINE_FIELDS_HEADER(UEnum, UNREALSDK_UENUM_FIELDS);

    /**
     * @brief Converts the enum names into a more usable name to value map.
     * @note Keys are always the raw enum names, rather than `MyEnum::Entry` it's always `Entry`.
     *
     * @return A map of names to their associated integer values.
     */
    [[nodiscard]] std::unordered_map<FName, uint64_t> get_names(void) const;
};

template <>
struct ClassTraits<UEnum> {
    static inline const wchar_t* const NAME = L"Enum";
};

#if defined(__clang__) || defined(__MINGW32__)
#pragma GCC diagnostic pop
#endif

#if defined(_MSC_VER) && UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW
#pragma pack(pop)
#endif

}  // namespace unrealsdk::unreal

#endif /* PYUNREALSDK_LIBS_UNREALSDK_SRC_UNREALSDK_UNREAL_CLASSES_UENUM_H */
