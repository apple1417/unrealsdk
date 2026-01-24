#include "unrealsdk/pch.h"
#include "unrealsdk/game/throwing_hook.h"
#include "unrealsdk/unreal/classes/uclass.h"
#include "unrealsdk/unreal/structs/fframe.h"
#include "unrealsdk/unreal/structs/ftext.h"
#include "unrealsdk/unreal/wrappers/gobjects.h"
#include "unrealsdk/version_error.h"

#ifndef UNREALSDK_IMPORTING

namespace unrealsdk::game {

void ThrowingHook::hook(void) {
    throw_version_error("hook not implemented");
    unreachable();
}
void ThrowingHook::post_init(void) {
    throw_version_error("post_init not implemented");
    unreachable();
}
bool ThrowingHook::is_console_ready(void) const {
    throw_version_error("is_console_ready not implemented");
    unreachable();
}
const unreal::GObjects& ThrowingHook::gobjects(void) const {
    throw_version_error("gobjects not implemented");
    unreachable();
}
void* ThrowingHook::u_malloc(size_t /*len*/) const {
    throw_version_error("u_malloc not implemented");
    unreachable();
}
void* ThrowingHook::u_realloc(void* /*original*/, size_t /*len*/) const {
    throw_version_error("u_realloc not implemented");
    unreachable();
}
void ThrowingHook::u_free(void* /*data*/) const {
    throw_version_error("u_free not implemented");
    unreachable();
}
unreal::UObject* ThrowingHook::construct_object(unreal::UClass* /*cls*/,
                                                unreal::UObject* /*outer*/,
                                                const unreal::FName& /*name*/,
                                                uint64_t /*flags*/,
                                                unreal::UObject* /*template_obj*/) const {
    throw_version_error("construct_object not implemented");
    unreachable();
}
unreal::UObject* ThrowingHook::find_object(unreal::UClass* /*cls*/,
                                           const std::wstring& /*name*/) const {
    throw_version_error("find_object not implemented");
    unreachable();
}
unreal::UObject* ThrowingHook::load_package(const std::wstring& /*name*/,
                                            uint32_t /*flags*/) const {
    throw_version_error("load_package not implemented");
    unreachable();
}
void ThrowingHook::fname_init(unreal::FName* /*name*/,
                              const wchar_t* /*str*/,
                              int32_t /*number*/) const {
    throw_version_error("fname_init not implemented");
    unreachable();
}
std::variant<const std::string_view, const std::wstring_view> ThrowingHook::fname_get_str(
    const unreal::FName& /*name*/) const {
    throw_version_error("fname_get_str not implemented");
    unreachable();
};
void ThrowingHook::fframe_step(unreal::FFrame* /*frame*/,
                               unreal::UObject* /*obj*/,
                               void* /*param*/) const {
    throw_version_error("fframe_step not implemented");
    unreachable();
}
void ThrowingHook::process_event(unreal::UObject* /*object*/,
                                 unreal::UFunction* /*func*/,
                                 void* /*params*/) const {
    throw_version_error("process_event not implemented");
    unreachable();
}
void ThrowingHook::uconsole_output_text(const std::wstring& /*str*/) const {
    // Ok breaking the contract a little here: this one just swallows calls without throwing
    // Otherwise every single log message throws
}
std::wstring ThrowingHook::uobject_path_name(const unreal::UObject* /*obj*/) const {
    throw_version_error("uobject_path_name not implemented");
    unreachable();
}
std::wstring ThrowingHook::ffield_path_name(const unreal::FField* /*field*/) const {
    throw_version_error("ffield_path_name not implemented");
    unreachable();
}
void ThrowingHook::ftext_as_culture_invariant(unreal::FText* /*text*/,
                                              unreal::TemporaryFString&& /*str*/) const {
    throw_version_error("ftext_as_culture_invariant not implemented");
    unreachable();
}
void ThrowingHook::fsoftobjectptr_assign(unreal::FSoftObjectPtr* /*ptr*/,
                                         const unreal::UObject* /*obj*/) const {
    throw_version_error("fsoftobjectptr_assign not implemented");
    unreachable();
}
void ThrowingHook::flazyobjectptr_assign(unreal::FLazyObjectPtr* /*ptr*/,
                                         const unreal::UObject* /*obj*/) const {
    throw_version_error("flazyobjectptr_assign not implemented");
    unreachable();
}
const unreal::offsets::OffsetList& ThrowingHook::get_offsets(void) const {
    throw_version_error("get_offsets not implemented");
    unreachable();
}

}  // namespace unrealsdk::game

#endif
