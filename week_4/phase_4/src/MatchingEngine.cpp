// src/matchingengine.cpp

#include "../include/OrderBook.hpp"
#include "../include/Order.hpp"
#include "../include/MarketDataFeed.hpp"
#include "../include/MemoryPool.hpp"
#include <iostream>


template <typename PriceType, typename OrderIdType, typename Allocator>
class MatchingEngine {
public:
    explicit MatchingEngine(OrderBook<PriceType, OrderIdType, Allocator>& ob)
        : orderBook(ob) {}

    // Called on each market-data tick
    void processMarketData(const MarketData& data) {
        // Generate two orders per tick as an example
        OrderIdType buyOrderId  = generateOrderId();
        OrderIdType sellOrderId = generateOrderId();

        orderBook.addOrder(buyOrderId,  data.symbol, data.bid_price, defaultQuantity,  true);  // Buy
        orderBook.addOrder(sellOrderId, data.symbol, data.ask_price, defaultQuantity, false);  // Sell
    }

private:
    OrderBook<PriceType, OrderIdType, Allocator>& orderBook;

    static constexpr int defaultQuantity = 20;
    OrderIdType generateOrderId() {
        static OrderIdType currentId = 1;
        return currentId++;
    }
};

template class MatchingEngine<double, int, MemoryPool>;
