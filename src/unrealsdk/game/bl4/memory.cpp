#include "unrealsdk/pch.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/game/bl4/bl4.h"
#include "unrealsdk/unreal/alignment.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2 && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {

namespace {

const constexpr Pattern<28> GMALLOC_SIG{
    "48 8B 0D {????????}"  // mov rcx, [15128CDA0]
    "48 8B 01"             // mov rax, [rcx]
    "FF 50 ??"             // call qword ptr [rax+48]
    "83 7E ?? 00"          // cmp dword ptr [rsi+10], 00
    "0F85 ????????"        // jne 140E73497
    "48 8D 5C 24 ??"       // lea rbx, [rsp+30]
};

struct FMalloc;
struct FMallocVFtable {
    uintptr_t unknown00;
    uintptr_t unknown08;
    uintptr_t unknown10;
    uintptr_t unknown18;
    uintptr_t unknown20;
    void* (*u_malloc)(FMalloc* self, uint32_t len, uint32_t align);
    uintptr_t unknown30;
    void* (*u_realloc)(FMalloc* self, void* original, uint32_t len, uint32_t align);
    uintptr_t unknown40;
    void* (*u_free)(FMalloc* self, void* data);
    uintptr_t unknown50;
    uintptr_t unknown58;
    uintptr_t possibly_get_allocation_size;
};
struct FMalloc {
    FMallocVFtable* vftable;
};

FMalloc* gmalloc;

}  // namespace

void BL4Hook::find_gmalloc(void) {
    volatile auto gmalloc_ptr = read_offset<FMalloc**>(GMALLOC_SIG.sigscan("GMalloc"));
    while (*gmalloc_ptr == nullptr) {
        // NOLINTNEXTLINE(readability-magic-numbers)
        std::this_thread::sleep_for(std::chrono::milliseconds{50});
    }

    gmalloc = *gmalloc_ptr;
    LOG(MISC, "GMalloc: {:p}", reinterpret_cast<void*>(gmalloc));
}

void* BL4Hook::u_malloc(size_t len) const {
    auto ret = gmalloc->vftable->u_malloc(gmalloc, len, get_malloc_alignment(len));
    memset(ret, 0, len);
    return ret;
}
void* BL4Hook::u_realloc(void* original, size_t len) const {
    return gmalloc->vftable->u_realloc(gmalloc, original, len, get_malloc_alignment(len));
}
void BL4Hook::u_free(void* data) const {
    gmalloc->vftable->u_free(gmalloc, data);
}

}  // namespace unrealsdk::game

#endif
