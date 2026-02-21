#ifndef UNREALSDK_TPOINTER_H
#define UNREALSDK_TPOINTER_H

#include <array>
#include <cstdint>

namespace unrealsdk {

#ifdef UNREALSDK_FEAT_VIRTUAL_POINTER_ALIGNMENT
constexpr auto TPOINTER_ALIGNMENT = UNREALSDK_FEAT_VIRTUAL_POINTER_ALIGNMENT;
#else
constexpr auto TPOINTER_ALIGNMENT = alignof(void*);
#endif

/**
 * @brief Pointer wrapper type that allows for storage of a pointer with a controlled alignment
 * requirement. Only reason for this is because UE3 expects 4 byte aligned pointers on x64 which
 * means we cannot store/use a pointer as that will create an 8 byte alignment requirement.
 * @tparam T The pointer type stored i.e., TPointer<void> stores a void*
 */
template <class T>
class alignas(TPOINTER_ALIGNMENT) TPointer {
    //
    // would prefer if this was just a simple get/set container and not have to do any of the
    // template shenanigans (because I am notoriously shit at writing templates). Might also be a
    // slight overhead for native aligned pointers so would want to avoid using this if possible
    // for when pointers are naturally aligned.
    //
   private:
#ifdef UNREALSDK_FEAT_VIRTUAL_POINTER_ALIGNMENT
    std::array<std::uint8_t, sizeof(void*)> storage{};
#else
    T* storage{};
#endif

   public:
    TPointer() noexcept { set(nullptr); }
    explicit TPointer(T* ptr) noexcept { set(ptr); }
    ~TPointer() noexcept = default;

    /**
     * @brief safely creates a native aligned pointer and returns it.
     * @return the created pointer.
     */
    T* get() const noexcept {
#ifdef UNREALSDK_FEAT_VIRTUAL_POINTER_ALIGNMENT
        void* ptr{};
        std::memcpy(&ptr, storage.data(), storage.size());
        return static_cast<T*>(ptr);
#else
        return storage;
#endif
    }

    /**
     * @brief overwrites the stored pointer with the provided.
     * @param ptr the pointer to store
     */
    void set(T* ptr) noexcept {
#ifdef UNREALSDK_FEAT_VIRTUAL_POINTER_ALIGNMENT
        std::memcpy(storage.data(), &ptr, storage.size());
#else
        storage = ptr;
#endif
    }

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
