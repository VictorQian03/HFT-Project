#ifndef MARKET_DATA_H
#define MARKET_DATA_H

#include <chrono>

struct alignas(64) MarketData {
    int instrument_id;
    double price;
    std::chrono::high_resolution_clock::time_point timestamp;
};

#endif 