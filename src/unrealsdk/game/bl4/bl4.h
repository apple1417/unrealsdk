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
    static void hook_process_event(void);

    static void find_fname_funcs(void);
    static void find_gobjects(void);
    static void find_gmalloc(void);
    static void find_get_path_name(void);

    static void find_construct_object(void);
    static void find_static_find_object(void);
    static void find_load_package(void);
    static void find_fframe_step(void);

    static void inject_console(void);
   public:
    void hook(void) override;
    void post_init(void) override;

    [[nodiscard]] bool is_console_ready(void) const override;

    [[nodiscard]] const unreal::GObjects& gobjects(void) const override;
    [[nodiscard]] void* u_malloc(size_t len) const override;
    [[nodiscard]] void* u_realloc(void* original, size_t len) const override;
    void u_free(void* data) const override;

    [[nodiscard]] unreal::UObject* construct_object(unreal::UClass* cls,
                                                    unreal::UObject* outer,
                                                    const unreal::FName& name,
                                                    uint64_t flags,
                                                    unreal::UObject* template_obj) const override;
    [[nodiscard]] unreal::UObject* find_object(unreal::UClass* cls,
                                               const std::wstring& name) const override;
    [[nodiscard]] unreal::UObject* load_package(const std::wstring& name,
                                                uint32_t flags) const override;
    void fname_init(unreal::FName* name, const wchar_t* str, int32_t number) const override;
    [[nodiscard]] std::variant<const std::string_view, const std::wstring_view> fname_get_str(
        const unreal::FName& name) const override;
    void fframe_step(unreal::FFrame* frame, unreal::UObject* obj, void* param) const override;
    void process_event(unreal::UObject* object,
                       unreal::UFunction* func,
                       void* params) const override;
    void uconsole_output_text(const std::wstring& str) const override;

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
