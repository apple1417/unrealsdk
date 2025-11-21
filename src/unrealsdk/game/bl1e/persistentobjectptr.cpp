#include "unrealsdk/pch.h"

#include "unrealsdk/game/bl1e/bl1e.h"
#include "unrealsdk/version_error.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW && !defined(UNREALSDK_IMPORTING)

namespace unrealsdk::game {

void BL1EHook::fsoftobjectptr_assign(unreal::FSoftObjectPtr* /* ptr */,
                                     const unreal::UObject* /* obj */) const {
    throw_version_error("Soft object pointers are not implemented in UE3");
}

void BL1EHook::flazyobjectptr_assign(unreal::FLazyObjectPtr* /* ptr */,
                                     const unreal::UObject* /* obj */) const {
    throw_version_error("Lazy object pointers are not implemented in UE3");
}

}  // namespace unrealsdk::game

#endif
