#ifndef UNREALSDK_UNREAL_STRUCTS_FIMPLEMENTEDINTERFACE_H
#define UNREALSDK_UNREAL_STRUCTS_FIMPLEMENTEDINTERFACE_H

#include "unrealsdk/pch.h"

namespace unrealsdk::unreal {

#if defined(_MSC_VER) && UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW \
    && UNREALSDK_FLAVOUR != UNREALSDK_FLAVOUR_WILLOW64
#pragma pack(push, 0x4)
#endif

class UStructProperty;
class UClass;

// TODO: This one requires changing pyunrealsdk if we want to use TPointer. So will leave this one
//  as is since it is currently naturally aligned.
struct FImplementedInterface {
    // NOLINTBEGIN(readability-identifier-naming)
    UClass* Class;

   private:
#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK
    int32_t PointerOffset;
    bool isNative;
#elif UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW \
    || UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW64
    UStructProperty* VFTableProperty;  // May be null (if native?)
#else
#error Unknown SDK flavour
#endif

    // NOLINTEND(readability-identifier-naming)

   public:
    /**
     * @brief Gets the offset of the interface pointer on this class.
     *
     * @return The interface pointer offset.
     */
    [[nodiscard]] size_t get_pointer_offset(void) const;
};

#if defined(_MSC_VER) && UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW \
    && UNREALSDK_FLAVOUR != UNREALSDK_FLAVOUR_WILLOW64
#pragma pack(pop)
#endif

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_STRUCTS_FIMPLEMENTEDINTERFACE_H */
