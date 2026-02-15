
#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/structs/fframe.h"
#include "unrealsdk/unreal/wrappers/wrapped_struct.h"
#include "unrealsdk/unrealsdk.h"

namespace unrealsdk::unreal {

uint8_t* FFrame::extract_current_args(WrappedStruct& args) {
    auto args_addr = reinterpret_cast<uintptr_t>(args.base.get());
    // NOLINTNEXTLINE(misc-const-correctness) - see llvm/llvm-project#157320
    uint8_t* original_code = this->Code.get();

    for (auto prop = reinterpret_cast<UProperty*>(args.type->Children());
         this->Code.operator*() != FFrame::EXPR_TOKEN_END_FUNCTION_PARAMS;
         prop = reinterpret_cast<UProperty*>(prop->Next())) {
        if ((prop->PropertyFlags() & UProperty::PROP_FLAG_RETURN) != 0) {
            continue;
        }

        unrealsdk::internal::fframe_step(
            this, this->Object.get(), reinterpret_cast<void*>(args_addr + prop->Offset_Internal()));
    }

    return original_code;
}

}  // namespace unrealsdk::unreal
