#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <type_traits>
#include <cstddef>
#include "Order.hpp"
#include <iostream>
#include "MatchingEngine.hpp"


// Templated memory pool using aligned storage
template <typename T>
class ObjectPool {
    std::vector<std::aligned_storage_t<sizeof(T), alignof(T)>> storage;
    std::vector<bool> in_use;

public:
    explicit ObjectPool(size_t capacity)
        : storage(capacity), in_use(capacity, false) {}

    template <typename... Args>
    T* allocate(Args&&... args) {
        for (size_t i = 0; i < storage.size(); ++i) {
            if (!in_use[i]) {
                in_use[i] = true;
                void* addr = &storage[i];
                return new (addr) T(std::forward<Args>(args)...);
            }
        }
        return nullptr; // Pool exhausted
    }

    void deallocate(T* obj) {
        if (!obj) return;
        // Explicitly call destructor
        obj->~T();
        // Compute index
        auto base = reinterpret_cast<uint8_t*>(storage.data());
        auto ptr  = reinterpret_cast<uint8_t*>(obj);
        size_t idx = (ptr - base) / sizeof(storage[0]);
        in_use[idx] = false;
    }
};

// OrderBook class template
template <typename PriceType, typename OrderIdType>
class OrderBook {
    // Memory pool for Order objects
    ObjectPool<Order<PriceType, OrderIdType>> pool;

    // Custom deleter invoking pool deallocate
    struct OrderDeleter {
        ObjectPool<Order<PriceType, OrderIdType>>* pool_ptr;
        void operator()(Order<PriceType, OrderIdType>* ptr) const {
            pool_ptr->deallocate(ptr);
        }
    };

    using OrderPtr = std::unique_ptr<Order<PriceType, OrderIdType>, OrderDeleter>;

    // Bids sorted highest price first, asks sorted lowest price first
    std::multimap<PriceType, OrderPtr, std::greater<PriceType>> bids;
    std::multimap<PriceType, OrderPtr> asks;

public:
    explicit OrderBook(size_t capacity)
        : pool(capacity) {}

    // Add a new order to the book
    bool addOrder(OrderIdType id, const std::string& sym, PriceType price, int qty, bool is_buy) {
        // Allocate from pool
        auto* raw = pool.allocate(id, sym, price, qty, is_buy);
        if (!raw) return false;
        // Wrap in unique_ptr with custom deleter
        OrderPtr uptr(raw, OrderDeleter{&pool});

        if (is_buy) {
            bids.emplace(price, std::move(uptr));
        } else {
            asks.emplace(price, std::move(uptr));
        }
        return true;
    }

    // Cancel an order by scanning both sides (could optimize with id-index)
    bool cancelOrder(OrderIdType id) {
        auto remove_from = [&](auto& side) {
            for (auto it = side.begin(); it != side.end(); ++it) {
                if (it->second->id == id) {
                    side.erase(it);
                    return true;
                }
            }
            return false;
        };
        return remove_from(bids) || remove_from(asks);
    }

    // Access top-of-book prices
    std::optional<PriceType> bestBid() const {
        if (bids.empty()) return std::nullopt;
        return bids.begin()->first;
    }
    std::optional<PriceType> bestAsk() const {
        if (asks.empty()) return std::nullopt;
        return asks.begin()->first;
    }

    void matchTop() { MatchingEngine::matchTop(bids, asks); }

     void printBook() const {
        std::cout << "--- Order Book ---\n";
        std::cout << "Asks:\n";
        for (const auto& [price, order] : asks) {
            std::cout << "  ID:" << order->id
                      << " " << order->symbol
                      << " qty=" << order->quantity
                      << " price=" << price << "\n";
        }
        std::cout << "Bids:\n";
        for (const auto& [price, order] : bids) {
            std::cout << "  ID:" << order->id
                      << " " << order->symbol
                      << " qty=" << order->quantity
                      << " price=" << price << "\n";
        }
    }
};
