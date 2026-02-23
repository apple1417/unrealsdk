#ifndef UNREALSDK_UNREAL_CLASSES_PROPERTIES_ATTRIBUTE_PROPERTY_H
#define UNREALSDK_UNREAL_CLASSES_PROPERTIES_ATTRIBUTE_PROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/properties/copyable_property.h"
#include "unrealsdk/unreal/classes/properties/ubyteproperty.h"
#include "unrealsdk/unreal/prop_traits.h"

namespace unrealsdk::unreal {

class ZArrayProperty;
class ZProperty;

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define DEFINE_ATTRIBUTE_PROPERTY(class_name, base_class, fields) \
    class class_name : public base_class {                        \
       public:                                                    \
        class_name() = delete;                                    \
        class_name(const class_name&) = delete;                   \
        class_name(class_name&&) = delete;                        \
        class_name& operator=(const class_name&) = delete;        \
        class_name& operator=(class_name&&) = delete;             \
        ~class_name() = delete;                                   \
        UNREALSDK_DEFINE_FIELDS_HEADER(class_name, fields);       \
    };                                                            \
    template <>                                                   \
    struct PropTraits<class_name> : public PropTraits<base_class> {};

// These fields become member functions, returning a reference into the object.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_ZBYTEATTRIBUTEPROPERTY_FIELDS(X) \
    X(ZArrayProperty*, ModifierStackProperty)      \
    X(ZByteAttributeProperty*, OtherAttributeProperty)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_ZFLOATATTRIBUTEPROPERTY_FIELDS(X) \
    X(ZArrayProperty*, ModifierStackProperty)       \
    X(ZFloatAttributeProperty*, OtherAttributeProperty)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNREALSDK_ZINTATTRIBUTEPROPERTY_FIELDS(X) \
    X(ZArrayProperty*, ModifierStackProperty)     \
    X(ZIntAttributeProperty*, OtherAttributeProperty)

DEFINE_ATTRIBUTE_PROPERTY(ZByteAttributeProperty,
                          ZByteProperty,
                          UNREALSDK_ZBYTEATTRIBUTEPROPERTY_FIELDS);
DEFINE_ATTRIBUTE_PROPERTY(ZFloatAttributeProperty,
                          ZFloatProperty,
                          UNREALSDK_ZFLOATATTRIBUTEPROPERTY_FIELDS);
DEFINE_ATTRIBUTE_PROPERTY(ZIntAttributeProperty,
                          ZIntProperty,
                          UNREALSDK_ZINTATTRIBUTEPROPERTY_FIELDS);

template <>
inline const wchar_t* const ClassTraits<ZByteAttributeProperty>::NAME = L"ByteAttributeProperty";
template <>
inline const wchar_t* const ClassTraits<ZFloatAttributeProperty>::NAME = L"FloatAttributeProperty";
template <>
inline const wchar_t* const ClassTraits<ZIntAttributeProperty>::NAME = L"IntAttributeProperty";

#undef DEFINE_ATTRIBUTE_PROPERTY

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_PROPERTIES_ATTRIBUTE_PROPERTY_H */
