#ifndef UNREALSDK_GAME_THROWING_HOOK_H
#define UNREALSDK_GAME_THROWING_HOOK_H

#include "unrealsdk/pch.h"

#include "unrealsdk/game/abstract_hook.h"

#ifndef UNREALSDK_IMPORTING

namespace unrealsdk::game {

/**
 * @brief Dummy hook which makes every virtual function throw.
 * @note Intended for use as a base class while porting to a new game.
 * @note Should be removed by the time it gets to a public release.
 */
class ThrowingHook : public AbstractHook {
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
    void ftext_as_culture_invariant(unreal::FText* text,
                                    unreal::TemporaryFString&& str) const override;
    void fsoftobjectptr_assign(unreal::FSoftObjectPtr* ptr,
                               const unreal::UObject* obj) const override;
    void flazyobjectptr_assign(unreal::FLazyObjectPtr* ptr,
                               const unreal::UObject* obj) const override;
    [[nodiscard]] const unreal::offsets::OffsetList& get_offsets(void) const override;
};

}  // namespace unrealsdk::game

#endif

#endif /* UNREALSDK_GAME_THROWING_HOOK_H */
