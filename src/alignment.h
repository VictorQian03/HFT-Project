#ifndef ALIGNMENT_H
#define ALIGNMENT_H

#include <cstdlib>
#include <memory>
#include <stdexcept>

template <typename T, std::size_t Alignment = 64>
class AlignedAllocator {
public:
    using value_type = T;

    AlignedAllocator() noexcept = default;

    template <typename U>
    AlignedAllocator(const AlignedAllocator<U, Alignment>&) noexcept {}

    T* allocate(std::size_t n) {
        void* ptr = nullptr;
        if (posix_memalign(&ptr, Alignment, n * sizeof(T)) != 0) {
            throw std::bad_alloc();
        }
        return static_cast<T*>(ptr);
    }

    void deallocate(T* ptr, std::size_t) noexcept {
        std::free(ptr);
    }

    template <typename U>
    struct rebind {
        using other = AlignedAllocator<U, Alignment>;
    };

    bool operator==(const AlignedAllocator&) const noexcept { return true; }
    bool operator!=(const AlignedAllocator&) const noexcept { return false; }
};

#endif // ALIGNMENT_H
