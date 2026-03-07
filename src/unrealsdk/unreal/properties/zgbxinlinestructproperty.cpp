#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/properties/zgbxinlinestructproperty.h"
#include "unrealsdk/unreal/offset_list.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/structs/fgbxinlinestruct.h"
#include "unrealsdk/unreal/wrappers/wrapped_struct.h"
#include "unrealsdk/unrealsdk.h"

namespace unrealsdk::unreal {

UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(ZGbxInlineStructProperty,
                                    UNREALSDK_ZGBXINLINESTRUCTPROPERTY_FIELDS);

PropTraits<ZGbxInlineStructProperty>::Value PropTraits<ZGbxInlineStructProperty>::get(
    const ZGbxInlineStructProperty* /*prop*/,
    uintptr_t addr,
    const UnrealPointer<void>& /*parent*/) {
    return *reinterpret_cast<FGbxInlineStruct*>(addr);
}

void PropTraits<ZGbxInlineStructProperty>::set(const ZGbxInlineStructProperty* /*prop*/,
                                               uintptr_t addr,
                                               const Value& value) {
    // TODO: do we need to validate struct type here?
    *reinterpret_cast<FGbxInlineStruct*>(addr) = value;
}

void PropTraits<ZGbxInlineStructProperty>::destroy(const ZGbxInlineStructProperty* /*prop*/,
                                                   uintptr_t /*addr*/) {
    // TODO: do we need to free anything here
}

}  // namespace unrealsdk::unreal
