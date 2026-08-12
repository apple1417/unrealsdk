#include "unrealsdk/pch.h"
#include "unrealsdk/locks.h"
#include "unrealsdk/config.h"

#ifndef UNREALSDK_IMPORTING

namespace unrealsdk::locks {

namespace {

std::recursive_mutex function_call_mutex;
std::recursive_mutex load_package_mutex;

}  // namespace

bool FunctionCall::enabled(void) {
    static auto enabled = config::get_bool("unrealsdk.locking_function_calls").value_or(false);
    return enabled;
}

FunctionCall::FunctionCall() {
    if (enabled()) {
        function_call_mutex.lock();
    }
}

FunctionCall::~FunctionCall() {
    if (enabled()) {
        function_call_mutex.unlock();
    }
}

bool LoadPackage::enabled(void) {
    static auto enabled = config::get_bool("unrealsdk.locking_load_package").value_or(false);
    return enabled;
}

LoadPackage::LoadPackage() {
    if (enabled()) {
        load_package_mutex.lock();
    }
}

LoadPackage::~LoadPackage() {
    if (enabled()) {
        load_package_mutex.unlock();
    }
}

}  // namespace unrealsdk::locks

#endif
