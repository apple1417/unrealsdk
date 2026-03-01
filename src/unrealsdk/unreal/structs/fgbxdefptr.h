#ifndef UNREALSDK_UNREAL_STRUCTS_FGBXDEFPTR_H
#define UNREALSDK_UNREAL_STRUCTS_FGBXDEFPTR_H

#include "unrealsdk/unreal/classes/uscriptstruct.h"
#include "unrealsdk/unreal/structs/fname.h"

namespace unrealsdk::unreal {

class UScriptStruct;

UNREALSDK_UNREAL_STRUCT_PADDING_PUSH()

struct FGbxDefPtr {
    FName name;
    UScriptStruct* ref{};
    void* instance{};
};

UNREALSDK_UNREAL_STRUCT_PADDING_POP()

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_STRUCTS_FGBXDEFPTR_H */
