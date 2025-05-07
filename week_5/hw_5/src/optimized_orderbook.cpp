#include "../include/optimized_orderbook.h"
#include <iostream>
#include <iomanip>
#include <vector> 

OptimizedOrderBook::OptimizedOrderBook(size_t initial_pool_capacity)
    : activeOrderCount(0) {
    orderPool.reserve(initial_pool_capacity);
}

OptimizedOrderBook::~OptimizedOrderBook() {
    std::vector<std::string> ids_to_clear;
    ids_to_clear.reserve(orderLookup.size()); 
    for(const auto& pair : orderLookup) {
        ids_to_clear.push_back(pair.first);
    }
    for(const std::string& id : ids_to_clear) {
        deleteOrder(id);
    }
}


void OptimizedOrderBook::addOrder(const std::string& id, double price, int quantity, bool isBuy) {
    if (id.empty() || quantity <= 0 || price <= 0) {
        std::cerr << "Warning: Invalid order parameters for ID " << (id.empty() ? "[EMPTY_ID]" : id) << ". Order not added." << std::endl;
        return;
    }
    if (orderLookup.count(id)) {
        std::cerr << "Warning: Order ID " << id << " already exists. Use modifyOrder or delete first. Order not added." << std::endl;
        return;
    }

    try {
        OrderOpt* newOrder = orderPool.allocate(id, price, quantity, isBuy);

        orderLevels[price][id] = newOrder;

        orderLookup[id] = newOrder;

        activeOrderCount.fetch_add(1, std::memory_order_relaxed);

    } catch (const std::runtime_error& e) {
        std::cerr << "Error allocating memory for order " << id << ": " << e.what() << std::endl;
        throw; 
    }
}

void OptimizedOrderBook::modifyOrder(const std::string& id, double newPrice, int newQuantity) {
    if (id.empty() || newQuantity <= 0 || newPrice <= 0) {
        std::cerr << "Warning: Invalid parameters for modifying order ID " << (id.empty() ? "[EMPTY_ID]" : id) << ". Modification cancelled." << std::endl;
        return;
    }

    auto it = orderLookup.find(id);
    if (it != orderLookup.end()) {
        OrderOpt* orderToModify = it->second;
        double oldPrice = orderToModify->price;

        if (oldPrice != newPrice) {
            auto level_it = orderLevels.find(oldPrice);
            if (level_it != orderLevels.end()) {
                 auto& ordersAtOldPrice = level_it->second;
                 ordersAtOldPrice.erase(id);
                 if (ordersAtOldPrice.empty()) {
                     orderLevels.erase(level_it);
                 }
            } else {
                 std::cerr << "Warning: Inconsistency! Order ID " << id << " found in lookup with price "
                           << oldPrice << ", but this price level was not found in orderLevels during modify (price change)." << std::endl;
            }

            orderToModify->price = newPrice;

            orderLevels[newPrice][id] = orderToModify;
        }

        orderToModify->quantity = newQuantity;

    } else {
        std::cerr << "Warning: Attempted to modify non-existent order with ID: " << id << std::endl;
    }
}

void OptimizedOrderBook::deleteOrder(const std::string& id) {
     if (id.empty()) {
        return;
    }
    auto it = orderLookup.find(id);
    if (it != orderLookup.end()) {
        OrderOpt* orderToDeletePtr = it->second;
        double price = orderToDeletePtr->price;

        auto level_it = orderLevels.find(price);
        if (level_it != orderLevels.end()) {
            auto& ordersAtPrice = level_it->second;
            ordersAtPrice.erase(id);
            if (ordersAtPrice.empty()) {
                orderLevels.erase(level_it);
            }
        } else {
             std::cerr << "Warning: Inconsistency! Order ID " << id << " found in lookup, but its price level "
                      << price << " was not found in orderLevels during delete." << std::endl;
        }

        orderPool.deallocate(orderToDeletePtr);

        orderLookup.erase(it);

        activeOrderCount.fetch_sub(1, std::memory_order_relaxed);

    } else {
         std::cerr << "Warning: Attempted to delete non-existent order with ID: " << id << std::endl;
    }
}

bool OptimizedOrderBook::getOrder(const std::string& id, OrderOpt& outOrder) const {
    auto it = orderLookup.find(id);
    if (it != orderLookup.end()) {
        outOrder = *(it->second);
        return true;
    }
    return false;
}

std::vector<double> OptimizedOrderBook::getAllActivePrices() const {
    std::vector<double> prices;
    prices.reserve(orderLevels.size()); 
    for(const auto& level_pair : orderLevels) {
        if (!level_pair.second.empty()) {
           prices.push_back(level_pair.first);
        }
    }
    return prices;
}

const std::unordered_map<std::string, OrderOpt*>* OptimizedOrderBook::getOrdersAtPrice(double price) const {
    auto it = orderLevels.find(price);
    if (it != orderLevels.end()) {
        return &(it->second);
    }
    return nullptr;
}

std::pair<double, const std::unordered_map<std::string, OrderOpt*>*> OptimizedOrderBook::getBestBidLevel() const {
    for (auto rit = orderLevels.rbegin(); rit != orderLevels.rend(); ++rit) {
        const auto& ordersAtPriceLevel = rit->second;
        for (const auto& orderPair : ordersAtPriceLevel) {
            if (orderPair.second->isBuy) {
                return {rit->first, &(rit->second)};
            }
        }
    }
    return {0.0, nullptr}; 
}

std::pair<double, const std::unordered_map<std::string, OrderOpt*>*> OptimizedOrderBook::getBestAskLevel() const {
    for (auto it = orderLevels.begin(); it != orderLevels.end(); ++it) {
        const auto& ordersAtPriceLevel = it->second;
        for (const auto& orderPair : ordersAtPriceLevel) {
            if (!orderPair.second->isBuy) {
                return {it->first, &(it->second)};
            }
        }
    }
    return {0.0, nullptr}; 
}

size_t OptimizedOrderBook::getActiveOrderCount() const {
    return activeOrderCount.load(std::memory_order_relaxed);
}

size_t OptimizedOrderBook::getPoolUsedCount() const {
    return orderPool.used_count();
}

size_t OptimizedOrderBook::getPoolFreeCount() const {
    return orderPool.free_count();
}

void OptimizedOrderBook::displayFullBook() const {
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "\n--- ASKS (Sell Orders - Price Ascending) --- (Active: " << getActiveOrderCount() << ")" << std::endl;
    bool asksFoundOverall = false;
    for (const auto& levelPair : orderLevels) {
        double price = levelPair.first;
        const auto& ordersAtPriceLevel = levelPair.second;

        bool hasAsksThisLevel = false;
        for (const auto& orderPair : ordersAtPriceLevel) {
            if (!orderPair.second->isBuy) { 
                hasAsksThisLevel = true;
                break;
            }
        }

        if (hasAsksThisLevel) {
            asksFoundOverall = true;
            std::cout << "Price: " << price << std::endl;
            for (const auto& orderPair : ordersAtPriceLevel) {
                const OrderOpt* order = orderPair.second;
                if (!order->isBuy) {
                    std::cout << "  ID: " << order->id << ", Qty: " << order->quantity << std::endl;
                }
            }
        }
    }
    if (!asksFoundOverall) {
        std::cout << "(No sell orders in the book)" << std::endl;
    }

    std::cout << "\n--- BIDS (Buy Orders - Price Descending) ---" << std::endl;
    bool bidsFoundOverall = false;

    for (auto rit = orderLevels.rbegin(); rit != orderLevels.rend(); ++rit) {
        double price = rit->first;
        const auto& ordersAtPriceLevel = rit->second;

        bool hasBidsThisLevel = false;
        for (const auto& orderPair : ordersAtPriceLevel) {
            if (orderPair.second->isBuy) { 
                hasBidsThisLevel = true;
                break;
            }
        }

        if (hasBidsThisLevel) {
            bidsFoundOverall = true;
            std::cout << "Price: " << price << std::endl;
            for (const auto& orderPair : ordersAtPriceLevel) {
                const OrderOpt* order = orderPair.second;
                if (order->isBuy) {
                    std::cout << "  ID: " << order->id << ", Qty: " << order->quantity << std::endl;
                }
            }
        }
    }
    if (!bidsFoundOverall) {
        std::cout << "(No buy orders in the book)" << std::endl;
    }
    std::cout << "Pool Status: Used=" << getPoolUsedCount() << ", Free=" << getPoolFreeCount() << std::endl;
    std::cout << "------------------------------------------" << std::endl;
}