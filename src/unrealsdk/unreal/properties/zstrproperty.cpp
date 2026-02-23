#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/properties/zstrproperty.h"
#include "unrealsdk/unreal/structs/fstring.h"
#include "unrealsdk/unreal/structs/tarray.h"
#include "unrealsdk/unreal/structs/tarray_funcs.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"
#include "unrealsdk/unreal/wrappers/wrapped_struct.h"
#include "unrealsdk/unrealsdk.h"

namespace unrealsdk::unreal {

PropTraits<ZStrProperty>::Value PropTraits<ZStrProperty>::get(
    const ZStrProperty* /*prop*/,
    uintptr_t addr,
    const UnrealPointer<void>& /*parent*/) {
    return *reinterpret_cast<UnmanagedFString*>(addr);
}

void PropTraits<ZStrProperty>::set(const ZStrProperty* /*prop*/,
                                   uintptr_t addr,
                                   const Value& value) {
    *reinterpret_cast<UnmanagedFString*>(addr) = value;
}

void PropTraits<ZStrProperty>::destroy(const ZStrProperty* /*prop*/, uintptr_t addr) {
    reinterpret_cast<UnmanagedFString*>(addr)->free();
}

}  // namespace unrealsdk::unreal
