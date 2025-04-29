#pragma once

#include "MarketData.hpp"
#include <vector>
#include <string>
#include <functional> 
#include <thread>    
#include <atomic>     
#include <random>     
#include <map>        
#include <chrono>     

class MarketDataFeed {
public:
    using TickCallback = std::function<void(const MarketData&)>;

    // Constructor
    MarketDataFeed(std::vector<std::string> symbols);

    // Destructor
    ~MarketDataFeed();

    // Register the callback function to be called for each new tick
    void registerCallback(TickCallback callback);

    // Start the market data simulation
    void start();

    // Stop the market data simulation
    void stop();

    // Set the delay between ticks (in microseconds)
    void setTickDelay(long long microseconds);

private:
    // Runs in the simulation thread
    void runSimulation();

    // Configuration
    std::vector<std::string> m_symbols;
    // Default delay: 1 millisecond
    long long m_tick_delay_us = 1000; 

    // State
    std::atomic<bool> m_running{false}; 
    std::thread m_simulation_thread;  
    TickCallback m_callback;            

    // Mock data generation helpers
    std::mt19937 m_rng; 
    std::uniform_real_distribution<double> m_price_change_dist{-0.1, 0.1}; 
    std::uniform_int_distribution<size_t> m_symbol_index_dist; 
    std::map<std::string, std::pair<double, double>> m_current_prices; 
};