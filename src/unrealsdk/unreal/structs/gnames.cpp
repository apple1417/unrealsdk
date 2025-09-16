#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/offset_list.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/structs/gnames.h"
#include "unrealsdk/unrealsdk.h"

namespace unrealsdk::unreal {

UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(FNameEntry, UNREALSDK_FNAMEENTRY_FIELDS);

FNameEntry* TStaticIndirectArrayThreadSafeRead_FNameEntry::at(size_t idx) const {
    if (std::cmp_greater_equal(idx, this->Count)) {
        throw std::out_of_range("FNameEntry index out of range");
    }
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic,
    //             cppcoreguidelines-pro-bounds-constant-array-index)
    return this->Objects[idx / this->ElementsPerChunk][idx % this->ElementsPerChunk];
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic,
    //           cppcoreguidelines-pro-bounds-constant-array-index)
};

}  // namespace unrealsdk::unreal
