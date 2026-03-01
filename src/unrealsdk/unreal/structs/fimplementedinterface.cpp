#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/properties/zstructproperty.h"
#include "unrealsdk/unreal/structs/fimplementedinterface.h"

namespace unrealsdk::unreal {

size_t FImplementedInterface::get_pointer_offset() const {
#if UNREALSDK_FIMPLEMENTEDINTERFACE_FORMAT == UNREALSDK_FIMPLEMENTEDINTERFACE_FORMAT_UE4
    return this->isNative ? 0 : this->PointerOffset;
#elif UNREALSDK_FIMPLEMENTEDINTERFACE_FORMAT == UNREALSDK_FIMPLEMENTEDINTERFACE_FORMAT_UE3
    return this->VFTableProperty == nullptr ? 0 : this->VFTableProperty->Offset_Internal();
#else
#error Unknown FImplementedInterface format
#endif
}

}  // namespace unrealsdk::unreal
