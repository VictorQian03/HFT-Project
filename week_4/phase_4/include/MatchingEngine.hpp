#pragma once
#include "MemoryPool.hpp"
#include "OrderBook.hpp"
#include "Order.hpp"
#include "MarketDataFeed.hpp"
#include <string>
#include <iostream>

template <typename PriceType, typename OrderIdType, typename Allocator=MemoryPool>
class MatchingEngine {
public:
    MatchingEngine(OrderBook<PriceType, OrderIdType, Allocator>& ob)
        : orderBook(ob) {}

    // Process incoming market data tick, create orders, and perform matching
    void processMarketData(const MarketData& data) {
        OrderIdType buyOrderId = generateOrderId();
        OrderIdType sellOrderId = generateOrderId();

        // Generate orders based on incoming market data
        orderBook.addOrder(buyOrderId, data.symbol, data.bid_price, defaultQuantity, true);   // Buy order
        orderBook.addOrder(sellOrderId, data.symbol, data.ask_price, defaultQuantity, false); // Sell order
    }

private:
    OrderBook<PriceType, OrderIdType, Allocator>& orderBook;

    static constexpr int defaultQuantity = 100;

    OrderIdType generateOrderId() {
        static OrderIdType currentId = 1;
        return currentId++;
    }
};
