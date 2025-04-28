#include "../include/MarketDataFeed.hpp"
#include <iostream> 
#include <random>
#include <chrono>
#include <thread> 

// Constructor
MarketDataFeed::MarketDataFeed(std::vector<std::string> symbols)
    : m_symbols(std::move(symbols)),
      m_rng(std::random_device{}()), 
      m_symbol_index_dist(0, m_symbols.size() - 1) 
{
    double start_price = 100.0;
    double spread = 0.1;
    for (const auto& sym : m_symbols) {
        m_current_prices[sym] = {start_price, start_price + spread};
        start_price += 10.0; 
    }
}

// Destructor
MarketDataFeed::~MarketDataFeed() {
    stop(); 
}

// Register the callback
void MarketDataFeed::registerCallback(TickCallback callback) {
    m_callback = std::move(callback);
}

// Set tick delay
void MarketDataFeed::setTickDelay(long long microseconds) {
    m_tick_delay_us = microseconds > 0 ? microseconds : 1; 
}


// Start the simulation thread
void MarketDataFeed::start() {
    if (m_running) {
        std::cerr << "Warning: MarketDataFeed already running." << std::endl;
        return;
    }
    m_running = true;
    m_simulation_thread = std::thread(&MarketDataFeed::runSimulation, this);
    std::cout << "MarketDataFeed started." << std::endl;
}

// Stop the simulation thread
void MarketDataFeed::stop() {
    m_running = false;
    if (m_simulation_thread.joinable()) {
        m_simulation_thread.join(); 
        std::cout << "MarketDataFeed stopped." << std::endl;
    }
}

// The core simulation loop running in its own thread
void MarketDataFeed::runSimulation() {
    while (m_running) {
        // 1. Select a random symbol
        const std::string& symbol = m_symbols[m_symbol_index_dist(m_rng)];

        // 2. Get current prices and generate small changes
        auto& prices = m_current_prices[symbol];
        double bid_change = m_price_change_dist(m_rng);
        double ask_change = m_price_change_dist(m_rng);

        // Update prices (make sure bid < ask and prices stay positive)
        prices.first = std::max(0.01, prices.first + bid_change);
        prices.second = std::max(prices.first + 0.01, prices.second + ask_change); 

        // 3. Get high-resolution timestamp just before creating the object
        auto now = std::chrono::high_resolution_clock::now();

        // 4. Create the MarketData tick
        MarketData tick(symbol, prices.first, prices.second, now);

        // 5. Send the tick to the registered callback 
        if (m_callback) {
            try {
                 m_callback(tick);
            } catch (const std::exception& e) {
                 std::cerr << "Error in MarketDataFeed callback: " << e.what() << std::endl;
            } catch (...) {
                 std::cerr << "Unknown error in MarketDataFeed callback." << std::endl;
            }

        } else {
            std::cout << "Generated Tick: " << tick << std::endl;
        }


        // 6. Wait for a short period to simulate time between ticks
        std::this_thread::sleep_for(std::chrono::microseconds(m_tick_delay_us));
    }
}