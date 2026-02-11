#include "unrealsdk/pch.h"

#include "unrealsdk/game/bl2/bl2.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/alignment.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {

namespace {

#ifdef __MINGW32__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"  // thiscall on non-class
#endif

struct FMalloc;
struct FMallocVFtable {
    void* exec;
    void*(__thiscall* u_malloc)(FMalloc* self, uint32_t len, uint32_t align);
    void*(__thiscall* u_realloc)(FMalloc* self, void* original, uint32_t len, uint32_t align);
    void*(__thiscall* u_free)(FMalloc* self, void* data);
};
struct FMalloc {
    FMallocVFtable* vftable;
};

#ifdef __MINGW32__
#pragma GCC diagnostic pop
#endif

FMalloc* gmalloc;

const constinit Pattern<8> GMALLOC_PATTERN{
    "89 35 {????????}"  // mov [Borderlands2.GDebugger+A95C], esi
    "FF D7"             // call edi
};

}  // namespace

void BL2Hook::find_gmalloc(void) {
    // Using plain `sigscan` since there's an extra level of indirection here, want to make sure to
    // print an error before we potentially dereference it
    gmalloc = *read_offset<FMalloc**>(GMALLOC_PATTERN.sigscan("GMalloc"));
    LOG(MISC, "GMalloc: {:p}", reinterpret_cast<void*>(gmalloc));
}
void* BL2Hook::u_malloc(size_t len) const {
    auto ui32_len = static_cast<uint32_t>(len);
    auto ret = gmalloc->vftable->u_malloc(gmalloc, ui32_len, get_malloc_alignment(ui32_len));
    memset(ret, 0, len);
    return ret;
}
void* BL2Hook::u_realloc(void* original, size_t len) const {
    auto ui32_len = static_cast<uint32_t>(len);
    return gmalloc->vftable->u_realloc(gmalloc, original, ui32_len, get_malloc_alignment(ui32_len));
}
void BL2Hook::u_free(void* data) const {
    gmalloc->vftable->u_free(gmalloc, data);
}

}  // namespace unrealsdk::game

#endif
