#pragma once

#include <string>
#include <chrono>
#include <ostream>

struct alignas(64) MarketData {
    std::string symbol;
    double bid_price;
    double ask_price;
    std::chrono::high_resolution_clock::time_point timestamp;

    MarketData() = default;

    MarketData(std::string sym, double bid, double ask, std::chrono::high_resolution_clock::time_point ts)
        : symbol(std::move(sym)), bid_price(bid), ask_price(ask), timestamp(ts) {}

    // Printing
    friend std::ostream& operator<<(std::ostream& os, const MarketData& data) {
        auto duration = data.timestamp.time_since_epoch();
        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
        os << "Symbol: " << data.symbol
           << ", Bid: " << data.bid_price
           << ", Ask: " << data.ask_price
           << ", Timestamp (ns): " << nanoseconds;
        return os;
    }
};