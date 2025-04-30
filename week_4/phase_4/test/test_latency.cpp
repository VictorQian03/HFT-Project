#include "../include/MarketDataFeed.hpp"
#include "../include/OrderBook.hpp"
#include "../include/MemoryPool.hpp"
#include "../include/MatchingEngine.hpp"
#include "../include/MarketData.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>

using namespace std::chrono;

// Typedefs for different configurations
using PriceType = double;
using OrderIdType = int;
using OrderType = Order<PriceType, OrderIdType>;
using AllocatorType = MemoryPool;
using OrderBookType = OrderBook<PriceType, OrderIdType, AllocatorType>;
using MatchingEngineType = MatchingEngine<PriceType, OrderIdType, AllocatorType>;

struct UnalignedMarketData {
    std::string symbol;
    double bid_price;
    double ask_price;
    std::chrono::high_resolution_clock::time_point timestamp;

    UnalignedMarketData() = default;

    UnalignedMarketData(std::string sym, double bid, double ask, std::chrono::high_resolution_clock::time_point ts)
        : symbol(std::move(sym)), bid_price(bid), ask_price(ask), timestamp(ts) {}
};



class LatencyTester {
private:
    static constexpr size_t POOL_SIZE = 10000000;
    OrderBookType orderBook;
    MatchingEngineType matchingEngine;
    std::vector<std::string> symbols;
    MarketDataFeed feed;
    std::vector<long long> aligned_latencies;
    std::vector<long long> unaligned_latencies;

public:
    LatencyTester() 
        : orderBook(sizeof(OrderType), POOL_SIZE),
          matchingEngine(orderBook),
          symbols({"PRIV"}),
          feed(symbols) {
    }
    void printAlignmentInfo() {
        std::cout << "\nMemory Alignment Information:\n"
                  << "Aligned MarketData size: " << sizeof(MarketData) << " bytes\n"
                  << "Aligned MarketData alignment: " << alignof(MarketData) << " bytes\n"
                  << "Unaligned MarketData size: " << sizeof(UnalignedMarketData) << " bytes\n"
                  << "Unaligned MarketData alignment: " << alignof(UnalignedMarketData) << " bytes\n"
                  << "Cache line size: 64 bytes\n\n";
    }

    void runAlignedTest(size_t numTicks) {
        std::cout << "\nRunning latency test with aligned market data " << numTicks << " ticks...\n";
        std::vector<long long> latencies;
        latencies.reserve(numTicks);
        
        // Generate random market data
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> priceDist(100.0, 200.0);
        std::uniform_real_distribution<> spreadDist(0.1, 1.0);

        
        latencies.reserve(numTicks);
        
        for (size_t i = 0; i < numTicks; ++i) {
            double mid_price = priceDist(gen);
            double spread = spreadDist(gen);
            double bid_price = mid_price - spread/2;
            double ask_price = mid_price + spread/2;
            
            MarketData data("PRIV", bid_price, ask_price, high_resolution_clock::now());
            
            // Measure processing time
            auto start = high_resolution_clock::now();
            matchingEngine.processMarketData(data);
            auto end = high_resolution_clock::now();
            
            auto latency = duration_cast<nanoseconds>(end - start).count();
            latencies.push_back(latency);
        }

        // Analyze latencies
        analyzeLatencies(latencies,"Aligned marketdata test");
    }

    void runUnalignedTest(size_t numTicks) {
        std::cout << "\nRunning unaligned test with " << numTicks << " ticks...\n";
        std::vector<long long> latencies;
        latencies.reserve(numTicks);
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> priceDist(100.0, 200.0);
        std::uniform_real_distribution<> spreadDist(0.1, 1.0);

        // Warm-up phase
        for (size_t i = 0; i < 1000; ++i) {
            double mid_price = priceDist(gen);
            double spread = spreadDist(gen);
            UnalignedMarketData data("PRIV", mid_price - spread/2, mid_price + spread/2, high_resolution_clock::now());
            // Convert to aligned MarketData for processing
            MarketData aligned_data(data.symbol, data.bid_price, data.ask_price, data.timestamp);
            matchingEngine.processMarketData(aligned_data);
        }

        // Actual test with unaligned MarketData
        for (size_t i = 0; i < numTicks; ++i) {
            double mid_price = priceDist(gen);
            double spread = spreadDist(gen);
            
            UnalignedMarketData data("PRIV", mid_price - spread/2, mid_price + spread/2, high_resolution_clock::now());
            
            std::atomic_thread_fence(std::memory_order_seq_cst);
            auto start = high_resolution_clock::now();
            // Convert to aligned MarketData for processing
            MarketData aligned_data(data.symbol, data.bid_price, data.ask_price, data.timestamp);
            matchingEngine.processMarketData(aligned_data);
            auto end = high_resolution_clock::now();
            std::atomic_thread_fence(std::memory_order_seq_cst);
            
            latencies.push_back(duration_cast<nanoseconds>(end - start).count());
        }

        unaligned_latencies = latencies;
        analyzeLatencies(latencies, "Unaligned MarketData Test");
    }
    
      void analyzeLatencies(const std::vector<long long>& latencies, const std::string& testName) {
        if (latencies.empty()) return;

        std::vector<long long> sorted_latencies = latencies;
        std::sort(sorted_latencies.begin(), sorted_latencies.end());

        auto min = sorted_latencies.front();
        auto max = sorted_latencies.back();
        double mean = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
        
        double variance = 0.0;
        for (auto l : latencies) {
            variance += (l - mean) * (l - mean);
        }
        double stddev = std::sqrt(variance / latencies.size());
        
        size_t p50_idx = static_cast<size_t>(latencies.size() * 0.50);
        size_t p95_idx = static_cast<size_t>(latencies.size() * 0.95);
        size_t p99_idx = static_cast<size_t>(latencies.size() * 0.99);
        
        std::cout << "\n" << testName << " Statistics (nanoseconds):\n"
                  << "Min: " << min << "\n"
                  << "Max: " << max << "\n"
                  << "Mean: " << mean << "\n"
                  << "StdDev: " << stddev << "\n"
                  << "P50: " << sorted_latencies[p50_idx] << "\n"
                  << "P95: " << sorted_latencies[p95_idx] << "\n"
                  << "P99: " << sorted_latencies[p99_idx] << "\n";
    }
};


int main() {
    LatencyTester tester;
    tester.printAlignmentInfo();
    
    std::vector<size_t> tickLoads = {1000, 10000, 100000};
    
    for (size_t ticks : tickLoads) {
        std::cout << "\n=== Testing with " << ticks << " ticks ===\n";
        tester.runAlignedTest(ticks);
        tester.runUnalignedTest(ticks);
       
    }
    
    return 0;
}
