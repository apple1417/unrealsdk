#include "unrealsdk/pch.h"

#include "unrealsdk/env.h"
#include "unrealsdk/game/bl2/bl2.h"
#include "unrealsdk/hook_manager.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/classes/ufunction.h"
#include "unrealsdk/unreal/classes/uobject.h"
#include "unrealsdk/unreal/classes/uproperty.h"
#include "unrealsdk/unreal/structs/fframe.h"
#include "unrealsdk/unreal/wrappers/bound_function.h"
#include "unrealsdk/unreal/wrappers/property_proxy.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer_funcs.h"
#include "unrealsdk/unreal/wrappers/wrapped_struct.h"
#include "unrealsdk/unrealsdk.h"

#if defined(UE3) && defined(ARCH_X86) && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {

namespace {

// This function is actually thiscall, but MSVC won't let us declare static thiscall functions
// As a workaround, declare it fastcall, and add a dummy edx arg.
// NOLINTNEXTLINE(modernize-use-using)
typedef void(__fastcall* process_event_func)(UObject* obj,
                                             void* /*edx*/,
                                             UFunction* func,
                                             void* params,
                                             void* /*null*/);
process_event_func process_event_ptr;

const constinit Pattern<45> PROCESS_EVENT_SIG{
    "55"              // push ebp
    "8B EC"           // mov ebp, esp
    "6A FF"           // push -01
    "68 ????????"     // push Borderlands2.exe+11107D8
    "64 A1 ????????"  // mov eax, fs:[00000000]
    "50"              // push eax
    "83 EC 50"        // sub esp, 50
    "A1 ????????"     // mov eax, [Borderlands2.g_LEngineDefaultPoolId+B2DC]
    "33 C5"           // xor eax,ebp
    "89 45 ??"        // mov [ebp-10], eax
    "53"              // push ebx
    "56"              // push esi
    "57"              // push edi
    "50"              // push eax
    "8D 45 ??"        // lea eax, [ebp-0C]
    "64 A3 ????????"  // mov fs:[00000000], eax
    "8B F1"           // mov esi, ecx
};

void __fastcall process_event_hook(UObject* obj,
                                   void* edx,
                                   UFunction* func,
                                   void* params,
                                   void* null) {
    try {
        // This arg seems to be in the process of being deprecated, no usage in ghidra, always seems
        // to be null, and it's gone in later ue versions. Gathering some extra info just in case.
        if (null != nullptr) {
            LOG(DEV_WARNING, L"Null param had a value in process event during func {} on obj {}",
                func->get_path_name(), obj->get_path_name());
        }

        auto data = hook_manager::impl::preprocess_hook("ProcessEvent", func, obj);
        if (data != nullptr) {
            // Copy args so that hooks can't modify them, for parity with call function
            const WrappedStruct args_base{func, params};
            WrappedStruct args = args_base.copy_params_only();
            hook_manager::Details hook{.obj = obj,
                                       .args = &args,
                                       .ret = {func->find_return_param()},
                                       .func = {.func = func, .object = obj}};

            const bool block_execution =
                hook_manager::impl::run_hooks_of_type(*data, hook_manager::Type::PRE, hook);

            if (!block_execution) {
                process_event_ptr(obj, edx, func, params, null);
            }

            if (hook.ret.has_value()) {
                hook.ret.copy_to(reinterpret_cast<uintptr_t>(params));
            }

            if (!hook_manager::impl::has_post_hooks(*data)) {
                return;
            }

            if (hook.ret.prop != nullptr && !hook.ret.has_value() && !block_execution) {
                hook.ret.copy_from(reinterpret_cast<uintptr_t>(params));
            }

            if (!block_execution) {
                hook_manager::impl::run_hooks_of_type(*data, hook_manager::Type::POST, hook);
            }

            hook_manager::impl::run_hooks_of_type(*data, hook_manager::Type::POST_UNCONDITIONAL,
                                                  hook);

            return;
        }
    } catch (const std::exception& ex) {
        LOG(ERROR, "An exception occurred during the ProcessEvent hook: {}", ex.what());
    }

    process_event_ptr(obj, edx, func, params, null);
}

std::recursive_mutex process_event_mutex{};

void __fastcall locking_process_event_hook(UObject* obj,
                                           void* edx,
                                           UFunction* func,
                                           void* params,
                                           void* null) {
    const std::lock_guard<std::recursive_mutex> lock{process_event_mutex};
    process_event_hook(obj, edx, func, params, null);
}

static_assert(std::is_same_v<decltype(&process_event_hook), process_event_func>,
              "process_event signature is incorrect");
static_assert(std::is_same_v<decltype(&process_event_hook), decltype(&locking_process_event_hook)>,
              "process_event signature is incorrect");

/**
 * @brief Checks if we should use a locking process event implementation.
 *
 * @return True if we should use locks.
 */
bool locking(void) {
    // Basically just a function so we can be sure this static is initialized late - LTO hopefully
    // takes care of it
    static auto locking = env::defined(env::LOCKING_PROCESS_EVENT);
    return locking;
}

}  // namespace

void BL2Hook::hook_process_event(void) {
    detour(PROCESS_EVENT_SIG, locking() ? locking_process_event_hook : process_event_hook,
           &process_event_ptr, "ProcessEvent");
}

void BL2Hook::process_event(UObject* object, UFunction* func, void* params) const {
    if (locking()) {
        const std::lock_guard<std::recursive_mutex> lock{process_event_mutex};
        process_event_hook(object, nullptr, func, params, nullptr);
    } else {
        process_event_hook(object, nullptr, func, params, nullptr);
    }
}

namespace {

// NOLINTNEXTLINE(modernize-use-using)
typedef void(__fastcall* call_function_func)(UObject* obj,
                                             void* /*edx*/,
                                             FFrame* stack,
                                             void* params,
                                             UFunction* func);
call_function_func call_function_ptr;

const constinit Pattern<23> CALL_FUNCTION_SIG{
    "55"              // push ebp
    "8B EC"           // mov ebp, esp
    "6A FF"           // push -01
    "68 ????????"     // push Borderlands2.exe+1110791
    "64 A1 ????????"  // mov eax, fs:[00000000]
    "50"              // push eax
    "81 EC 58040000"  // sub esp, 00000458
};

void __fastcall call_function_hook(UObject* obj,
                                   void* edx,
                                   FFrame* stack,
                                   void* result,
                                   UFunction* func) {
    try {
        auto data = hook_manager::impl::preprocess_hook("CallFunction", func, obj);
        if (data != nullptr) {
            WrappedStruct args{func};
            auto original_code = stack->extract_current_args(args);

            hook_manager::Details hook{.obj = obj,
                                       .args = &args,
                                       .ret = {func->find_return_param()},
                                       .func = {.func = func, .object = obj}};

            const bool block_execution =
                hook_manager::impl::run_hooks_of_type(*data, hook_manager::Type::PRE, hook);

            if (block_execution) {
                stack->Code++;
            } else {
                stack->Code = original_code;
                call_function_ptr(obj, edx, stack, result, func);
            }

            if (hook.ret.has_value()) {
                // Result is a pointer directly to where the property should go, remove the offset
                hook.ret.copy_to(reinterpret_cast<uintptr_t>(result)
                                 - hook.ret.prop->Offset_Internal);
            }

            if (!hook_manager::impl::has_post_hooks(*data)) {
                return;
            }

            if (hook.ret.prop != nullptr && !hook.ret.has_value() && !block_execution) {
                hook.ret.copy_from(reinterpret_cast<uintptr_t>(result)
                                   - hook.ret.prop->Offset_Internal);
            }

            if (!block_execution) {
                hook_manager::impl::run_hooks_of_type(*data, hook_manager::Type::POST, hook);
            }

            hook_manager::impl::run_hooks_of_type(*data, hook_manager::Type::POST_UNCONDITIONAL,
                                                  hook);

            return;
        }
    } catch (const std::exception& ex) {
        LOG(ERROR, "An exception occurred during the CallFunction hook: {}", ex.what());
    }

    call_function_ptr(obj, edx, stack, result, func);
}
static_assert(std::is_same_v<decltype(&call_function_hook), call_function_func>,
              "call_function signature is incorrect");

}  // namespace

void BL2Hook::hook_call_function(void) {
    detour(CALL_FUNCTION_SIG, call_function_hook, &call_function_ptr, "CallFunction");
}

}  // namespace unrealsdk::game

#endif
