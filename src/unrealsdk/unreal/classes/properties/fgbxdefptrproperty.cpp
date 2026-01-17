#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/offset_list.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/classes/properties/fgbxdefptrproperty.h"
#include "unrealsdk/unreal/wrappers/wrapped_struct.h"
#include "unrealsdk/unrealsdk.h"

namespace unrealsdk::unreal {

UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(FGbxDefPtrProperty, UNREALSDK_FGBXDEFPTRPROPERTY_FIELDS);

PropTraits<FGbxDefPtrProperty>::Value PropTraits<FGbxDefPtrProperty>::get(
    const FGbxDefPtrProperty* /*prop*/,
    uintptr_t addr,
    const UnrealPointer<void>& /*parent*/) {
    auto def = reinterpret_cast<FGbxDefPtr*>(addr);
    return *def;
}

void PropTraits<FGbxDefPtrProperty>::set(const FGbxDefPtrProperty* /*prop*/,
                                         uintptr_t addr,
                                         const Value& value) {
    auto def = reinterpret_cast<FGbxDefPtr*>(addr);
    *def = value;
}


}  // namespace unrealsdk::unreal
