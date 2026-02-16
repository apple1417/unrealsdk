#ifndef UNREALSDK_TPOINTER_H
#define UNREALSDK_TPOINTER_H

#include <array>
#include <cstdint>

namespace unrealsdk {

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
    std::array<std::uint8_t, sizeof(void*)> storage{};

   public:
    TPointer() noexcept { set(nullptr); }
    explicit TPointer(T* ptr) noexcept { set(ptr); }
    ~TPointer() noexcept = default;

    /**
     * @brief safely creates a native aligned pointer and returns it.
     * @return the created pointer.
     */
    T* get() const noexcept {
        void* ptr{};
        std::memcpy(&ptr, storage.data(), storage.size());
        return static_cast<T*>(ptr);
    }

    /**
     * @brief overwrites the stored pointer with the provided.
     * @param ptr the pointer to store
     */
    void set(T* ptr) noexcept { std::memcpy(storage.data(), &ptr, storage.size()); }

    TPointer& operator=(T* ptr) noexcept {
        set(ptr);
        return *this;
    }

    bool operator==(const void* rhs) const noexcept { return get() == rhs; }
    bool operator!=(const void* rhs) const noexcept { return get() != rhs; }

    auto* operator->() const noexcept
        requires(!std::is_void_v<T>)
    {
        return get();
    }

    const auto& operator*() const noexcept
        requires(!std::is_void_v<T>)
    {
        return *get();
    }

    auto& operator*() noexcept
        requires(!std::is_void_v<T>)
    {
        return *get();
    }

    TPointer& operator++() noexcept
        requires(!std::is_void_v<T>)
    {
        set(get() + 1);
        return *this;
    }

    TPointer operator++(int) noexcept
        requires(!std::is_void_v<T>)
    {
        TPointer old_value = *this;
        set(get() + 1);
        return old_value;
    }
};

/**
 * @brief generic alias for void pointers
 */
using Pointer = TPointer<void>;

}  // namespace unrealsdk

#endif
