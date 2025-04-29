#pragma once

#include "Order.hpp"
#include "MemoryPool.hpp"
#include <iostream>
#include <memory>
#include <map>
#include <unordered_map>
#include <string>

template <typename PriceType,
          typename OrderIdType,
          typename Allocator = MemoryPool>
class OrderBook {
public:
    using OrderPtr = std::shared_ptr<Order<PriceType, OrderIdType>>;

private:
    struct OrderDeleter {
        Allocator* pool_allocator;
        explicit OrderDeleter(Allocator* alloc) : pool_allocator(alloc) {}

        void operator()(Order<PriceType, OrderIdType>* ptr) const {
            if (ptr && pool_allocator) {
                ptr->~Order<PriceType, OrderIdType>();
                pool_allocator->deallocate(ptr);
            }
        }
    };

    Allocator                                              allocator_;
    std::multimap<PriceType, OrderPtr, std::greater<PriceType>> bids_;
    std::multimap<PriceType, OrderPtr>                     asks_;
    std::unordered_map<OrderIdType, OrderPtr>              ordersById_;

public:
    explicit OrderBook(std::size_t blockSize, std::size_t poolSize)
        : allocator_(blockSize, poolSize) {}

    OrderPtr addOrder(const OrderIdType& id,
                      const std::string& symbol,
                      const PriceType& price,
                      int quantity,
                      bool is_buy)
    {
        if (ordersById_.count(id)) {
            std::cerr << "Error: Order ID " << id << " already exists.\n";
            return nullptr;
        }

        void* mem = nullptr;
        try {
            mem = allocator_.allocate();
        } catch (const std::bad_alloc&) {
            std::cerr << "Error: Memory pool exhausted.\n";
            return nullptr;
        }

        auto raw = new (mem) Order<PriceType, OrderIdType>(
            id, symbol, price, quantity, is_buy
        );

        OrderPtr ptr(raw, OrderDeleter(&allocator_));
        ordersById_.emplace(id, ptr);

        if (is_buy)      bids_.emplace(price, ptr);
        else             asks_.emplace(price, ptr);

        return ptr;
    }

    bool deleteOrder(const OrderIdType& id) {
        auto it = ordersById_.find(id);
        if (it == ordersById_.end()) return false;

        auto ptr    = it->second;
        auto price  = ptr->price;
        auto is_buy = ptr->is_buy;

        ordersById_.erase(it);

        if (is_buy) {
            auto range = bids_.equal_range(price);
            for (auto itr = range.first; itr != range.second; ++itr) {
                if (itr->second == ptr) {
                    bids_.erase(itr);
                    break;
                }
            }
        } else {
            auto range = asks_.equal_range(price);
            for (auto itr = range.first; itr != range.second; ++itr) {
                if (itr->second == ptr) {
                    asks_.erase(itr);
                    break;
                }
            }
        }
        return true;
    }

    bool updateQuantity(const OrderIdType& id, int new_quantity) {
        auto it = ordersById_.find(id);
        if (it == ordersById_.end() || new_quantity <= 0) {
            return false;
        }
        it->second->quantity = new_quantity;
        return true;
    }

    OrderPtr getOrderById(const OrderIdType& id) const {
        auto it = ordersById_.find(id);
        return (it != ordersById_.end() ? it->second : nullptr);
    }

    void printOrders(std::ostream& os = std::cout) const {
        os << "--- Order Book ---\n"
           << "Asks:\n";
        for (auto const& [px, order] : asks_) {
            os << "  ID:" << order->id
               << " Px:" << order->price
               << " Qty:" << order->quantity << "\n";
        }
        os << "Bids:\n";
        for (auto const& [px, order] : bids_) {
            os << "  ID:" << order->id
               << " Px:" << order->price
               << " Qty:" << order->quantity << "\n";
        }
        os << "------------------\n";
    }

    const auto& getBids() const { return bids_; }
    const auto& getAsks() const { return asks_; }
};