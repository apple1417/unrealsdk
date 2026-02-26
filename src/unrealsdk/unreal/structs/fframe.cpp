#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/offset_list.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/structs/fframe.h"
#include "unrealsdk/unreal/wrappers/wrapped_struct.h"
#include "unrealsdk/unrealsdk.h"

namespace unrealsdk::unreal {

UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(FFrame, UNREALSDK_UOBJECT_FIELDS);

uint8_t* FFrame::extract_current_args(WrappedStruct& args) {
    auto args_addr = reinterpret_cast<uintptr_t>(args.base.get());
    // NOLINTNEXTLINE(misc-const-correctness) - see llvm/llvm-project#157320
    uint8_t* original_code = this->Code();

#if UNREALSDK_PROPERTIES_ARE_FFIELD
    auto first_prop = args.type->ChildProperties();
#else
    auto first_prop = args.type->Children();
#endif

    if (first_prop == nullptr) {
        LOG(ERROR, "Tried to extract frame from a function with no properties!");
        return original_code;
    }

    for (auto prop = reinterpret_cast<ZProperty*>(first_prop);
         *this->Code() != FFrame::EXPR_TOKEN_END_FUNCTION_PARAMS;
         prop = reinterpret_cast<ZProperty*>(prop->Next())) {
        if ((prop->PropertyFlags() & ZProperty::PROP_FLAG_RETURN) != 0) {
            continue;
        }

        unrealsdk::internal::fframe_step(
            this, this->Object(), reinterpret_cast<void*>(args_addr + prop->Offset_Internal()));
    }

    return original_code;
}

}  // namespace unrealsdk::unreal
