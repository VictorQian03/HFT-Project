//
// Created by Amy Wang on 4/23/25.
//

#ifndef ORDERBOOKBUFFER_H
#define ORDERBOOKBUFFER_H
// ─── Allocator concepts used by OrderBookBuffer ────────────
template<typename T>
struct HeapAllocator {
    static T* allocate(std::size_t n)            { return static_cast<T*>(::operator new(n*sizeof(T))); }
    static void deallocate(T* p)                 { ::operator delete(p); }
};

template<typename T>
struct StackAllocator {
    explicit StackAllocator(std::size_t n) : buf_(new unsigned char[n*sizeof(T)]), cap_(n) {}
    ~StackAllocator()                     { delete[] buf_; }

    T* allocate(std::size_t n)            {
        if (used_+n > cap_) throw std::bad_alloc{};
        T* p = reinterpret_cast<T*>(buf_ + used_*sizeof(T));
        used_ += n;
        return p;
    }
    static void deallocate(T*) {}

private:
    unsigned char* buf_;
    std::size_t    cap_;
    std::size_t    used_ = 0;
};

template <typename T, typename AllocationPolicy, typename ThreadingPolicy>
class OrderBookBuffer {
    T* buffer = nullptr;
    std::size_t size = 0;
    std::size_t capacity = 0;


public:
    OrderBookBuffer(std::size_t cap) {
        typename ThreadingPolicy::Lock lock;
        buffer = AllocationPolicy::allocate(cap);
        capacity = cap;
    }

    ~OrderBookBuffer() {
        typename ThreadingPolicy::Lock lock;
        AllocationPolicy::deallocate(buffer);
    }

    bool add_order(const T& order) {
        typename ThreadingPolicy::Lock lock;
        if (size < capacity) {
            buffer[size++] = order;
            return true;
        }
        return false;
    }

    void print_orders() const {
        for (std::size_t i = 0; i < size; ++i) {
            std::cout << buffer[i] << '\n';
        }
    }



};
struct NoLock {
    struct Lock {
        void lock()   noexcept {}
        void unlock() noexcept {}
    };
};

struct MutexLock {
    struct Lock {
        void lock()   noexcept { m_.lock(); }
        void unlock() noexcept { m_.unlock(); }
    private:
        std::mutex m_;
    };
};
#endif //ORDERBOOKBUFFER_H
