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

    /**
     * @brief Finds and sets up such that `fname_init` and `fname_get_str` may be called.
     */
    static void find_fname_funcs(void);

   public:
    void hook(void) override;
    void post_init(void) override;

    void fname_init(unreal::FName* name, const wchar_t* str, int32_t number) const override;
    [[nodiscard]] std::variant<const std::string_view, const std::wstring_view> fname_get_str(
        const unreal::FName& name) const override;

    [[nodiscard]] const unreal::offsets::OffsetList& get_offsets(void) const override;
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
