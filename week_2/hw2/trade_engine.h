#ifndef TRADE_ENGINE_H
#define TRADE_ENGINE_H

#include "market_data.h"
#include "order.h"
#include <random>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <string> 

class TradeEngine {
public:
    TradeEngine(const std::vector<MarketData>& feed);

    void process();
    void reportStats();

    void exportOrderHistory(const std::string& filename);
    void exportTickData(const std::string& filename);

private:
    const std::vector<MarketData>& market_data;
    std::vector<Order> orders;
    std::vector<long long> latencies;
    std::unordered_map<int, std::vector<double>> price_history;
    std::unordered_map<int, std::vector<std::chrono::high_resolution_clock::time_point>> timestamp_history;
    std::mt19937 random_generator;
    
    void updateHistory(const MarketData& tick);
    double getAvg(int instrument_id);

    enum class SignalAction { NONE, BUY, SELL };

    SignalAction evaluateSignal1(const MarketData& tick);
    SignalAction evaluateSignal2(const MarketData& tick);
    SignalAction evaluateSignal3(const MarketData& tick);

    void placeOrder(const MarketData& tick, bool is_buy, const std::string& signal_name);

};

#endif