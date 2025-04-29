#pragma once

#include "Order.hpp"
#include "MemoryPool.hpp"
#include "OrderManager.hpp"
#include <iostream>
#include <memory>
#include <map>

template <typename PriceType, typename OrderIdType, typename Allocator = MemoryPool> //hardcode MemoryPool allocator
class OrderBook {
private:
    using OrderPtr = std::unique_ptr<Order<PriceType, OrderIdType>>;

    std::multimap<PriceType, OrderPtr> bids;
    std::multimap<PriceType, OrderPtr> asks;
    Allocator allocator;

    std::unique_ptr<OrderManager<PriceType, OrderIdType>> manager = std::make_unique<OrderManager<PriceType, OrderIdType>>();

public:
    OrderBook(std::size_t blockSize, std::size_t poolSize)
        : allocator(blockSize, poolSize) {}
    void addOrder(const OrderIdType& id,const std::string& symbol, const PriceType& price, int quantity, bool is_buy);
    bool deleteOrder(const OrderIdType& id, bool is_buy);
    bool updateQuantity(const OrderIdType& id, int new_quantity, bool is_buy);
    void printOrders() const;

};

// need to implement in .hpp for templates, since we need the code at compile time!!!

template <typename PriceType, typename OrderIdType, typename Allocator>
void OrderBook<PriceType, OrderIdType, Allocator>::addOrder(const OrderIdType& id,const std::string& symbol,  const PriceType& price, int quantity, bool is_buy) {
    // Allocate memory for the order from the pool
    void* memory = allocator.allocate();
    auto* order = new (memory) Order<PriceType, OrderIdType>(id, symbol,price, quantity, is_buy);

    // Insert the order into the correct multimap (buy or sell)
    auto& targetOrders = is_buy ? bids : asks;
    targetOrders.insert({price, std::unique_ptr<Order<PriceType, OrderIdType>>(order)});

    // Communicate with OrderManager
    try {
        if (!manager->addOrder(id, std::shared_ptr<Order<PriceType, OrderIdType>>(order))) {
            throw std::exception();
        }
    }
    catch (std::exception e) {
        std::cout << "unable to add order to OrderManager";
    }

}

template <typename PriceType, typename OrderIdType, typename Allocator>
bool OrderBook<PriceType, OrderIdType, Allocator>::deleteOrder(const OrderIdType& id, bool is_buy) {
    // Try to find order in the bids map
    if (is_buy){
        auto bidsIt = bids.begin();
        while (bidsIt != bids.end()) {
            if (bidsIt->second->id == id) {
                // Found the order, remove it and return memory to the pool
                allocator.deallocate(bidsIt->second.release()); // Release the unique_ptr and deallocate memory
                bids.erase(bidsIt);  // Remove from the multimap
                return true;
            }
            ++bidsIt;
        }
    }

    // Try to find order in the asks map
    else {
        auto asksIt = asks.begin();
        while (asksIt != asks.end()) {
            if (asksIt->second->id == id) {
                // Found the order, remove it and return memory to the pool
                allocator.deallocate(asksIt->second.release()); // Release the unique_ptr and deallocate memory
                asks.erase(asksIt);  // Remove from the multimap
                return true;
            }
            ++asksIt;
        }
    }

    // Communicate with OrderManager
    try {
        if (!manager->cancelOrder(id)) {
            throw std::exception();
        }
    }
    catch (std::exception e) {
        std::cout << "unable to cancel order in OrderManager";
    }


    // order not found
    return false;
}

template <typename PriceType, typename OrderIdType, typename Allocator>
bool OrderBook<PriceType, OrderIdType, Allocator>::updateQuantity(const OrderIdType& id, int new_quantity, bool is_buy) {
    // Try to find order in the bids map
    if (is_buy){
        auto bidsIt = bids.begin();
        while (bidsIt != bids.end()) {
            if (bidsIt->second->id == id) {
                // Found the order
                bidsIt->second->quantity = new_quantity;
                return true;
            }
            ++bidsIt;
        }
    }


    // Try to find order in the asks map
    else {
        auto asksIt = asks.begin();
        while (asksIt != asks.end()) {
            if (asksIt->second->id == id) {
                asksIt->second->quantity = new_quantity;
                return true;
            }
            ++asksIt;
        }
    }

    // order not found
    return false;
}

template <typename PriceType, typename OrderIdType, typename Allocator>
void OrderBook<PriceType, OrderIdType, Allocator>::printOrders() const {
    std::cout << "Buy Orders:\n";
    for (const auto& [price, order] : bids) {
        std::cout << "Order ID: " << order->id << " Price: " << order->price
                  << " Quantity: " << order->quantity << " State: " << manager->getState(order->id) << '\n';
    }

    std::cout << "Sell Orders:\n";
    for (const auto& [price, order] : asks) {
        std::cout << "Order ID: " << order->id << " Price: " << order->price
                  << " Quantity: " << order->quantity << " State: " << manager->getState(order->id) << '\n';
    }
}