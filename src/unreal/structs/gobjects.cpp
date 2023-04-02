#include "pch.h"

#include "unreal/structs/gobjects.h"

namespace unrealsdk::unreal {

#ifdef UE4

FUObjectItem* FChunkedFixedUObjectArray::at(size_t idx) const {
    if (idx > (size_t)this->Count) {
        throw std::out_of_range("FChunkedFixedUObjectArray index out of range");
    }
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return &this->Objects[idx / this->NumElementsPerChunk][idx % this->NumElementsPerChunk];
};

#endif

}  // namespace unrealsdk::unreal
