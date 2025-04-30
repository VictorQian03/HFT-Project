#include "../include/MatchingEngine.hpp"
#include "MemoryPool.hpp"
#include "OrderBook.hpp"
#include "Order.hpp"
#include "MarketDataFeed.hpp"
#include <string>
#include <iostream>
#include <algorithm>

template <typename PriceType, typename OrderIdType, typename Allocator>
MatchingEngine<PriceType, OrderIdType, Allocator>::MatchingEngine(OrderBook<PriceType, OrderIdType, Allocator>& ob)
    : orderBook(ob) {}

template <typename PriceType, typename OrderIdType, typename Allocator>
void MatchingEngine<PriceType, OrderIdType, Allocator>::processMarketData(const MarketData& data) {
    OrderIdType buyOrderId = generateOrderId();
    OrderIdType sellOrderId = generateOrderId();

    // Generate limit orders based on incoming market data
    orderBook.addOrder(buyOrderId, data.symbol, data.bid_price, defaultQuantity, true);   // Buy order
    orderBook.addOrder(sellOrderId, data.symbol, data.ask_price, defaultQuantity, false); // Sell order
    
    // After adding orders, try to match them
    matchOrders();

    // Check best bid and ask prices
    auto bestBid = orderBook.bestBid();
    auto bestAsk = orderBook.bestAsk();
    
    if (bestBid.has_value() && bestAsk.has_value()) {
      
        // If best bid is less than 100, add a new buy order
        if (bestBid.value() < 100) {
            OrderIdType newOrderId = generateOrderId();
            // Create a buy order at the current best bid price with 1 quantity
            auto orderPtr = orderBook.addOrder(
                newOrderId,
                data.symbol,
                bestBid.value(),
                1,  // quantity
                true  // is_buy
            );
            
            // Track the order in the order manager
            if (orderPtr) {
                bool success = orderManager.addOrder(orderPtr);
                if (success) {
                    std::cout << "Added new buy order at price " << bestBid.value() 
                              << " with ID " << newOrderId << std::endl;
                }
            }
        }
    }
}

template <typename PriceType, typename OrderIdType, typename Allocator>
void MatchingEngine<PriceType, OrderIdType, Allocator>::matchOrders() {
    auto& bids = orderBook.getBids();
    auto& asks = orderBook.getAsks();

    // Keep matching while there are crossing orders
    while (!bids.empty() && !asks.empty()) {
        auto bid_it = bids.begin();
        auto ask_it = asks.begin();

        // Check if orders cross (bid >= ask)
        if (bid_it->first < ask_it->first) {
            break;  // No more matches possible
        }

        auto& buyOrder = bid_it->second;
        auto& sellOrder = ask_it->second;

        // Calculate matched quantity
        int matchedQty = std::min(buyOrder->quantity, sellOrder->quantity);

        // Execute the trade
        std::cout << "MATCH: "
                 << "Buy Order " << buyOrder->id 
                 << " with Sell Order " << sellOrder->id
                 << " at price " << ask_it->first
                 << " for " << matchedQty << " units" << std::endl;

        // Update quantities
        buyOrder->quantity -= matchedQty;
        sellOrder->quantity -= matchedQty;

        // Remove fully filled orders
        if (buyOrder->quantity <= 0) {
            bids.erase(bid_it);
        }
        if (sellOrder->quantity <= 0) {
            asks.erase(ask_it);
        }
    }
}

template <typename PriceType, typename OrderIdType, typename Allocator>
OrderIdType MatchingEngine<PriceType, OrderIdType, Allocator>::generateOrderId() {
    static OrderIdType currentId = 1;
    return currentId++;
}

// Explicit template instantiation for common types
template class MatchingEngine<double, int, MemoryPool>;
