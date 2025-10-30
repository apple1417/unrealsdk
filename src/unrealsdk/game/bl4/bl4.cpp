#include "unrealsdk/pch.h"
#include "unrealsdk/game/bl4/bl4.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2 && !defined(UNREALSDK_IMPORTING)

namespace unrealsdk::game {

void BL4Hook::hook(void) {
    hook_antidebug();
    hook_call_function();
    hook_process_event();

    find_fname_funcs();
    find_gobjects();
    find_gmalloc();
    find_get_path_name();

    find_construct_object();
    find_static_find_object();
}

void BL4Hook::post_init(void) {}

}  // namespace unrealsdk::game


#endif
