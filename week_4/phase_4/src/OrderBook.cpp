#include "../include/OrderBook.hpp"
#include <iostream>

template <typename PriceType, typename OrderIdType, typename Allocator>
void OrderBook<PriceType, OrderIdType, Allocator>::addOrder(const OrderIdType& id, const PriceType& price, int quantity, bool is_buy) {
    // Allocate memory for the order from the pool
    void* memory = allocator.allocate();
    auto* order = new (memory) Order<PriceType, OrderIdType>(id, price, quantity, is_buy);

    // Insert the order into the correct multimap (buy or sell)
    auto& targetOrders = is_buy ? bids : asks;
    targetOrders.insert({price, std::unique_ptr<Order<PriceType, OrderIdType>>(order)});
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
                buyOrders.erase(bidsIt);  // Remove from the multimap
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
                sellOrders.erase(asksIt);  // Remove from the multimap
                return true;
            }
            ++asksIt;
        }
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
                bidsIt->second->quantity = new_quantity
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
                asksIt->second->quantity = new_quantity
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
    for (const auto& [price, order] : buyOrders) {
        std::cout << "Order ID: " << order->id << " Price: " << order->price
                  << " Quantity: " << order->quantity << '\n';
    }

    std::cout << "Sell Orders:\n";
    for (const auto& [price, order] : sellOrders) {
        std::cout << "Order ID: " << order->id << " Price: " << order->price
                  << " Quantity: " << order->quantity << '\n';
    }
}