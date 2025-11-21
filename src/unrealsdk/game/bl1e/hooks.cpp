#include "unrealsdk/pch.h"

#include "unrealsdk/game/bl1e/bl1e.h"

#include "unrealsdk/hook_manager.h"
#include "unrealsdk/locks.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/classes/ufunction.h"
#include "unrealsdk/unreal/structs/fframe.h"
#include "unrealsdk/unreal/wrappers/wrapped_struct.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {

namespace {

// NOLINTNEXTLINE(modernize-use-using)
using process_event_func = void (*)(UObject* obj, UFunction* func, void* params, void* /*null*/);
process_event_func process_event_ptr;

// TODO: Refactor this
constexpr Pattern<68> PROCESS_EVENT_SIG{
    "4055415641574881EC90000000488D6C242048C74560FEFFFFFF48899D900000004889B5980000004889BDA0000000"
    "4C89A5A8000000488B05130A3A024833C548894568"};

void process_event_hook(UObject* obj, UFunction* func, void* params, void* null) {
    try {
        // This arg seems to be in the process of being deprecated, no usage in ghidra, always seems
        // to be null, and it's gone in later ue versions. Gathering some extra info just in case.
        if (null != nullptr) {
            LOG(DEV_WARNING, L"Null param had a value in process event during func {} on obj {}",
                func->get_path_name(), obj->get_path_name());
        }

        auto data = hook_manager::impl::preprocess_hook(L"ProcessEvent", func, obj);
        if (data != nullptr) {
            // Copy args so that hooks can't modify them, for parity with call function
            const WrappedStruct args_base{func, params};
            WrappedStruct args = args_base.copy_params_only();
            hook_manager::Details hook{.obj = obj,
                                       .args = &args,
                                       .ret = {func->find_return_param()},
                                       .func = {.func = func, .object = obj}};

            const bool block_execution = run_hooks_of_type(data, hook_manager::Type::PRE, hook);

            if (!block_execution) {
                process_event_ptr(obj, func, params, null);
            }

            if (hook.ret.has_value()) {
                hook.ret.copy_to(reinterpret_cast<uintptr_t>(params));
            }

            if (!has_post_hooks(data)) {
                return;
            }

            if (hook.ret.prop != nullptr && !hook.ret.has_value() && !block_execution) {
                hook.ret.copy_from(reinterpret_cast<uintptr_t>(params));
            }

            if (!block_execution) {
                run_hooks_of_type(data, hook_manager::Type::POST, hook);
            }

            run_hooks_of_type(data, hook_manager::Type::POST_UNCONDITIONAL, hook);

            return;
        }
    } catch (const std::exception& ex) {
        LOG(ERROR, "An exception occurred during the ProcessEvent hook: {}", ex.what());
    }

    process_event_ptr(obj, func, params, null);
}

void locking_process_event_hook(UObject* obj, UFunction* func, void* params, void* null) {
    const locks::FunctionCall lock{};
    process_event_hook(obj, func, params, null);
}

static_assert(std::is_same_v<decltype(&process_event_hook), process_event_func>,
              "process_event signature is incorrect");

static_assert(std::is_same_v<decltype(&process_event_hook), decltype(&locking_process_event_hook)>,
              "process_event signature is incorrect");

}  // namespace

void BL1EHook::hook_process_event(void) {
    detour(PROCESS_EVENT_SIG,
           locks::FunctionCall::enabled() ? locking_process_event_hook : process_event_hook,
           &process_event_ptr, "ProcessEvent");
}

void BL1EHook::process_event(UObject* object, UFunction* func, void* params) const {
    locking_process_event_hook(object, func, params, nullptr);
}

namespace {

using call_function_func = void (*)(UObject* obj, FFrame* stack, void* params, UFunction* func);
call_function_func call_function_ptr;

// TODO: Refactor this
constexpr Pattern<64> CALL_FUNCTION_SIG{
    "405553565741544155415641574881ECA8040000488D6C242048C74568FEFFFFFF488B05E8A83A024833C548898570"
    "0400004D8BF94C894D60498BF04C894500"};

void __fastcall call_function_hook(UObject* obj, FFrame* stack, void* result, UFunction* func) {
    try {
        auto data = hook_manager::impl::preprocess_hook(L"CallFunction", func, obj);
        if (data != nullptr) {
            WrappedStruct args{func};
            auto original_code = stack->extract_current_args(args);

            hook_manager::Details hook{.obj = obj,
                                       .args = &args,
                                       .ret = {func->find_return_param()},
                                       .func = {.func = func, .object = obj}};

            const bool block_execution = run_hooks_of_type(data, hook_manager::Type::PRE, hook);

            if (block_execution) {
                stack->Code++;
            } else {
                stack->Code = original_code;
                call_function_ptr(obj, stack, result, func);
            }

            if (hook.ret.has_value()) {
                // Result is a pointer directly to where the property should go, remove the offset
                hook.ret.copy_to(reinterpret_cast<uintptr_t>(result)
                                 - hook.ret.prop->Offset_Internal());
            }

            if (!has_post_hooks(data)) {
                return;
            }

            if (hook.ret.prop != nullptr && !hook.ret.has_value() && !block_execution) {
                hook.ret.copy_from(reinterpret_cast<uintptr_t>(result)
                                   - hook.ret.prop->Offset_Internal());
            }

            if (!block_execution) {
                run_hooks_of_type(data, hook_manager::Type::POST, hook);
            }

            run_hooks_of_type(data, hook_manager::Type::POST_UNCONDITIONAL, hook);

            return;
        }
    } catch (const std::exception& ex) {
        LOG(ERROR, "An exception occurred during the CallFunction hook: {}", ex.what());
    }

    call_function_ptr(obj, stack, result, func);
}

void locking_call_function_hook(UObject* obj, FFrame* stack, void* result, UFunction* func) {
    const locks::FunctionCall lock{};
    call_function_hook(obj, stack, result, func);
}

static_assert(std::is_same_v<decltype(&call_function_hook), call_function_func>,
              "call_function signature is incorrect");

}  // namespace

void BL1EHook::hook_call_function(void) {
    detour(CALL_FUNCTION_SIG,
           locks::FunctionCall::enabled() ? locking_call_function_hook : call_function_hook,
           &call_function_ptr, "CallFunction");
}

}  // namespace unrealsdk::game

#endif
