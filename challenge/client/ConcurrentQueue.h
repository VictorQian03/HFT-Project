#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cassert>
#include <memory> 

template <typename T, std::size_t Capacity>
class ConcurrentQueue {
public:
    struct Cell {
        std::atomic<std::size_t> seq;
        T data; 
    };

    static_assert(Capacity > 0, "Capacity must be > 0");

    ConcurrentQueue() : head(0), tail(0) {
        for (std::size_t i = 0; i < Capacity; ++i) {
            buffer[i].seq.store(i, std::memory_order_relaxed);
        }
    }

    bool push(const T& item) {
        Cell* cell;
        std::size_t pos = tail.load(std::memory_order_relaxed);
        for (;;) {
            cell = &buffer[pos % Capacity];
            auto seq = cell->seq.load(std::memory_order_acquire);
            intptr_t dif = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);

            if (dif == 0) { 
                if (tail.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed, std::memory_order_relaxed)) {
                    break; 
                }
            } else if (dif < 0) { 
                return false;
            } else { 
                pos = tail.load(std::memory_order_relaxed);
            }
        }

        cell->data = item; 
        cell->seq.store(pos + 1, std::memory_order_release); 
        return true;
    }

    bool pop(T& item) {
        Cell* cell;
        std::size_t pos = head.load(std::memory_order_relaxed);
        for (;;) {
            cell = &buffer[pos % Capacity];
            auto seq = cell->seq.load(std::memory_order_acquire);
            intptr_t dif = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);

            if (dif == 0) { 
                if (head.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed, std::memory_order_relaxed)) {
                    break; 
                }
            } else if (dif < 0) { 
                return false; 
            } else { 
                pos = head.load(std::memory_order_relaxed);
            }
        }

        item = cell->data; 
        cell->seq.store(pos + Capacity, std::memory_order_release); 
        return true;
    }

    bool is_empty() const {
        return head.load(std::memory_order_acquire) == tail.load(std::memory_order_acquire);
    }

    std::size_t size() const {
        std::size_t current_tail = tail.load(std::memory_order_acquire);
        std::size_t current_head = head.load(std::memory_order_acquire);
        return (current_tail >= current_head) ? (current_tail - current_head) : (Capacity - (current_head - current_tail));
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
    alignas(64) std::atomic<std::size_t> head; 
    alignas(64) std::atomic<std::size_t> tail;
};