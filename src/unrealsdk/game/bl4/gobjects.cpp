#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/wrappers/gobjects.h"
#include "unrealsdk/game/bl4/bl4.h"
#include "unrealsdk/memory.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2 && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {

namespace {

    // 44 8B 05 ? ? ? ? 8B 15 ? ? ? ? 89 F9
const constexpr Pattern<15> GOBJECTS_SIG{
    "44 8B 05 {????????}"  // mov r8d, [1513868E0]
    "8B 15 {????????}"     // mov edx, [1513868E8]
    "89 F9"                // mov ecx, edi

    //"48 8D 05 {????????}"  // lea rax, [1513868E0]
    //"48 89 01"             // mov [rcx], rax
    //"B8 FFFFFFFF"          // mov eax, FFFFFFFF
    //"45 84 C0"             // test r8l, r8l
};

GObjects gobjects_wrapper{};

}  // namespace

void BL4Hook::find_gobjects(void) {
    auto gobjects_ptr = read_offset<GObjects::internal_type>(GOBJECTS_SIG.sigscan_nullable());
    LOG(MISC, "GObjects: {:p}", reinterpret_cast<void*>(gobjects_ptr));

    gobjects_wrapper = GObjects(gobjects_ptr);

    // wait for gobjects to be initialized
    while (gobjects_ptr->ObjObjects.Count == 0) {
        const constexpr auto sleep_time = std::chrono::milliseconds{50};
        std::this_thread::sleep_for(sleep_time);
    }

    LOG(MISC, "GObjObjects at {}", gobjects_ptr->ObjObjects.Count);
}

const unreal::GObjects& BL4Hook::gobjects(void) const {
    return gobjects_wrapper;
}

}  // namespace unrealsdk::game

#endif
