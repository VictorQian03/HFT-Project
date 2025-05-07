#ifndef OPTIMIZED_ORDER_BOOK_H
#define OPTIMIZED_ORDER_BOOK_H

#include <string>
#include <map>
#include <unordered_map>
#include <utility>
#include <atomic>
#include "object_pool.h"
#include <vector>

class OptimizedOrderBook;

struct OrderOpt {
    std::string id;
    double price;
    int quantity;
    bool isBuy; 

    OrderOpt(std::string oid = "", double p = 0.0, int q = 0, bool side = true)
        : id(std::move(oid)), price(p), quantity(q), isBuy(side) {}

    OrderOpt(const OrderOpt& other) = default;

    OrderOpt& operator=(const OrderOpt& other) {
        if (this != &other) {
            id = other.id;
            price = other.price;
            quantity = other.quantity;
            isBuy = other.isBuy;
        }
        return *this;
    }
};

class OptimizedOrderBook {
private:
    std::map<double, std::unordered_map<std::string, OrderOpt*>> orderLevels;

    std::unordered_map<std::string, OrderOpt*> orderLookup;

    ObjectPool<OrderOpt> orderPool;

    std::atomic<size_t> activeOrderCount;

public:
    explicit OptimizedOrderBook(size_t initial_pool_capacity);

    ~OptimizedOrderBook();

    void addOrder(const std::string& id, double price, int quantity, bool isBuy);
    void modifyOrder(const std::string& id, double newPrice, int newQuantity);
    void deleteOrder(const std::string& id);

    bool getOrder(const std::string& id, OrderOpt& outOrder) const;

    std::vector<double> getAllActivePrices() const;

    const std::unordered_map<std::string, OrderOpt*>* getOrdersAtPrice(double price) const;

    std::pair<double, const std::unordered_map<std::string, OrderOpt*>*> getBestBidLevel() const;

    std::pair<double, const std::unordered_map<std::string, OrderOpt*>*> getBestAskLevel() const;

    size_t getActiveOrderCount() const;
    size_t getPoolUsedCount() const;
    size_t getPoolFreeCount() const;

    void displayFullBook() const;
};

#endif 