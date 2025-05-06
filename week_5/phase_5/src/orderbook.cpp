#include "../include/orderbook.h"
#include <iostream>
#include <iomanip> 

OrderBook::OrderBook() {
}

void OrderBook::addOrder(const std::string& id, double price, int quantity, bool isBuy) {
    if (id.empty() || quantity <= 0 || price <= 0) {
        std::cerr << "Warning: Invalid order parameters for ID " << (id.empty() ? "[EMPTY_ID]" : id) << ". Order not added." << std::endl;
        return;
    }
    Order order(id, price, quantity, isBuy);
    orderLevels[price][id] = order;
    orderLookup[id] = order;
}

void OrderBook::modifyOrder(const std::string& id, double newPrice, int newQuantity) {
    if (id.empty() || newQuantity <= 0 || newPrice <= 0) {
        std::cerr << "Warning: Invalid parameters for modifying order ID " << (id.empty() ? "[EMPTY_ID]" : id) << ". Modification cancelled." << std::endl;
        return;
    }

    auto it = orderLookup.find(id);
    if (it != orderLookup.end()) {
        Order oldOrder = it->second; 
        if (orderLevels.count(oldOrder.price)) {
            auto& ordersAtOldPrice = orderLevels.at(oldOrder.price);
            ordersAtOldPrice.erase(id);
            if (ordersAtOldPrice.empty()) {
                orderLevels.erase(oldOrder.price);
            }
        } else {
            std::cerr << "Warning: Order ID " << id << " found in lookup, but its price level "
                      << oldOrder.price << " was not found in orderLevels during modify. State might be inconsistent." << std::endl;
        }
        addOrder(id, newPrice, newQuantity, oldOrder.isBuy);
    } else {
        std::cerr << "Warning: Order ID " << id << " not found. Cannot modify." << std::endl;
    }
}

void OrderBook::deleteOrder(const std::string& id) {
    if (id.empty()) {
        std::cerr << "Warning: Empty order ID provided for deletion. Deletion cancelled." << std::endl;
        return;
    }
    auto it = orderLookup.find(id);
    if (it != orderLookup.end()) {
        Order orderToDelete = it->second; 
        if (orderLevels.count(orderToDelete.price)) {
            auto& ordersAtPrice = orderLevels.at(orderToDelete.price);
            ordersAtPrice.erase(id);
            if (ordersAtPrice.empty()) {
                orderLevels.erase(orderToDelete.price);
            }
        } else {
            std::cerr << "Warning: Order ID " << id << " found in lookup, but its price level "
                      << orderToDelete.price << " was not found in orderLevels during delete. State might be inconsistent." << std::endl;
        }

        orderLookup.erase(it);
    } else {
        std::cerr << "Warning: Order ID " << id << " not found. Cannot delete." << std::endl;
    }
}

bool OrderBook::getOrder(const std::string& id, Order& outOrder) const {
    auto it = orderLookup.find(id);
    if (it != orderLookup.end()) {
        outOrder = it->second;
        return true;
    }
    return false;
}

void OrderBook::displayFullBook() const {
    std::cout << std::fixed << std::setprecision(2); 

    std::cout << "\n--- ASKS (Sell Orders - Price Ascending) ---" << std::endl;
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