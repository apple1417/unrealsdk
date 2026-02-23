#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/classes/properties/fgbxdefptrproperty.h"
#include "unrealsdk/unreal/offset_list.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/wrappers/wrapped_struct.h"
#include "unrealsdk/unrealsdk.h"

namespace unrealsdk::unreal {

UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(ZGbxDefPtrProperty, UNREALSDK_ZGBXDEFPTRPROPERTY_FIELDS);

PropTraits<ZGbxDefPtrProperty>::Value PropTraits<ZGbxDefPtrProperty>::get(
    const ZGbxDefPtrProperty* /*prop*/,
    uintptr_t addr,
    const UnrealPointer<void>& /*parent*/) {
    return *reinterpret_cast<FGbxDefPtr*>(addr);
}

void PropTraits<ZGbxDefPtrProperty>::set(const ZGbxDefPtrProperty* /*prop*/,
                                         uintptr_t addr,
                                         const Value& value) {
    // TODO: do we need to validate struct type here?
    *reinterpret_cast<FGbxDefPtr*>(addr) = value;
}

void PropTraits<ZGbxDefPtrProperty>::destroy(const ZGbxDefPtrProperty* /*prop*/,
                                             uintptr_t /*addr*/) {
    // TODO: do we need to free anything here
}

}  // namespace unrealsdk::unreal
