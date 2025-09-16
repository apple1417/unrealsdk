#ifndef UNREALSDK_GAME_BL4_BL4_H
#define UNREALSDK_GAME_BL4_BL4_H

#include "unrealsdk/pch.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2 && !defined(UNREALSDK_IMPORTING)

#include "unrealsdk/game/selector.h"
#include "unrealsdk/game/throwing_hook.h"

namespace unrealsdk::game {

class BL4Hook : public ThrowingHook {
   protected:
    /**
     * @brief Hooks the antidebug functions and disables them.
     */
    static void hook_antidebug(void);

   public:
    void hook(void) override;
    void post_init(void) override;
};

template <>
struct GameTraits<BL4Hook> {
    static constexpr auto NAME = "BL4";

    static bool matches_executable(std::string_view executable) {
        return executable == "Borderlands4.exe";
    }
};

}  // namespace unrealsdk::game

#endif

#endif /* UNREALSDK_GAME_BL4_BL4_H */
