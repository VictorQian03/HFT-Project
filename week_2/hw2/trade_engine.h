#ifndef TRADE_ENGINE_H
#define TRADE_ENGINE_H

#include "market_data.h" 
#include "order.h"     
#include <vector>
#include <unordered_map>
#include <chrono>
#include <iostream> 

class TradeEngine {
public:
    TradeEngine(const std::vector<MarketData>& feed);

    void process();

    void reportStats();

private:
    const std::vector<MarketData>& market_data; 
    std::vector<Order> orders;                  
    std::vector<long long> latencies;           
    std::unordered_map<int, std::vector<double>> price_history;

    void updateHistory(const MarketData& tick);

    double getAvg(int instrument_id);

    bool signal1(const MarketData& tick);

    bool signal2(const MarketData& tick);

    bool signal3(const MarketData& tick);
};

#endif 