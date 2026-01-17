#ifndef UNREALSDK_UNREAL_STRUCTS_TFIELDVARIANT_H
#define UNREALSDK_UNREAL_STRUCTS_TFIELDVARIANT_H

#include "unrealsdk/pch.h"

namespace unrealsdk::unreal {

struct FField;
class UObject;

UNREALSDK_UNREAL_STRUCT_PADDING_PUSH()

// Not sure if this is actually a template in practice, but we'll make it one to be able to limit
// subclasses for our use cases
template <typename FFieldType, typename UObjectType>
struct TFieldVariant {
   private:
    static constexpr auto IS_OBJ_FLAG = 1;

    uintptr_t ptr = 0;

   public:
    /**
     * @brief Constructs a new field variant.
     * @note Default constructs to a null pointer.
     *
     * @param field The FField to construct it based on.
     * @param obj The UObject to construct it based on.
     * @param other The existing field variant to construct it based on.
     */
    TFieldVariant(void) = default;
    TFieldVariant(std::nullptr_t) {};
    TFieldVariant(const FFieldType* field) { *this = field; };
    TFieldVariant(const UObjectType* obj) { *this = obj; };
    TFieldVariant(const TFieldVariant& other) { *this = other; };
    TFieldVariant(TFieldVariant&& other) noexcept { *this = other; };

    /**
     * @brief Assigns a new value to the variant.
     *
     * @param field The FField to assign.
     * @param obj The UObject to assign.
     * @param other The existing field variant to copy/move from.
     * @return A reference to this variant.
     */
    TFieldVariant& operator=(std::nullptr_t) {
        this->ptr = 0;
        return *this;
    }
    TFieldVariant& operator=(const FFieldType* field) {
        this->ptr = reinterpret_cast<uintptr_t>(field);
        return *this;
    }
    TFieldVariant& operator=(const UObjectType* obj) {
        if (obj == nullptr) {
            // In case someone tries to force the issue...
            this->ptr = 0;
        } else {
            this->ptr = reinterpret_cast<uintptr_t>(obj) | IS_OBJ_FLAG;
        }
        return *this;
    }
    TFieldVariant& operator=(const TFieldVariant& other) = default;
    TFieldVariant& operator=(TFieldVariant&& other) noexcept = default;

    /**
     * @brief Destroys the variant.
     */
    ~TFieldVariant(void) = default;

    /**
     * @brief Checks if the variant points to the same object as another.
     *
     * @param field An FField to compare with.
     * @param obj A UObject to compare with.
     * @param other A variant to compare with.
     * @return True if they point to the same object.
     */
    [[nodiscard]] bool operator==(std::nullptr_t) const { return this->ptr == 0; }
    [[nodiscard]] bool operator==(const FFieldType* field) const {
        return this->ptr == reinterpret_cast<uintptr_t>(field);
    }
    [[nodiscard]] bool operator==(const UObjectType* obj) const {
        return this->ptr == (reinterpret_cast<uintptr_t>(obj) | IS_OBJ_FLAG);
    }
    [[nodiscard]] bool operator==(const TFieldVariant& other) const {
        return this->ptr == other.ptr;
    }

    /**
     * @brief Checks if this variant holds a field or an object.
     * @note Both of these may return false, if this variant contains a null pointer.
     *
     * @return True if it holds an object of the relevant type.
     */
    [[nodiscard]] bool is_field(void) const {
        return this->ptr != 0 && (this->ptr & IS_OBJ_FLAG) == 0;
    }
    [[nodiscard]] bool is_obj(void) const {
        return this->ptr != 0 && (this->ptr & IS_OBJ_FLAG) != 0;
    }

    /**
     * @brief Gets the pointer out of a variant.
     *
     * @return The contained pointer, or nullptr if the wrong type.
     */
    [[nodiscard]] FFieldType* as_field(void) const {
        return is_field() ? reinterpret_cast<FFieldType*>(this->ptr) : nullptr;
    }
    [[nodiscard]] UObjectType* as_obj(void) const {
        return is_obj() ? reinterpret_cast<UObjectType*>(this->ptr & ~IS_OBJ_FLAG) : nullptr;
    }

    /**
     * @brief Calls a lambda with the contained pointer, templated on it's type.
     * @note This *includes* std::nullptr_t, if this variant holds a null pointer.
     *
     * @tparam F The function type - picked up automatically.
     * @param func The function to call.
     */
    template <typename F>
    void cast(const F& func) const {
        if (this->ptr == 0) {
            func.template operator()<std::nullptr_t>(nullptr);
        } else if ((this->ptr & IS_OBJ_FLAG) != 0) {
            func.template operator()<UObjectType>(
                reinterpret_cast<UObjectType*>(this->ptr & ~IS_OBJ_FLAG));
        } else {
            func.template operator()<FFieldType>(reinterpret_cast<FFieldType*>(this->ptr));
        }
    }
};

// Stub type with mostly the same interface, but which only ever holds an object.
// Intended to be used when `UNREALSDK_PROPERTIES_ARE_FFIELD` is false.
template <typename UObjectType>
struct TFieldVariantStub {
   private:
    UObjectType* ptr = nullptr;

   public:
    TFieldVariantStub(void) = default;
    TFieldVariantStub(std::nullptr_t) {};
    // Cannot define a sane TFieldVariantStub(const FFieldType* field);
    TFieldVariantStub(UObjectType* obj) { *this = obj; };
    TFieldVariantStub(const TFieldVariantStub& other) { *this = other; };
    TFieldVariantStub(TFieldVariantStub&& other) noexcept { *this = other; };

    TFieldVariantStub& operator=(std::nullptr_t) {
        this->ptr = nullptr;
        return *this;
    }
    // Cannot define a sane TFieldVariantStub& operator=(const FFieldType* field);
    TFieldVariantStub& operator=(UObjectType* obj) {
        this->ptr = obj;
        return *this;
    }
    TFieldVariantStub& operator=(const TFieldVariantStub& other) = default;
    TFieldVariantStub& operator=(TFieldVariantStub&& other) noexcept = default;

    ~TFieldVariantStub(void) = default;

    [[nodiscard]] bool operator==(std::nullptr_t) const { return this->ptr == nullptr; }
    [[nodiscard]] bool operator==(const FField* /*field*/) const { return false; }
    [[nodiscard]] bool operator==(const UObjectType* obj) const { return this->ptr == obj; }
    [[nodiscard]] bool operator==(const TFieldVariantStub& other) const {
        return this->ptr == other.ptr;
    }

    [[nodiscard]] bool is_field(void) const { return false; }
    [[nodiscard]] bool is_obj(void) const { return this->ptr != nullptr; }

    [[nodiscard]] FField* as_field(void) const { return nullptr; }
    [[nodiscard]] UObjectType* as_obj(void) const { return this->ptr; }

    template <typename F>
    void cast(const F& func) const {
        if (this->ptr == nullptr) {
            func.template operator()<std::nullptr_t>(nullptr);
        } else {
            func.template operator()<UObjectType>(this->ptr);
        }
    }
};

using FFieldVariant = TFieldVariant<FField, UObject>;

static_assert(sizeof(FFieldVariant) == sizeof(uintptr_t));
static_assert(alignof(FFieldVariant) == alignof(uintptr_t));

UNREALSDK_UNREAL_STRUCT_PADDING_POP()

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_STRUCTS_TFIELDVARIANT_H */
