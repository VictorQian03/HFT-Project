#include "../include/unoptimized_orderbook.h"
#include <iostream>
#include <iomanip>

UnoptimizedOrderBook::UnoptimizedOrderBook() : activeOrderCount(0) {}

UnoptimizedOrderBook::~UnoptimizedOrderBook() {
    std::vector<std::string> ids_to_clear;
    for(const auto& pair : orderLookup) {
        ids_to_clear.push_back(pair.first);
    }
    for(const std::string& id : ids_to_clear) {
        deleteOrder(id);
    }
}

void UnoptimizedOrderBook::addOrder(const std::string& id, double price, int quantity, bool isBuy) {
    if (id.empty() || quantity <= 0 || price <= 0) {
        std::cerr << "Warning: Invalid order parameters for ID " << (id.empty() ? "[EMPTY_ID]" : id) << ". Order not added." << std::endl;
        return;
    }
    if (orderLookup.count(id)) {
        std::cerr << "Warning: Order ID " << id << " already exists. Use modifyOrder or delete first. Order not added." << std::endl;
        return;
    }

    Order newOrder(id, price, quantity, isBuy);
    orderLevels[price][id] = newOrder;
    orderLookup[id] = newOrder;
    activeOrderCount++;
}

void UnoptimizedOrderBook::modifyOrder(const std::string& id, double newPrice, int newQuantity) {
    if (id.empty() || newQuantity <= 0 || newPrice <= 0) {
        std::cerr << "Warning: Invalid parameters for modifying order ID " << (id.empty() ? "[EMPTY_ID]" : id) << ". Modification cancelled." << std::endl;
        return;
    }

    auto it = orderLookup.find(id);
    if (it != orderLookup.end()) {
        Order& oldOrder = it->second;
        double oldPrice = oldOrder.price;

        // Remove from old price level
        orderLevels[oldPrice].erase(id);
        if (orderLevels[oldPrice].empty()) {
            orderLevels.erase(oldPrice);
        }

        // Update order details
        oldOrder.price = newPrice;
        oldOrder.quantity = newQuantity;

        // Add to new price level
        orderLevels[newPrice][id] = oldOrder;
    }
}

void UnoptimizedOrderBook::deleteOrder(const std::string& id) {
    if (id.empty()) {
        return;
    }

    auto it = orderLookup.find(id);
    if (it != orderLookup.end()) {
        const Order& orderToDelete = it->second;
        double price = orderToDelete.price;

        // Remove from price level
        if (orderLevels.count(price)) {
            orderLevels[price].erase(id);
            if (orderLevels[price].empty()) {
                orderLevels.erase(price);
            }
        }

        // Remove from lookup
        orderLookup.erase(it);
        activeOrderCount--;
    }
}

bool UnoptimizedOrderBook::getOrder(const std::string& id, Order& outOrder) const {
    auto it = orderLookup.find(id);
    if (it != orderLookup.end()) {
        outOrder = it->second;
        return true;
    }
    return false;
}

size_t UnoptimizedOrderBook::getActiveOrderCount() const {
    return activeOrderCount;
}

std::vector<double> UnoptimizedOrderBook::getAllActivePrices() const {
    std::vector<double> prices;
    prices.reserve(orderLevels.size());
    for (const auto& [price, _] : orderLevels) {
        prices.push_back(price);
    }
    return prices;
}

const std::map<std::string, Order>* UnoptimizedOrderBook::getOrdersAtPrice(double price) const {
    auto it = orderLevels.find(price);
    if (it != orderLevels.end()) {
        return &(it->second);
    }
    return nullptr;
}

std::pair<double, const std::map<std::string, Order>*> UnoptimizedOrderBook::getBestBidLevel() const {
    for (auto rit = orderLevels.rbegin(); rit != orderLevels.rend(); ++rit) {
        for (const auto& [_, order] : rit->second) {
            if (order.isBuy) {
                return {rit->first, &(rit->second)};
            }
        }
    }
    return {0.0, nullptr};
}

std::pair<double, const std::map<std::string, Order>*> UnoptimizedOrderBook::getBestAskLevel() const {
    for (const auto& [price, orders] : orderLevels) {
        for (const auto& [_, order] : orders) {
            if (!order.isBuy) {
                return {price, &orders};
            }
        }
    }
    return {0.0, nullptr};
}

void UnoptimizedOrderBook::displayFullBook() const {
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "\n--- ASKS (Sell Orders - Price Ascending) --- (Active: " << getActiveOrderCount() << ")" << std::endl;
    bool asksFound = false;
    for (const auto& levelPair : orderLevels) {
        double price = levelPair.first;
        const auto& ordersAtPriceLevel = levelPair.second;
        
        bool hasAsksThisLevel = false;
        for (const auto& orderPair : ordersAtPriceLevel) {
            if (!orderPair.second.isBuy) {
                hasAsksThisLevel = true;
                break;
            }
        }

        if (hasAsksThisLevel) {
            asksFound = true;
            std::cout << "Price: " << price << std::endl;
            for (const auto& orderPair : ordersAtPriceLevel) {
                const Order& order = orderPair.second;
                if (!order.isBuy) {
                    std::cout << "  ID: " << order.id << ", Qty: " << order.quantity << std::endl;
                }
            }
        }
    }
    if (!asksFound) {
        std::cout << "(No sell orders in the book)" << std::endl;
    }

    std::cout << "\n--- BIDS (Buy Orders - Price Descending) ---" << std::endl;
    bool bidsFound = false;
    for (auto rit = orderLevels.rbegin(); rit != orderLevels.rend(); ++rit) {
        double price = rit->first;
        const auto& ordersAtPriceLevel = rit->second;

        bool hasBidsThisLevel = false;
        for (const auto& orderPair : ordersAtPriceLevel) {
            if (orderPair.second.isBuy) {
                hasBidsThisLevel = true;
                break;
            }
        }

        if (hasBidsThisLevel) {
            bidsFound = true;
            std::cout << "Price: " << price << std::endl;
            for (const auto& orderPair : ordersAtPriceLevel) {
                const Order& order = orderPair.second;
                if (order.isBuy) {
                    std::cout << "  ID: " << order.id << ", Qty: " << order.quantity << std::endl;
                }
            }
        }
    }
    if (!bidsFound) {
        std::cout << "(No buy orders in the book)" << std::endl;
    }
    std::cout << "------------------------------------------" << std::endl;
}
