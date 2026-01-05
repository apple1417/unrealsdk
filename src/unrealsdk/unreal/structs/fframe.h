#ifndef UNREALSDK_UNREAL_STRUCTS_FFRAME_H
#define UNREALSDK_UNREAL_STRUCTS_FFRAME_H

#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/offsets.h"

namespace unrealsdk::unreal {

class UFunction;
class UObject;
class WrappedStruct;

struct FFrame {
    static constexpr const auto EXPR_TOKEN_END_FUNCTION_PARAMS = 0x16;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_FFRAME_FIELDS(X) \
    X(UFunction*, Node)            \
    X(UObject*, Object)            \
    X(uint8_t*, Code)              \
    X(FFrame*, PreviousFrame)

    UNREALSDK_DEFINE_FIELDS_HEADER(FFrame, UNREALSDK_FFRAME_FIELDS);

    /**
     * @brief Extracts the current function args, assuming stopped during the `CallFunction` hook.
     * @note Steps `Code` to do so, leaving it pointing at the end of function params token.
     *
     * @param args The args struct to write to. Assumes type is already pointing to the function.
     * @return The original position of `Code`.
     */
    uint8_t* extract_current_args(WrappedStruct& args);
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_STRUCTS_FFRAME_H */
