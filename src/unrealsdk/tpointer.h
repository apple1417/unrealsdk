#ifndef UNREALSDK_TPOINTER_H
#define UNREALSDK_TPOINTER_H

#include <array>
#include <cstdint>

namespace unrealsdk {

constexpr auto TPOINTER_SIZE = sizeof(void*);

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW || UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW64
constexpr auto TPOINTER_ALIGNMENT = 4;
#elif UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK
constexpr auto TPOINTER_ALIGNMENT = TPOINTER_SIZE;
#endif

/**
 * @brief Pointer wrapper type that allows for storage of a pointer with a controlled alignment
 * requirement. Only reason for this is because UE3 expects 4 byte aligned pointers on x64 which
 * means we cannot store/use a pointer as that will create an 8 byte alignment requirement.
 * @tparam T The pointer type stored i.e., TPointer<void> stores a void*
 * @tparam Alignment The alignment requirement for the pointer storage
 * @note not married to the name
 */
template <class T, auto Alignment = TPOINTER_ALIGNMENT>
class alignas(Alignment) TPointer {
    //
    // would prefer if this was just a simple get/set container and not have to do any of the
    // template shenanigans (because I am notoriously shit at writing templates)
    //
   private:
    std::array<std::uint8_t, TPOINTER_SIZE> block{};

   public:
    TPointer() noexcept { set(nullptr); }
    TPointer(std::nullptr_t) noexcept { set(nullptr); }
    TPointer(const T* ptr) noexcept { set(ptr); }
    ~TPointer() noexcept = default;

    /**
     * @brief safely creates a native aligned pointer and returns it.
     * @return the created pointer.
     */
    T* get() const noexcept {
        void* ptr{};
        std::memcpy(&ptr, block.data(), block.size());
        return static_cast<T*>(ptr);
    }

    /**
     * @brief overwrites the stored pointer with the provided.
     * @param ptr the pointer to store
     */
    void set(const T* ptr) noexcept { std::memcpy(block.data(), &ptr, block.size()); }

    TPointer& operator=(const T* ptr) noexcept {
        set(ptr);
        return *this;
    }

    bool operator==(std::nullptr_t /*tag*/) const noexcept { return get() == nullptr; }
    bool operator!=(std::nullptr_t /*tag*/) const noexcept { return get() != nullptr; }
    bool operator==(const void* rhs) const noexcept { return get() == rhs; }
    bool operator!=(const void* rhs) const noexcept { return get() != rhs; }

    T* operator->() const noexcept { return get(); }

    template <class U = T>
        requires(!std::is_void_v<U>)
    const U& operator*() const noexcept {
        return *get();
    }

    template <class U = T>
        requires(!std::is_void_v<U>)
    U& operator*() noexcept {
        return *get();
    }

    TPointer operator++() noexcept {
        set(get() + 1);
        return *this;
    }

    TPointer operator++(int /*tag*/) noexcept {
        TPointer old_value = *this;
        set(get() + 1);
        return old_value;
    }
};

using Pointer = TPointer<void>;

}  // namespace unrealsdk

#endif
