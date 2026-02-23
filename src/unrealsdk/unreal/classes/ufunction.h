#ifndef UNREALSDK_UNREAL_CLASSES_UFUNCTION_H
#define UNREALSDK_UNREAL_CLASSES_UFUNCTION_H

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/ustruct.h"
#include "unrealsdk/unreal/properties/zproperty.h"

namespace unrealsdk::unreal {

class UFunction : public UStruct {
   public:
    static constexpr auto FUNC_NATIVE = 0x400;

    UFunction() = delete;
    UFunction(const UFunction&) = delete;
    UFunction(UFunction&&) = delete;
    UFunction& operator=(const UFunction&) = delete;
    UFunction& operator=(UFunction&&) = delete;
    ~UFunction() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_UFUNCTION_FIELDS(X) \
    X(uint32_t, FunctionFlags)        \
    X(uint8_t, NumParams)             \
    X(uint16_t, ParamsSize)           \
    X(uint16_t, ReturnValueOffset)

    UNREALSDK_DEFINE_FIELDS_HEADER(UFunction, UNREALSDK_UFUNCTION_FIELDS);

    /**
     * @brief Finds the return param for this function (if it exists).
     *
     * @return The return param, or `nullptr` if none exists.
     */
    [[nodiscard]] ZProperty* find_return_param(void) const;
};

template <>
struct ClassTraits<UFunction> {
    static inline const wchar_t* const NAME = L"Function";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_UFUNCTION_H */
