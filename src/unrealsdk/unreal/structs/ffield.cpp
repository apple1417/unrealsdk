#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/structs/ffield.h"
#include "unrealsdk/unreal/offset_list.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/structs/fname.h"

#if UNREALSDK_PROPERTIES_ARE_FFIELD

namespace unrealsdk::unreal {

UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(FFieldClass, UNREALSDK_FFIELDCLASS_FIELDS);
UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(FField, UNREALSDK_FFIELD_FIELDS);

[[nodiscard]] std::wstring FField::get_path_name(void) const {
    return std::format(L"TEMP|{}|{}", this->Owner()->get_path_name(), (std::wstring)this->Name());
}

}  // namespace unrealsdk::unreal

#endif
