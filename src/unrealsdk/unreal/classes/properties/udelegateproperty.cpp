#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/classes/properties/udelegateproperty.h"
#include "unrealsdk/unreal/offset_list.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/structs/fscriptdelegate.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"
#include "unrealsdk/unrealsdk.h"

namespace unrealsdk::unreal {

UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(ZDelegateProperty, UNREALSDK_ZDELEGATEPROPERTY_FIELDS);

PropTraits<ZDelegateProperty>::Value PropTraits<ZDelegateProperty>::get(
    const ZDelegateProperty* /*prop*/,
    uintptr_t addr,
    const UnrealPointer<void>& /*parent*/) {
    return reinterpret_cast<FScriptDelegate*>(addr)->as_function();
}

void PropTraits<ZDelegateProperty>::set(const ZDelegateProperty* prop,
                                        uintptr_t addr,
                                        const Value& value) {
    FScriptDelegate::validate_signature(value, prop->Signature());
    reinterpret_cast<FScriptDelegate*>(addr)->bind(value);
}

}  // namespace unrealsdk::unreal
