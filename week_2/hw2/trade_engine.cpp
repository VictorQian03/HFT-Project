#include "trade_engine.h"
#include <vector>
#include <numeric>   
#include <iostream>  
#include <algorithm> 
#include <unordered_map>

TradeEngine::TradeEngine(const std::vector<MarketData>& feed)
    : market_data(feed) {}

void TradeEngine::process() {
    for (const auto& tick : market_data) {
        updateHistory(tick);

        bool buy_signal = false;
        bool sell_signal = false;

        if (signal1(tick)) {
            if (tick.price < 105.0) {
                buy_signal = true;
            } else if (tick.price > 195.0) {
                buy_signal = true; 
            }
        }
        if (signal2(tick)) {

            if (tick.price < getAvg(tick.instrument_id)) { 
                buy_signal = true;
            } else { 
                sell_signal = true;
            }
        }
        if (signal3(tick)) {
            buy_signal = true;
        }

        bool place_order = buy_signal || sell_signal;
        bool is_buy_order = buy_signal; 

        if (place_order) {
            auto now = std::chrono::high_resolution_clock::now();
            Order order {
                tick.instrument_id,
                tick.price + (is_buy_order ? 0.01 : -0.01), 
                is_buy_order,
                now
            };
            orders.push_back(order);

            auto latency_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now - tick.timestamp).count();
            latencies.push_back(latency_ns);
        }
    }
}

void TradeEngine::reportStats() {
    long long sum_latency = 0;
    long long max_latency = 0;

    for (long long l : latencies) {
        sum_latency += l;
        if (l > max_latency) {
            max_latency = l;
        }
    }

    long long avg_latency = latencies.empty() ? 0 : sum_latency / latencies.size();

    std::cout << "\n--- Performance Report ---\n";
    std::cout << "Total Market Ticks Processed: " << market_data.size() << "\n";
    std::cout << "Total Orders Placed:          " << orders.size() << "\n";
    std::cout << "Average Tick-to-Trade Latency (ns): " << avg_latency << "\n";
    std::cout << "Maximum Tick-to-Trade Latency (ns): " << max_latency << "\n";
}

void TradeEngine::updateHistory(const MarketData& tick) {
    auto& history = price_history[tick.instrument_id];
    history.push_back(tick.price);
    if (history.size() > 10) {
        history.erase(history.begin()); 
    }
}

double TradeEngine::getAvg(int instrument_id) {
    auto it = price_history.find(instrument_id);
    if (it == price_history.end() || it->second.empty()) {
        return 0.0;
    }
    const auto& history = it->second;
    double sum = 0.0;
    for (double p : history) {
        sum += p;
    }
    return sum / history.size();
}

// Signal 1: Price thresholds
bool TradeEngine::signal1(const MarketData& tick) {
    return tick.price < 105.0 || tick.price > 195.0;
}

// Signal 2: Deviation from average
bool TradeEngine::signal2(const MarketData& tick) {
    auto it = price_history.find(tick.instrument_id);
    if (it == price_history.end() || it->second.size() < 5) {
        return false;
    }
    double avg = getAvg(tick.instrument_id);
    return tick.price < avg * 0.98 || tick.price > avg * 1.02;
}

// Signal 3: Simple momentum
bool TradeEngine::signal3(const MarketData& tick) {
    auto it = price_history.find(tick.instrument_id);
    if (it == price_history.end() || it->second.size() < 3) {
        return false;
    }

    const auto& history = it->second;
    double price_t_minus_1 = history[history.size() - 2];
    double price_t_minus_2 = history[history.size() - 3];
    double diff1 = price_t_minus_1 - price_t_minus_2; 
    double diff2 = tick.price - price_t_minus_1; 
    return diff1 > 0 && diff2 > 0;
}