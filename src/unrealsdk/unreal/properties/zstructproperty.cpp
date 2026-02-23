#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/classes/uscriptstruct.h"
#include "unrealsdk/unreal/offset_list.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/properties/zstructproperty.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"
#include "unrealsdk/unreal/wrappers/wrapped_struct.h"
#include "unrealsdk/unrealsdk.h"

namespace unrealsdk::unreal {

UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(ZStructProperty, UNREALSDK_ZSTRUCTPROPERTY_FIELDS);

PropTraits<ZStructProperty>::Value PropTraits<ZStructProperty>::get(
    const ZStructProperty* prop,
    uintptr_t addr,
    const UnrealPointer<void>& parent) {
    auto this_struct = prop->Struct();
    return {this_struct, reinterpret_cast<void*>(addr), parent};
}

void PropTraits<ZStructProperty>::set(const ZStructProperty* prop,
                                      uintptr_t addr,
                                      const Value& value) {
    auto this_struct = prop->Struct();
    if (value.type != this_struct) {
        throw std::runtime_error("Struct is not an instance of "
                                 + (std::string)this_struct->Name());
    }
    copy_struct(addr, value);
}

void PropTraits<ZStructProperty>::destroy(const ZStructProperty* prop, uintptr_t addr) {
    destroy_struct(prop->Struct(), addr);
}

}  // namespace unrealsdk::unreal
