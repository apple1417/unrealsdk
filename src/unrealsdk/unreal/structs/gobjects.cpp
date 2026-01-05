#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/structs/gobjects.h"

namespace unrealsdk::unreal {

#if UNREALSDK_GOBJECTS_FORMAT == UNREALSDK_GOBJECTS_FORMAT_FUOBJECTARRAY

FUObjectItem* FChunkedFixedUObjectArray::at(size_t idx) const {
    if (std::cmp_greater_equal(idx, this->Count)) {
        throw std::out_of_range("FChunkedFixedUObjectArray index out of range");
    }
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return &this->Objects[idx / FChunkedFixedUObjectArray::NumElementsPerChunk]
                         [idx % FChunkedFixedUObjectArray::NumElementsPerChunk];
};

#endif

}  // namespace unrealsdk::unreal
