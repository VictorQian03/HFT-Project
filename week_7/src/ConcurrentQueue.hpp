#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cassert>

// Memory‐ordering policy
enum class MemoryPolicy { RAR, SeqCst, Fence };
extern MemoryPolicy g_memory_policy;

inline std::memory_order mo_relaxed() {
    return (g_memory_policy == MemoryPolicy::SeqCst)
           ? std::memory_order_seq_cst
           : std::memory_order_relaxed;
}
inline std::memory_order mo_acquire() {
    return (g_memory_policy == MemoryPolicy::SeqCst)
           ? std::memory_order_seq_cst
           : std::memory_order_acquire;
}
inline std::memory_order mo_release() {
    return (g_memory_policy == MemoryPolicy::SeqCst)
           ? std::memory_order_seq_cst
           : std::memory_order_release;
}

// Lock-free ring buffer
template <typename T, std::size_t Capacity>
class ConcurrentQueue {
public:
    struct Cell {
        std::atomic<std::size_t> seq;
        T                       data;
    };

    static_assert(Capacity > 0, "Capacity must be > 0");

    ConcurrentQueue() : head(0), tail(0) {
        for (std::size_t i = 0; i < Capacity; ++i) {
            buffer[i].seq.store(i, std::memory_order_relaxed);
        }
    }

    bool push(const T& item) {
        Cell* cell;
        std::size_t pos = tail.load(mo_relaxed());
        for (;;) {
            cell = &buffer[pos % Capacity];
            auto seq = cell->seq.load(mo_acquire());
            intptr_t dif = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);
            if (dif == 0) {
                if (tail.compare_exchange_weak(
                        pos, pos+1,
                        mo_relaxed(), mo_relaxed()
                    ))
                {
                    break;
                }
            } else if (dif < 0) {
                return false;  
            } else {
                pos = tail.load(mo_relaxed());
            }
        }

        cell->data = item;
        if (g_memory_policy == MemoryPolicy::Fence) {
            std::atomic_thread_fence(std::memory_order_seq_cst);
        }
        cell->seq.store(pos + 1, mo_release());
        return true;
    }

    bool pop(T& item) {
        Cell* cell;
        std::size_t pos = head.load(mo_relaxed());
        for (;;) {
            cell = &buffer[pos % Capacity];
            auto seq = cell->seq.load(mo_acquire());
            intptr_t dif = static_cast<intptr_t>(seq)
                          - static_cast<intptr_t>(pos + 1);
            if (dif == 0) {
                if (head.compare_exchange_weak(
                        pos, pos+1,
                        mo_relaxed(), mo_relaxed()
                    ))
                {
                    break;
                }
            } else if (dif < 0) {
                return false; 
            } else {
                pos = head.load(mo_relaxed());
            }
        }

        item = cell->data;
        if (g_memory_policy == MemoryPolicy::Fence) {
            std::atomic_thread_fence(std::memory_order_seq_cst);
        }
        cell->seq.store(pos + Capacity, mo_release());
        return true;
    }

    bool is_empty() const {
        auto h = head.load(mo_acquire());
        auto t = tail.load(mo_acquire());
        return (t - h) == 0;
    }

    std::size_t size() const {
        auto h = head.load(mo_acquire());
        auto t = tail.load(mo_acquire());
        return t - h;
    }

    void reset() {
        head.store(0, std::memory_order_relaxed);
        tail.store(0, std::memory_order_relaxed);
        for (std::size_t i = 0; i < Capacity; ++i) {
            buffer[i].seq.store(i, std::memory_order_relaxed);
        }
    }

private:
    std::array<Cell, Capacity> buffer;
    std::atomic<std::size_t>   head;
    std::atomic<std::size_t>   tail;
};