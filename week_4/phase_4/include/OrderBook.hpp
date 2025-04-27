#pragma once

#include "Order.hpp"
#include "MemoryPool.hpp"
#include <memory>
#include <map>

template <typename PriceType, typename OrderIdType, typename Allocator = MemoryPool> //hardcode MemoryPool allocator
class OrderBook {
private:
    using OrderPtr = std::unique_ptr<Order<PriceType, OrderIdType>>;

    std::multimap<PriceType, OrderPtr> bids;
    std::multimap<PriceType, OrderPtr> asks;
    Allocator allocator;

public:
    void addOrder(const OrderIdType& id, const PriceType& price, int quantity, bool is_buy);
    bool deleteOrder(const OrderIdType& id, bool is_buy);
    bool updateQuantity(const OrderIdType& id, int new_quantity, bool is_buy);
    void printOrders() const;
};
