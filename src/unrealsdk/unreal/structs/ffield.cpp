#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/structs/ffield.h"
#include "unrealsdk/unreal/classes/uobject.h"
#include "unrealsdk/unreal/offset_list.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/structs/fname.h"
#include "unrealsdk/unreal/structs/tfieldvariant.h"

namespace unrealsdk::unreal {

UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(FFieldClass, UNREALSDK_FFIELDCLASS_FIELDS);
UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(FField, UNREALSDK_FFIELD_FIELDS);

[[nodiscard]] std::wstring FField::get_path_name(void) const {
    std::wstring owner_name{};
    this->Owner().cast([&owner_name]<typename T>(T* obj) {
        if constexpr (!std::is_null_pointer_v<T>) {
            owner_name = obj->get_path_name();
        }
    });
    return std::format(L"TEMP|{}|{}", owner_name, (std::wstring)this->Name());
}

}  // namespace unrealsdk::unreal
