#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/structs/ffield.h"
#include "unrealsdk/unreal/classes/uobject.h"
#include "unrealsdk/unreal/offset_list.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/structs/fname.h"
#include "unrealsdk/unrealsdk.h"

namespace unrealsdk::unreal {

UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(FFieldClass, UNREALSDK_FFIELDCLASS_FIELDS);
UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(FField, UNREALSDK_FFIELD_FIELDS);

[[nodiscard]] bool FFieldClass::inherits(const FFieldClass* base_class) const {
    auto cls = this;
    do {
        if (cls == base_class) {
            return true;
        }
    } while ((cls = cls->SuperField()) != nullptr);
    return false;
}
[[nodiscard]] bool FFieldClass::inherits(const UStruct* base_class) const {
    // HACK: assume the UStructs have the same name as their associated FFieldClass
    auto name = base_class->Name();

    auto cls = this;
    do {
        if (cls->Name() == name) {
            return true;
        }
    } while ((cls = cls->SuperField()) != nullptr);
    return false;
}

bool FField::is_instance(const FFieldClass* cls) const {
    return this->Class()->inherits(cls);
}
bool FField::is_instance(const UClass* cls) const {
    return this->Class()->inherits(cls);
}

[[nodiscard]] std::wstring FField::get_path_name(void) const {
    return unrealsdk::internal::ffield_path_name(this);
}

}  // namespace unrealsdk::unreal
