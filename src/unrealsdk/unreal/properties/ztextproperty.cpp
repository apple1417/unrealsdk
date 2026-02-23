#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/properties/ztextproperty.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/structs/fstring.h"
#include "unrealsdk/unreal/structs/ftext.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"
#include "unrealsdk/unrealsdk.h"

namespace unrealsdk::unreal {

PropTraits<ZTextProperty>::Value PropTraits<ZTextProperty>::get(
    const ZTextProperty* /*prop*/,
    uintptr_t addr,
    const UnrealPointer<void>& /*parent*/) {
    return *reinterpret_cast<FText*>(addr);
}

void PropTraits<ZTextProperty>::set(const ZTextProperty* /*prop*/,
                                    uintptr_t addr,
                                    const Value& value) {
    *reinterpret_cast<FText*>(addr) = value;
}

void PropTraits<ZTextProperty>::destroy(const ZTextProperty* /*prop*/, uintptr_t addr) {
    // Only call the destructor, don't delete, since we probably don't own this memory
    // If we do, the higher levels will deal with it
    reinterpret_cast<FText*>(addr)->~FText();
}

}  // namespace unrealsdk::unreal
