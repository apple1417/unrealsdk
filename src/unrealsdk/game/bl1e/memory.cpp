#include "unrealsdk/pch.h"

#include "unrealsdk/game/bl1e/bl1e.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/alignment.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW64 && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {

namespace {

struct FMalloc;
struct FMallocVFtable {
    void* exec;
    void* (*u_malloc)(FMalloc* self, uint32_t len, uint32_t align);
    void* (*u_realloc)(FMalloc* self, void* original, uint32_t len, uint32_t align);
    void (*u_free)(FMalloc* self, void* data);
};
struct FMalloc {
    FMallocVFtable* vftable;
};

FMalloc** gmalloc_ptr;

// static initialiser for gmalloc
constexpr Pattern<14> SIG_GMALLOC{
    "33 FF"              // XOR  EDI,EDI
    "48893D {????????}"  // MOV  qword ptr [GMalloc_DAT_142519ef0],RDI
    "488B7C 2438"        // MOV  RDI,qword ptr [RSP + local_res10]
};

}  // namespace

void BL1EHook::find_gmalloc() {
    gmalloc_ptr = read_offset<FMalloc**>(SIG_GMALLOC.sigscan_nullable());
    LOG(MISC, "GMalloc*: {:p}", reinterpret_cast<void*>(gmalloc_ptr));

    if (gmalloc_ptr != nullptr) {
        LOG(MISC, "GMalloc: {:p}", reinterpret_cast<void*>(*gmalloc_ptr));
    }
}

void* BL1EHook::u_malloc(size_t len) const {
    if (gmalloc_ptr == nullptr) {
        throw std::runtime_error("tried allocating memory while gmalloc was still null!");
    }
    auto ui32_len = static_cast<uint32_t>(len);
    auto gmalloc = *gmalloc_ptr;
    void* ret = gmalloc->vftable->u_malloc(gmalloc, ui32_len, get_malloc_alignment(ui32_len));
    std::memset(ret, 0, len);
    return ret;
}

void* BL1EHook::u_realloc(void* original, size_t len) const {
    if (gmalloc_ptr == nullptr) {
        throw std::runtime_error("tried allocating memory while gmalloc was still null!");
    }
    auto ui32_len = static_cast<uint32_t>(len);
    auto gmalloc = *gmalloc_ptr;
    return gmalloc->vftable->u_realloc(gmalloc, original, ui32_len, get_malloc_alignment(ui32_len));
}

void BL1EHook::u_free(void* data) const {
    if (gmalloc_ptr == nullptr) {
        throw std::runtime_error("tried allocating memory while gmalloc was still null!");
    }
    auto gmalloc = *gmalloc_ptr;
    gmalloc->vftable->u_free(gmalloc, data);
}

}  // namespace unrealsdk::game

#endif
