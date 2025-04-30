#pragma once
#include "MemoryPool.hpp"
#include "OrderBook.hpp"
#include "Order.hpp"
#include "MarketDataFeed.hpp"
#include <string>
#include <iostream>
#include <algorithm>

template <typename PriceType, typename OrderIdType, typename Allocator=MemoryPool>
class MatchingEngine {
public:
    MatchingEngine(OrderBook<PriceType, OrderIdType, Allocator>& ob);
    
    void processMarketData(const MarketData& data);
    void matchOrders();

private:
    OrderBook<PriceType, OrderIdType, Allocator>& orderBook;
    static constexpr int defaultQuantity = 20;
    OrderIdType generateOrderId();
};
