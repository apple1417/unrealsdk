#ifndef UNREALSDK_GAME_BL4_BL4_H
#define UNREALSDK_GAME_BL4_BL4_H

#include "unrealsdk/pch.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2 && !defined(UNREALSDK_IMPORTING)

#include "unrealsdk/game/selector.h"
#include "unrealsdk/game/throwing_hook.h"

namespace unrealsdk::game {

class BL4Hook : public ThrowingHook {
   protected:
    static void hook_antidebug(void);
    static void hook_call_function(void);

    static void find_fname_funcs(void);
    static void find_gobjects(void);
    static void find_gmalloc(void);
    static void find_get_path_name(void);

   public:
    void hook(void) override;
    void post_init(void) override;

    [[nodiscard]] const unreal::GObjects& gobjects(void) const override;
    [[nodiscard]] void* u_malloc(size_t len) const override;
    [[nodiscard]] void* u_realloc(void* original, size_t len) const override;
    void u_free(void* data) const override;

    void fname_init(unreal::FName* name, const wchar_t* str, int32_t number) const override;
    [[nodiscard]] std::variant<const std::string_view, const std::wstring_view> fname_get_str(
        const unreal::FName& name) const override;

    [[nodiscard]] std::wstring uobject_path_name(const unreal::UObject* obj) const override;

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
