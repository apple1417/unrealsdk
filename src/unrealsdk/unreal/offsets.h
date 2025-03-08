#ifndef UNREALSDK_UNREAL_OFFSETS_H
#define UNREALSDK_UNREAL_OFFSETS_H

#include "unrealsdk/pch.h"

namespace unrealsdk::unreal::offsets {

using offset_type = uint16_t;

template <typename From, typename To>
using copy_cv = std::conditional_t<
    std::is_const_v<From>,
    std::add_const_t<std::conditional_t<std::is_volatile_v<From>, std::add_volatile_t<To>, To>>,
    /* align      */ std::conditional_t<std::is_volatile_v<From>, std::add_volatile_t<To>, To>>;

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

// Internal macros - reflection can't come soon enough
#define UNREALSDK_OFFSETS__EMPTY 1
#define UNREALSDK_OFFSETS__NOT_EMPTY 0
#define UNREALSDK_OFFSETS__IS_EMPTY(...) UNREALSDK_OFFSETS__##__VA_OPT__(NOT_)##EMPTY
#define UNREALSDK_OFFSETS__EVAL_IF_0(true_val, false_val) false_val
#define UNREALSDK_OFFSETS__EVAL_IF_1(true_val, false_val) true_val
#define UNREALSDK_OFFSETS__EVAL_IF(cond, true_val, false_val) \
    UNREALSDK_OFFSETS__EVAL_IF_##cond(true_val, false_val)
#define UNREALSDK_OFFSETS__IF(cond, true_val, false_val) \
    UNREALSDK_OFFSETS__EVAL_IF(cond, true_val, false_val)
#define UNREALSDK_OFFSETS__IF_NOT_EMPTY(test, value) \
    UNREALSDK_OFFSETS__IF(UNREALSDK_OFFSETS__IS_EMPTY(test), , value)

#define UNREALSDK_OFFSETS__DEFINE_OFFSET_MEMBERS(type, name, proxy_arg_type, proxy_arg_name) \
    unrealsdk::unreal::offsets::offset_type name;
#define UNREALSDK_OFFSETS__DEFINE_PROXY_ARG_MEMBERS(type, name, proxy_arg_type, proxy_arg_name) \
    UNREALSDK_OFFSETS__IF_NOT_EMPTY(proxy_arg_type, proxy_arg_type name##_##proxy_arg_name;)

#define UNREALSDK_OFFSETS__OFFSETOF_ASSERTS(type, name, proxy_arg_type, proxy_arg_name) \
    static_assert(offsetof(T, name)                                                     \
                  < std::numeric_limits<unrealsdk::unreal::offsets::offset_type>::max());
#define UNREALSDK_OFFSETS__OFFSETOF(type, name, proxy_arg_type, proxy_arg_name) \
    static_cast<unrealsdk::unreal::offsets::offset_type>(offsetof(T, name)),
#define UNREALSDK_OFFSETS__COPY_PROXY_ARG(type, name, proxy_arg_type, proxy_arg_name) \
    UNREALSDK_OFFSETS__IF_NOT_EMPTY(proxy_arg_type, T::name##_##proxy_arg_name;)

#define UNREALSDK_OFFSETS__DEFINE_REFERENCE_GETTER(type, name, proxy_arg_type, proxy_arg_name) \
    template <typename T>                                                                      \
    [[nodiscard]] unrealsdk::unreal::offsets::copy_cv<T, type>& name(this T& self) {           \
        return *reinterpret_cast<unrealsdk::unreal::offsets::copy_cv<T, type>*>(               \
            reinterpret_cast<uintptr_t>(&self) + Offsets::get().name);                         \
    }
#define UNREALSDK_OFFSETS__DEFINE_PROXY_GETTER(type, name, proxy_arg_type, proxy_arg_name) \
    template <typename T>                                                                  \
    [[nodiscard]] unrealsdk::unreal::offsets::copy_cv<T, type> name(this T& self) {        \
        return {reinterpret_cast<uintptr_t>(&self) + Offsets::get().name,                  \
                Offsets::get().name##_##proxy_arg_name};                                   \
    }
#define UNREALSDK_OFFSETS__DEFINE_GETTER(type, name, proxy_arg_type, proxy_arg_name)             \
    UNREALSDK_OFFSETS__IF(                                                                       \
        UNREALSDK_OFFSETS__IS_EMPTY(proxy_arg_type), UNREALSDK_OFFSETS__DEFINE_REFERENCE_GETTER, \
        UNREALSDK_OFFSETS__DEFINE_PROXY_GETTER)(type, name, proxy_arg_type, proxy_arg_name)

#if defined(__MINGW32__) || defined(__clang__)
#define UNREALSDK_OFFSETS__OFFSETOF_PRAGMA_PUSH \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Winvalid-offsetof\"")
#define UNREALSDK_OFFSETS__OFFSETOF_PRAGMA_POP _Pragma("GCC diagnostic pop")
#else
#define UNREALSDK_OFFSETS__OFFSETOF_PRAGMA_PUSH
#define UNREALSDK_OFFSETS__OFFSETOF_PRAGMA_POP
#endif

/**
 * @brief Header file macro to defines all the machinery for variable offset fields.
 * @note Should be placed within the class definition.
 *
 * @param ClassName The name of the class these fields are being defined for.
 * @param X_MACRO An X macro list of the fields to define. See the example below.
 */
#define UNREALSDK_DEFINE_FIELDS_HEADER(ClassName, X_MACRO)           \
   public:                                                           \
    struct Offsets;                                                  \
    X_MACRO(UNREALSDK_OFFSETS__DEFINE_GETTER)                        \
    struct Offsets {                                                 \
        X_MACRO(UNREALSDK_OFFSETS__DEFINE_OFFSET_MEMBERS)            \
        X_MACRO(UNREALSDK_OFFSETS__DEFINE_PROXY_ARG_MEMBERS)         \
        template <typename T>                                        \
        static constexpr Offsets from(void) {                        \
            UNREALSDK_OFFSETS__OFFSETOF_PRAGMA_PUSH                  \
            X_MACRO(UNREALSDK_OFFSETS__OFFSETOF_ASSERTS);            \
            return {X_MACRO(UNREALSDK_OFFSETS__OFFSETOF)             \
                        X_MACRO(UNREALSDK_OFFSETS__COPY_PROXY_ARG)}; \
            UNREALSDK_OFFSETS__OFFSETOF_PRAGMA_POP                   \
        }                                                            \
        static const Offsets& get(void);                             \
    }  // deliberately no semicolon - forward declared earlier so that we could put this last

/**
 * @brief Source file macro to defines all the machinery for variable offset fields.
 * @note Should be placed at the top level (but within the relevant namespace).
 *
 * @param ClassName The name of the class these fields are being defined for.
 * @param X_MACRO An X macro list of the fields to define. See the example below.
 */
#define UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(ClassName, X_MACRO) \
    const ClassName::Offsets& ClassName::Offsets::get(void) {   \
        return unrealsdk::internal::get_offsets().ClassName;    \
    }

#if 0  // NOLINT(readability-avoid-unconditional-preprocessor-if)

// The above macros take an X macro field list. This is a macro function, which takes another
// macro function, and calls it for each field

#define UOBJECT_FIELDS(X) \
    X(UClass*, Class, , ) \
    X(BitFieldProxy, ObjectFlags, uint8_t, BitSize)

// The inner macro always takes the following args:
// - The field's type
// - The field's name
// - [Optional] The type of an extra arg used to create a field proxy
// - [Optional] The name of the proxy arg

// Calling the macro
UNREALSDK_DEFINE_FIELDS_HEADER(UObject, UOBJECT_FIELDS);

// Creates a class approximately like the following:
struct Offsets {
    offset_type Class;
    offset_type ObjectFlags;
    uint8_t ObjectFlags_BitSize;

    template <typename T>
    static constexpr Offsets from(void) {
        return {
            offsetof(T, Class),
            offsetof(T, ObjectFlags),
            T::ObjectFlags_BitSize,
        };
    }
    static const Offsets& get(void);  // Defined in C file to avoid a recursive include
};

// You can use a one-liner to fill this with all the relevant offsets from another type. All fields
// must be public, and proxy args should be constexpr copyable.
auto bl2_offsets = Offsets::from<bl2::UObject>();

// The macro also creates a bunch of getters on the object

// Values without a proxy look like the following, returning a reference to the member
template <typename T>
[[nodiscard]] unrealsdk::unreal::offsets::copy_cv<T, UClass*>& Class(this T& self) {
    return *reinterpret_cast<unrealsdk::unreal::offsets::copy_cv<T, UClass*>*>(
        reinterpret_cast<uintptr_t>(&self) + Offsets::get().Class);
}

// Since these return a reference, they can be used pretty much the exact same way as a member.
auto cls = obj->Class()->find(L"MyField"_fn);
obj->Class() = some_other_class;

// Values with a proxy instead look like this, creating a new object using the address of the field,
// and the proxy arg.
template <typename T>
[[nodiscard]] unrealsdk ::unreal ::offsets ::copy_cv<T, BitFieldProxy> ObjectFlags(this T& self) {
    return {reinterpret_cast<uintptr_t>(&self) + Offsets ::get().ObjectFlags,
            Offsets ::get().ObjectFlags_BitSize};
}

// The proxy type is expected to implement all the relevant assignment operators, so that these
// still behave the same as a member
obj->ObjectFlags() |= 8;
obj->ObjectFlags() = 0x124;

// Storing the value returned from either type of function has the exact same semantics as taking a
// member reference normally  - i.e. don't do it, it's only valid for the parent object's lifetime.

#endif

// NOLINTEND(cppcoreguidelines-macro-usage)

}  // namespace unrealsdk::unreal::offsets

#endif /* UNREALSDK_UNREAL_OFFSETS_H */
