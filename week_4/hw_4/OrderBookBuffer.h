//
// Created by Amy Wang on 4/23/25.
//
#ifndef ORDERBOOKBUFFER_H
#define ORDERBOOKBUFFER_H
#include <cstddef>      
#include <new>         
#include <stdexcept>    
#include <mutex>       
#include <iostream>     
#include <utility>      
#include <type_traits>  
#include <cstring> 

struct NoLock {
    struct Lock {
        explicit Lock(NoLock&) noexcept {}
        ~Lock() noexcept {}
        Lock(const Lock&) = delete;
        Lock& operator=(const Lock&) = delete;
    };
};

struct MutexLock {
    void lock()   const noexcept { mtx_.lock(); }
    void unlock() const noexcept { mtx_.unlock(); }

    struct Lock {
        explicit Lock(MutexLock& m) : policy_(m) { policy_.lock(); }
        ~Lock() noexcept { policy_.unlock(); }
        Lock(const Lock&) = delete;
        Lock& operator=(const Lock&) = delete;
      private:
        MutexLock& policy_;
    };

  private:
    mutable std::mutex mtx_;
};

// ─── Allocator concepts used by OrderBookBuffer ────────────
template<typename T>
struct HeapAllocator {
    T* allocate(std::size_t n) {
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }
    void deallocate(T* p) noexcept {
        ::operator delete(p);
    }
};

template<typename T>
struct StackAllocator {
    explicit StackAllocator(std::size_t cap)
      : buf_(new unsigned char[cap * sizeof(T)]), cap_(cap), used_(0)
    {}
    ~StackAllocator() {
        delete[] buf_;
    }

    StackAllocator(const StackAllocator&) = delete;
    StackAllocator& operator=(const StackAllocator&) = delete;

    T* allocate(std::size_t n) {
        if (used_ + n > cap_) throw std::bad_alloc{};
        T* p = reinterpret_cast<T*>(buf_ + used_ * sizeof(T));
        used_ += n;
        return p;
    }
    void deallocate(T*) noexcept {}

  private:
    unsigned char* buf_;
    std::size_t    cap_;
    std::size_t    used_;
};

template<typename T>
struct ZeroInitAllocator {
    T* allocate(std::size_t n) {
        T* p = static_cast<T*>(::operator new(n * sizeof(T)));
        std::memset(p, 0, n * sizeof(T));
        return p;
    }
    void deallocate(T* p) noexcept {
        ::operator delete(p);
    }
};

template<typename T, typename AllocatorPolicy, typename ThreadPolicy = NoLock>
class OrderBookBuffer {
  public:
    explicit OrderBookBuffer(std::size_t capacity)
      : allocator_( make_allocator(capacity) )
      , thread_policy_()
      , buffer_(nullptr)
      , size_(0)
      , capacity_(capacity)
    {
        if (capacity_ > 0) {
            buffer_ = allocator_.allocate(capacity_);
        }
    }

    ~OrderBookBuffer() {
        for (std::size_t i = size_; i > 0; --i) {
            buffer_[i-1].~T();
        }
        if (buffer_) {
            allocator_.deallocate(buffer_);
        }
    }

    OrderBookBuffer(const OrderBookBuffer&) = delete;
    OrderBookBuffer& operator=(const OrderBookBuffer&) = delete;

    OrderBookBuffer(OrderBookBuffer&& o) noexcept
      : buffer_(o.buffer_)
      , size_(o.size_)
      , capacity_(o.capacity_)
      , allocator_(std::move(o.allocator_))
      , thread_policy_()
    {
        o.buffer_ = nullptr;
        o.size_   = 0;
        o.capacity_ = 0;
    }

    OrderBookBuffer& operator=(OrderBookBuffer&& o) noexcept {
        if (this != &o) {
            for (std::size_t i = size_; i > 0; --i)
                buffer_[i-1].~T();
            if (buffer_) allocator_.deallocate(buffer_);

            allocator_     = std::move(o.allocator_); 
            thread_policy_ = ThreadPolicy();         
            buffer_        = o.buffer_;
            size_          = o.size_;
            capacity_      = o.capacity_;

            o.buffer_   = nullptr;
            o.size_     = 0;
            o.capacity_ = 0;
        }
        return *this;
    }

    bool add_order(const T& order) {
        typename ThreadPolicy::Lock lock(thread_policy_);
        if (size_ >= capacity_) return false;
        new (buffer_ + size_) T(order);
        ++size_;
        return true;
    }

    void print_orders() const {
        typename ThreadPolicy::Lock lock(const_cast<ThreadPolicy&>(thread_policy_));
        for (std::size_t i = 0; i < size_; ++i)
            std::cout << buffer_[i] << "\n";
    }

  private:
    static AllocatorPolicy make_allocator(std::size_t c) {
        if constexpr (std::is_constructible_v<AllocatorPolicy, std::size_t>)
            return AllocatorPolicy(c);
        else
            return AllocatorPolicy();
    }

    AllocatorPolicy allocator_;
    mutable ThreadPolicy    thread_policy_;
    T*              buffer_   = nullptr;
    std::size_t     size_     = 0;
    std::size_t     capacity_ = 0;
};

#endif //ORDERBOOKBUFFER_H
