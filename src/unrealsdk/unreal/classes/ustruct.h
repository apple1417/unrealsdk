#ifndef UNREALSDK_UNREAL_CLASSES_USTRUCT_H
#define UNREALSDK_UNREAL_CLASSES_USTRUCT_H

#include "unrealsdk/unreal/class_traits.h"
#include "unrealsdk/unreal/classes/ufield.h"
#include "unrealsdk/unreal/classes/uproperty.h"
#include "unrealsdk/unreal/structs/tarray.h"
#include "unrealsdk/unreal/structs/tfieldvariant.h"
#include "unrealsdk/utils.h"

namespace unrealsdk::unreal {

class UFunction;
class ZProperty;

class UStruct : public UField {
   public:
    UStruct() = delete;
    UStruct(const UStruct&) = delete;
    UStruct(UStruct&&) = delete;
    UStruct& operator=(const UStruct&) = delete;
    UStruct& operator=(UStruct&&) = delete;
    ~UStruct() = delete;

    using property_size_type = UNREALSDK_USTRUCT_PROPERTY_SIZE_TYPE;

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#if UNREALSDK_USTRUCT_HAS_ALIGNMENT
#define UNREALSDK__USTRUCT_MIN_ALIGNMENT(X) X(int32_t, MinAlignment)
#else
#define UNREALSDK__USTRUCT_MIN_ALIGNMENT(X)
#endif
#if UNREALSDK_PROPERTIES_ARE_FFIELD
#define UNREALSDK__USTRUCT_CHILD_PROPERTIES(X) X(FField*, ChildProperties)
#else
#define UNREALSDK__USTRUCT_CHILD_PROPERTIES(X)
#endif

    // These fields become member functions, returning a reference into the object.
#define UNREALSDK_USTRUCT_FIELDS(X)     \
    X(UStruct*, SuperField)             \
    X(UField*, Children)                \
    X(property_size_type, PropertySize) \
    X(ZProperty*, PropertyLink)         \
    UNREALSDK__USTRUCT_MIN_ALIGNMENT(X) \
    UNREALSDK__USTRUCT_CHILD_PROPERTIES(X)

    // NOLINTEND(cppcoreguidelines-macro-usage)

    UNREALSDK_DEFINE_FIELDS_HEADER(UStruct, UNREALSDK_USTRUCT_FIELDS);

#pragma region Iterators
    struct FieldIterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = UField*;
        using pointer = UField**;
        using reference = UField*;

        friend class UStruct;

       private:
        const UStruct* this_struct;
        UField* field;

        FieldIterator(const UStruct* this_struct, UField* field);

       public:
        FieldIterator(void);

        reference operator*() const;

        FieldIterator& operator++();
        FieldIterator operator++(int);

        bool operator==(const FieldIterator& rhs) const;
        bool operator!=(const FieldIterator& rhs) const;
    };

    struct PropertyIterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = ZProperty*;
        using pointer = ZProperty**;
        using reference = ZProperty*;

        friend class UStruct;

       private:
#if UNREALSDK_USTRUCT_PROPERTY_ITER == UNREALSDK_USTRUCT_PROPERTY_ITER_PROPERTYLINK
        ZProperty* prop;

        PropertyIterator(ZProperty* prop);
#elif UNREALSDK_USTRUCT_PROPERTY_ITER == UNREALSDK_USTRUCT_PROPERTY_ITER_CHILDPROPERTIES
        const UStruct* this_struct;
        FField* field;

        PropertyIterator(const UStruct* this_struct, FField* field);
#else
#error Unknown UStruct::properties() iterator type
#endif

       public:
        PropertyIterator(void);

        reference operator*() const;

        PropertyIterator& operator++();
        PropertyIterator operator++(int);

        bool operator==(const PropertyIterator& rhs) const;
        bool operator!=(const PropertyIterator& rhs) const;
    };

    struct SuperFieldIterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const UStruct*;
        using pointer = const UStruct**;
        using reference = const UStruct*;

        friend class UStruct;

       private:
        const UStruct* this_struct;

        SuperFieldIterator(const UStruct* this_struct);

       public:
        SuperFieldIterator(void);

        reference operator*() const;

        SuperFieldIterator& operator++();
        SuperFieldIterator operator++(int);

        bool operator==(const SuperFieldIterator& rhs) const;
        bool operator!=(const SuperFieldIterator& rhs) const;
    };

    /**
     * @brief Gets an iterator over this struct's fields.
     *
     * @return The iterator.
     */
    [[nodiscard]] utils::IteratorProxy<FieldIterator> fields(void) const;

    /**
     * @brief Gets an iterator over this struct's properties.
     *
     * @return The iterator.
     */
    [[nodiscard]] utils::IteratorProxy<PropertyIterator> properties(void) const;

    /**
     * @brief Gets an iterator over this struct and it's superfields.
     * @note Includes this struct itself.
     *
     * @return The iterator.
     */
    [[nodiscard]] utils::IteratorProxy<SuperFieldIterator> superfields(void) const;
#pragma endregion

    /**
     * @brief Get the actual size of the described structure, including alignment.
     *
     * @return The size which must be allocated
     */
    [[nodiscard]] size_t get_struct_size(void) const;

    /**
     * @brief Finds a child field/property by name.
     * @note Throws an exception if the child is not found.
     * @note When known to be a property, property lookup is more efficient.
     *
     * @param name The name of the child.
     * @return The found child object.
     */
#if UNREALSDK_PROPERTIES_ARE_FFIELD
    [[nodiscard]] TFieldVariant<ZProperty, UField> find(const FName& name) const;
#else
    [[nodiscard]] TFieldVariantStub<UField> find(const FName& name) const;
#endif
    [[nodiscard]] ZProperty* find_prop(const FName& name) const;

    /**
     * @brief Finds a child property/function by name, and validates that it's of the expected type.
     * @note Throws exceptions if the child is not found, or if it's of an invalid type.
     *
     * @tparam T The expected property type.
     * @param name The name of the child.
     * @return The found child object.
     */
    template <typename T>
    [[nodiscard]] T* find_prop_and_validate(const FName& name) const;
    [[nodiscard]] UFunction* find_func_and_validate(const FName& name) const;

    /**
     * @brief Checks if this structs inherits from another.
     * @note Also returns true if this struct *is* the given struct.
     *
     * @param base_struct The base struct to check if this inherits from.
     * @return True if this struct is the given struct, or inherits from it.
     */
    [[nodiscard]] bool inherits(const UStruct* base_struct) const;
};

template <>
struct ClassTraits<UStruct> {
    static inline const wchar_t* const NAME = L"Struct";
};

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_CLASSES_USTRUCT_H */
