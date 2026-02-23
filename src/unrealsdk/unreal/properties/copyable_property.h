#ifndef UNREALSDK_UNREAL_PROPERTIES_COPYABLE_PROPERTY_H
#define UNREALSDK_UNREAL_PROPERTIES_COPYABLE_PROPERTY_H

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/prop_traits.h"
#include "unrealsdk/unreal/properties/zproperty.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"

/*
This file describes all properties which can be read/written by a simple copy, and whose classes
 don't need to add any extra fields to `ZProperty` (or at least not any we care about).
*/

namespace unrealsdk::unreal {

template <typename T>
class CopyableProperty : public ZProperty {
   public:
    CopyableProperty() = delete;
    CopyableProperty(const CopyableProperty&) = delete;
    CopyableProperty(CopyableProperty&&) = delete;
    CopyableProperty& operator=(const CopyableProperty&) = delete;
    CopyableProperty& operator=(CopyableProperty&&) = delete;
    ~CopyableProperty() = delete;
};

template <typename T>
struct PropTraits<CopyableProperty<T>> : public AbstractPropTraits<CopyableProperty<T>> {
    using Value = T;

    static Value get(const CopyableProperty<T>* /*prop*/,
                     uintptr_t addr,
                     const UnrealPointer<void>& /*parent*/) {
        return *reinterpret_cast<Value*>(addr);
    }
    static void set(const CopyableProperty<T>* /*prop*/, uintptr_t addr, const Value& value) {
        *reinterpret_cast<Value*>(addr) = value;
    }
};

using ZInt8Property = CopyableProperty<int8_t>;
using ZInt16Property = CopyableProperty<int16_t>;
using ZIntProperty = CopyableProperty<int32_t>;
using ZInt64Property = CopyableProperty<int64_t>;

// ZByteProperty has an extra field we care about, it's handled in a different file
using ZUInt16Property = CopyableProperty<uint16_t>;
using ZUInt32Property = CopyableProperty<uint32_t>;
using ZUInt64Property = CopyableProperty<uint64_t>;

using ZFloatProperty = CopyableProperty<float32_t>;
using ZDoubleProperty = CopyableProperty<float64_t>;

using ZNameProperty = CopyableProperty<FName>;

template <>
inline const wchar_t* const ClassTraits<ZInt8Property>::NAME = L"Int8Property";
template <>
inline const wchar_t* const ClassTraits<ZInt16Property>::NAME = L"Int16Property";
template <>
inline const wchar_t* const ClassTraits<ZIntProperty>::NAME = L"IntProperty";
template <>
inline const wchar_t* const ClassTraits<ZInt64Property>::NAME = L"Int64Property";

template <>
inline const wchar_t* const ClassTraits<ZUInt16Property>::NAME = L"UInt16Property";
template <>
inline const wchar_t* const ClassTraits<ZUInt32Property>::NAME = L"UInt32Property";
template <>
inline const wchar_t* const ClassTraits<ZUInt64Property>::NAME = L"UInt64Property";

template <>
inline const wchar_t* const ClassTraits<ZFloatProperty>::NAME = L"FloatProperty";
template <>
inline const wchar_t* const ClassTraits<ZDoubleProperty>::NAME = L"DoubleProperty";

template <>
inline const wchar_t* const ClassTraits<ZNameProperty>::NAME = L"NameProperty";

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_PROPERTIES_COPYABLE_PROPERTY_H */
