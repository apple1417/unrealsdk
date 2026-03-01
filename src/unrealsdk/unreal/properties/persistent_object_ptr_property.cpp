#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/classes/uclass.h"
#include "unrealsdk/unreal/offset_list.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/properties/persistent_object_ptr_property.h"
#include "unrealsdk/unreal/structs/fstring.h"
#include "unrealsdk/unreal/structs/tarray.h"
#include "unrealsdk/unreal/structs/tarray_funcs.h"
#include "unrealsdk/unreal/structs/tpersistentobjectptr.h"
#include "unrealsdk/unreal/wrappers/gobjects.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"
#include "unrealsdk/unrealsdk.h"

namespace unrealsdk::unreal {

UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(ZSoftClassProperty, UNREALSDK_ZSOFTCLASSPROPERTY_FIELDS);

PropTraits<ZLazyObjectProperty>::Value PropTraits<ZLazyObjectProperty>::get(
    const ZLazyObjectProperty* /*prop*/,
    uintptr_t addr,
    const UnrealPointer<void>& /*parent*/) {
    auto ptr = reinterpret_cast<FLazyObjectPtr*>(addr);
    return unrealsdk::gobjects().get_weak_object(&ptr->weak_ptr);
}
PropTraits<ZSoftObjectProperty>::Value PropTraits<ZSoftObjectProperty>::get(
    const ZSoftObjectProperty* /*prop*/,
    uintptr_t addr,
    const UnrealPointer<void>& /*parent*/) {
    auto ptr = reinterpret_cast<FSoftObjectPtr*>(addr);
    return unrealsdk::gobjects().get_weak_object(&ptr->weak_ptr);
}
PropTraits<ZSoftClassProperty>::Value PropTraits<ZSoftClassProperty>::get(
    const ZSoftClassProperty* /*prop*/,
    uintptr_t addr,
    const UnrealPointer<void>& /*parent*/) {
    // Soft classes still use a soft object pointer struct
    auto ptr = reinterpret_cast<FSoftObjectPtr*>(addr);
    return reinterpret_cast<UClass*>(unrealsdk::gobjects().get_weak_object(&ptr->weak_ptr));
}

void PropTraits<ZLazyObjectProperty>::set(const ZLazyObjectProperty* prop,
                                          uintptr_t addr,
                                          const Value& value) {
    // Ensure the object is of a valid class
    if (value != nullptr) {
        auto prop_cls = prop->PropertyClass();
        if (!value->is_instance(prop_cls)) {
            throw std::runtime_error("Object is not instance of " + prop_cls->Name());
        }
    }

    unrealsdk::internal::flazyobjectptr_assign(reinterpret_cast<FLazyObjectPtr*>(addr), value);
}
void PropTraits<ZSoftObjectProperty>::set(const ZSoftObjectProperty* prop,
                                          uintptr_t addr,
                                          const Value& value) {
    // Ensure the object is of a valid class
    if (value != nullptr) {
        auto prop_cls = prop->PropertyClass();
        if (!value->is_instance(prop_cls)) {
            throw std::runtime_error("Object is not instance of " + prop_cls->Name());
        }
    }

    unrealsdk::internal::fsoftobjectptr_assign(reinterpret_cast<FSoftObjectPtr*>(addr), value);
}
void PropTraits<ZSoftClassProperty>::set(const ZSoftClassProperty* prop,
                                         uintptr_t addr,
                                         const Value& value) {
    // Ensure the object is of a valid class
    if (value != nullptr) {
        auto prop_cls = prop->PropertyClass();
        if (!value->is_instance(prop_cls)) {
            throw std::runtime_error("Object is not instance of " + prop_cls->Name());
        }
        auto meta_cls = prop->MetaClass();
        if (!value->inherits(meta_cls)) {
            throw std::runtime_error("Class does not inherit from " + meta_cls->Name());
        }
    }

    unrealsdk::internal::fsoftobjectptr_assign(reinterpret_cast<FSoftObjectPtr*>(addr), value);
}

void PropTraits<ZSoftObjectProperty>::destroy(const ZSoftObjectProperty* /*prop*/, uintptr_t addr) {
    reinterpret_cast<FSoftObjectPtr*>(addr)->identifier.subpath.free();
}
void PropTraits<ZSoftClassProperty>::destroy(const ZSoftClassProperty* /*prop*/, uintptr_t addr) {
    reinterpret_cast<FSoftObjectPtr*>(addr)->identifier.subpath.free();
}

}  // namespace unrealsdk::unreal
