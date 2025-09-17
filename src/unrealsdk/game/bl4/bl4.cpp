#include "unrealsdk/pch.h"
#include "unrealsdk/game/bl4/bl4.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2 && !defined(UNREALSDK_IMPORTING)

namespace unrealsdk::game {

void BL4Hook::hook(void) {
    hook_antidebug();

    find_fname_funcs();
    find_gobjects();
}

void BL4Hook::post_init(void) {}

}  // namespace unrealsdk::game

#endif
