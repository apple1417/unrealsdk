#ifndef UNREALSDK_UNREAL_PROPERTIES_PERSISTENT_OBJECT_PTR_PROPERTY_H
#define UNREALSDK_UNREAL_PROPERTIES_PERSISTENT_OBJECT_PTR_PROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/properties/zobjectproperty.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"

namespace unrealsdk::unreal {

struct FLazyObjectPath;
struct FSoftObjectPath;
class UObject;

class ZLazyObjectProperty : public ZObjectProperty {
   public:
    ZLazyObjectProperty() = delete;
    ZLazyObjectProperty(const ZLazyObjectProperty&) = delete;
    ZLazyObjectProperty(ZLazyObjectProperty&&) = delete;
    ZLazyObjectProperty& operator=(const ZLazyObjectProperty&) = delete;
    ZLazyObjectProperty& operator=(ZLazyObjectProperty&&) = delete;
    ~ZLazyObjectProperty() = delete;
};

class ZSoftObjectProperty : public ZObjectProperty {
   public:
    ZSoftObjectProperty() = delete;
    ZSoftObjectProperty(const ZSoftObjectProperty&) = delete;
    ZSoftObjectProperty(ZSoftObjectProperty&&) = delete;
    ZSoftObjectProperty& operator=(const ZSoftObjectProperty&) = delete;
    ZSoftObjectProperty& operator=(ZSoftObjectProperty&&) = delete;
    ~ZSoftObjectProperty() = delete;
};

// Not entirely sure if this should inherit soft object property or class property, but soft object
// object simplifies a lot of our code
class ZSoftClassProperty : public ZSoftObjectProperty {
   public:
    ZSoftClassProperty() = delete;
    ZSoftClassProperty(const ZSoftClassProperty&) = delete;
    ZSoftClassProperty(ZSoftClassProperty&&) = delete;
    ZSoftClassProperty& operator=(const ZSoftClassProperty&) = delete;
    ZSoftClassProperty& operator=(ZSoftClassProperty&&) = delete;
    ~ZSoftClassProperty() = delete;

    // These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_ZSOFTCLASSPROPERTY_FIELDS(X) X(UClass*, MetaClass)

    UNREALSDK_DEFINE_FIELDS_HEADER(ZSoftClassProperty, UNREALSDK_ZSOFTCLASSPROPERTY_FIELDS);
};

template <>
struct PropTraits<ZLazyObjectProperty> : public AbstractPropTraits<ZLazyObjectProperty> {
    using Value = UObject*;

    static Value get(const ZLazyObjectProperty* prop,
                     uintptr_t addr,
                     const UnrealPointer<void>& parent);
    static void set(const ZLazyObjectProperty* prop, uintptr_t addr, const Value& value);

    static const FLazyObjectPath* get_identifier(const ZLazyObjectProperty* prop,
                                                 uintptr_t addr,
                                                 const UnrealPointer<void>& parent);
};

template <>
struct PropTraits<ZSoftObjectProperty> : public AbstractPropTraits<ZSoftObjectProperty> {
    using Value = UObject*;

    static Value get(const ZSoftObjectProperty* prop,
                     uintptr_t addr,
                     const UnrealPointer<void>& parent);
    static void set(const ZSoftObjectProperty* prop, uintptr_t addr, const Value& value);
    static void destroy(const ZSoftObjectProperty* prop, uintptr_t addr);

    static const FSoftObjectPath* get_identifier(const ZSoftObjectProperty* prop,
                                                 uintptr_t addr,
                                                 const UnrealPointer<void>& parent);
};

template <>
struct PropTraits<ZSoftClassProperty> : public AbstractPropTraits<ZSoftClassProperty> {
    using Value = UClass*;

    static Value get(const ZSoftClassProperty* prop,
                     uintptr_t addr,
                     const UnrealPointer<void>& parent);
    static void set(const ZSoftClassProperty* prop, uintptr_t addr, const Value& value);
    static void destroy(const ZSoftClassProperty* prop, uintptr_t addr);

    static const FSoftObjectPath* get_identifier(const ZSoftClassProperty* prop,
                                                 uintptr_t addr,
                                                 const UnrealPointer<void>& parent);
};

template <>
struct ClassTraits<ZLazyObjectProperty> {
    static inline const wchar_t* const NAME = L"LazyObjectProperty";
};
template <>
struct ClassTraits<ZSoftObjectProperty> {
    static inline const wchar_t* const NAME = L"SoftObjectProperty";
};
template <>
struct ClassTraits<ZSoftClassProperty> {
    static inline const wchar_t* const NAME = L"SoftClassProperty";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_PROPERTIES_PERSISTENT_OBJECT_PTR_PROPERTY_H */
