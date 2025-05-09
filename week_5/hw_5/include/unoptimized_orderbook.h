#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <vector>

struct Order {
    std::string id;
    double price;
    int quantity;
    bool isBuy;

    // Default constructor
    Order() : price(0.0), quantity(0), isBuy(false) {}

    // Parameterized constructor
    Order(const std::string& id, double price, int quantity, bool isBuy)
        : id(id), price(price), quantity(quantity), isBuy(isBuy) {}
};

class UnoptimizedOrderBook {
public:
    UnoptimizedOrderBook();
    ~UnoptimizedOrderBook();

    void addOrder(const std::string& id, double price, int quantity, bool isBuy);
    void modifyOrder(const std::string& id, double newPrice, int newQuantity);
    void deleteOrder(const std::string& id);
    bool getOrder(const std::string& id, Order& outOrder) const;
    size_t getActiveOrderCount() const;
    void displayFullBook() const;

    // Additional methods for benchmarking
    std::vector<double> getAllActivePrices() const;
    const std::map<std::string, Order>* getOrdersAtPrice(double price) const;
    std::pair<double, const std::map<std::string, Order>*> getBestBidLevel() const;
    std::pair<double, const std::map<std::string, Order>*> getBestAskLevel() const;

private:
    // Using ordered map for price levels
    std::map<double, std::map<std::string, Order>> orderLevels;
    // Using unordered map for quick order lookup
    std::unordered_map<std::string, Order> orderLookup;
    size_t activeOrderCount;
}; 