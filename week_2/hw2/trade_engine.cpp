#include "trade_engine.h"
#include <vector>
#include <numeric>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <fstream>   
#include <iomanip>   
#include <sstream>  
#include <map>       
#include <ctime>     
#include <random> 
#include <chrono>

TradeEngine::TradeEngine(const std::vector<MarketData>& feed)
    : market_data(feed) {
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    random_generator.seed(seed);
}

void TradeEngine::process() {
    for (const auto& tick : market_data) {
        updateHistory(tick);

        int buy_votes = 0;
        int sell_votes = 0;
        std::vector<std::string> contributing_buy_signals;
        std::vector<std::string> contributing_sell_signals;

        // 1: Evaluate all signals and count votes
        SignalAction action1 = evaluateSignal1(tick);
        if (action1 == SignalAction::BUY) {
            buy_votes++;
            contributing_buy_signals.push_back("Signal 1 (Low Threshold)");
        } else if (action1 == SignalAction::SELL) {
            sell_votes++;
            contributing_sell_signals.push_back("Signal 1 (High Threshold)");
        }

        SignalAction action2 = evaluateSignal2(tick);
        if (action2 == SignalAction::BUY) {
            buy_votes++;
            contributing_buy_signals.push_back("Signal 2 (Below Avg)");
        } else if (action2 == SignalAction::SELL) {
            sell_votes++;
            contributing_sell_signals.push_back("Signal 2 (Above Avg)");
        }

        SignalAction action3 = evaluateSignal3(tick);
        if (action3 == SignalAction::BUY) {
            buy_votes++;
            contributing_buy_signals.push_back("Signal 3 (Momentum)");
        }

        // 2: Apply Voting Consensus
        if (buy_votes > sell_votes) {
            // Consensus is BUY: Place one BUY order
            std::string selected_signal;
            // Choose one of the contributing buy signals randomly for attribution
            if (contributing_buy_signals.size() == 1) {
                selected_signal = contributing_buy_signals[0];
            } else {
                std::uniform_int_distribution<> distrib(0, contributing_buy_signals.size() - 1);
                selected_signal = contributing_buy_signals[distrib(random_generator)];
            }
            placeOrder(tick, true, selected_signal); 

        } else if (sell_votes > buy_votes) {
            // Consensus is SELL: Place one SELL order
            std::string selected_signal;
            // Choose one of the contributing sell signals randomly for attribution
             if (contributing_sell_signals.size() == 1) {
                selected_signal = contributing_sell_signals[0];
            } else {
                std::uniform_int_distribution<> distrib(0, contributing_sell_signals.size() - 1);
                selected_signal = contributing_sell_signals[distrib(random_generator)];
            }
            placeOrder(tick, false, selected_signal); 

        }
    } 
}

void TradeEngine::placeOrder(const MarketData& tick, bool is_buy, const std::string& signal_name) {
    auto now = std::chrono::high_resolution_clock::now(); 
    Order order {
        tick.instrument_id,
        tick.price + (is_buy ? 0.01 : -0.01), 
        is_buy,
        now,
        signal_name 
    };
    orders.push_back(order);

    auto latency_ns = std::chrono::duration_cast<std::chrono::milliseconds>(now - tick.timestamp).count();
    latencies.push_back(latency_ns);
}

void TradeEngine::reportStats() {
    long long sum_latency = 0;
    long long max_latency = 0;
    std::map<std::string, int> signal_counts; 

    for (long long l : latencies) {
        sum_latency += l;
        max_latency = std::max(max_latency, l);
    }

    for (const auto& order : orders) {
        signal_counts[order.signal_source]++;
    }

    long long avg_latency = latencies.empty() ? 0 : sum_latency / latencies.size();

    std::cout << "\n--- Performance Report ---\n";
    std::cout << "Total Market Ticks Processed: " << market_data.size() << "\n";
    std::cout << "Total Orders Placed:          " << orders.size() << "\n";
    if (!orders.empty()) {
         std::cout << "Orders Breakdown by Signal:\n";
         for(const auto& pair : signal_counts) {
             std::cout << "  - " << std::left << std::setw(25) << pair.first + ":" << pair.second << "\n";
         }
    }
    std::cout << "Average Generation-to-Order Latency (ms): " << avg_latency << "\n";
    std::cout << "Maximum Generation-to-Order Latency (ms): " << max_latency << "\n";
}

// History Management
void TradeEngine::updateHistory(const MarketData& tick) {
    auto& prices = price_history[tick.instrument_id];
    prices.push_back(tick.price);
    if (prices.size() > 10) {
        prices.erase(prices.begin());
    }
    auto& timestamps = timestamp_history[tick.instrument_id];
    timestamps.push_back(tick.timestamp);
}

double TradeEngine::getAvg(int instrument_id) {
    auto it = price_history.find(instrument_id);
    if (it == price_history.end() || it->second.empty()) {
        return 0.0; 
    }
    const auto& history = it->second;
    double sum = std::accumulate(history.begin(), history.end(), 0.0);
    return sum / history.size();
}

// Signals
// Signal 1: Price thresholds
TradeEngine::SignalAction TradeEngine::evaluateSignal1(const MarketData& tick) {
    if (tick.price < 105.0) {
        return SignalAction::BUY;
    }
    if (tick.price > 195.0) {
        return SignalAction::SELL;
    }
    return SignalAction::NONE;
}

// Signal 2: Deviation from average
TradeEngine::SignalAction TradeEngine::evaluateSignal2(const MarketData& tick) {
    auto it = price_history.find(tick.instrument_id);
    if (it == price_history.end() || it->second.size() < 5) {
        return SignalAction::NONE;
    }
    double avg = getAvg(tick.instrument_id);
    if (avg <= 0) return SignalAction::NONE; 

    if (tick.price < avg * 0.98) {
        return SignalAction::BUY;
    }
    if (tick.price > avg * 1.02) {
        return SignalAction::SELL;
    }
    return SignalAction::NONE;
}

// Signal 3: Simple momentum
TradeEngine::SignalAction TradeEngine::evaluateSignal3(const MarketData& tick) {
    auto it = price_history.find(tick.instrument_id);
    if (it == price_history.end() || it->second.size() < 3) {
        return SignalAction::NONE; 
    }

    const auto& history = it->second;

    double price_t_minus_1 = history[history.size() - 2]; 
    double price_t_minus_2 = history[history.size() - 3]; 

    if (price_t_minus_1 > price_t_minus_2 && tick.price > price_t_minus_1) {
        return SignalAction::BUY;
    }

    return SignalAction::NONE;
}
