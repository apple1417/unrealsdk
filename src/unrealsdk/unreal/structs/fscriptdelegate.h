#ifndef UNREALSDK_UNREAL_STRUCTS_FSCRIPTDELEGATE_H
#define UNREALSDK_UNREAL_STRUCTS_FSCRIPTDELEGATE_H

#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/structs/fname.h"
#include "unrealsdk/unreal/structs/fweakobjectptr.h"
#include "unrealsdk/unreal/wrappers/bound_function.h"

namespace unrealsdk::unreal {

UNREALSDK_UNREAL_STRUCT_PADDING_PUSH()

class UObject;
class UFunction;

struct FScriptDelegate {
   private:
#if UNREALSDK_HAS_NATIVE_WEAK_POINTERS
    FWeakObjectPtr object;
#else
    UObject* object = nullptr;
#endif

   public:
    FName func_name;

    /**
     * @brief Get the object this delegate is bound to.
     *
     * @return The object.
     */
    [[nodiscard]] UObject* get_object(void) const;

    /**
     * @brief Sets the object this delegate is bound to.
     *
     * @param object The object.
     */
    void set_object(UObject* obj);

    /**
     * @brief Tries to convert this delegate to a bound function.
     *
     * @return The bound function, or std::nullopt if this delegate is not bound to anything.
     */
    [[nodiscard]] std::optional<BoundFunction> as_function(void) const;

    /**
     * @brief Binds this delegate to the given bound function.
     * @note Does not validate the function, should call `validate_signature` beforehand.
     *
     * @param func The function to bind.
     */
    void bind(const std::optional<BoundFunction>& func);

    /**
     * @brief Checks if a particular bound function is matches a delegate's signature function.
     * @note Throws a `std::invalid_argument` if it doesn't match.
     *
     * @param func The function to validate.
     * @param signature The delegate's signature function.
     */
    static void validate_signature(const std::optional<BoundFunction>& func,
                                   const UFunction* signature);
};

UNREALSDK_UNREAL_STRUCT_PADDING_POP()

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_STRUCTS_FSCRIPTDELEGATE_H */
