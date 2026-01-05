#ifndef UNREALSDK_UNREAL_STRUCTS_FIMPLEMENTEDINTERFACE_H
#define UNREALSDK_UNREAL_STRUCTS_FIMPLEMENTEDINTERFACE_H

#include "unrealsdk/pch.h"

namespace unrealsdk::unreal {

UNREALSDK_UNREAL_STRUCT_PADDING_PUSH()

class UStructProperty;
class UClass;

struct FImplementedInterface {
    // NOLINTBEGIN(readability-identifier-naming)

    UClass* Class;

   private:
#if UNREALSDK_FIMPLEMENTEDINTERFACE_FORMAT == UNREALSDK_FIMPLEMENTEDINTERFACE_FORMAT_UE4
    int32_t PointerOffset;
    bool isNative;
#elif UNREALSDK_FIMPLEMENTEDINTERFACE_FORMAT == UNREALSDK_FIMPLEMENTEDINTERFACE_FORMAT_UE3
    UStructProperty* VFTableProperty;  // May be null (if native?)
#else
#error Unknown FImplementedInterface format
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

UNREALSDK_UNREAL_STRUCT_PADDING_POP()

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_STRUCTS_FIMPLEMENTEDINTERFACE_H */
