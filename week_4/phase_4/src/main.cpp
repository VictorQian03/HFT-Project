#include "./include/MarketDataFeed.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread> 
#
void handleTick(const MarketData& data) {
    // Supposed to push the data to the OrderBook/MatchingEngine, but just print it for now
    std::cout << "Received Tick: " << data << std::endl;
}

int main() {
    // Define symbols for the simulation
    std::vector<std::string> symbols = {"PRIV", "VPC", "PCMM", "PCLO"};

    // Create the Market Data Feed simulator
    MarketDataFeed feed(symbols);

    // Register handler function as the callback
    feed.registerCallback(handleTick);

    // Optionally set a custom tick delay
    feed.setTickDelay(500000);

    // Start the feed in a separate thread
    feed.start();

    // Let the simulation run for a while
    std::cout << "Simulation running for 10 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Stop the feed
    std::cout << "Stopping simulation..." << std::endl;
    feed.stop();

    std::cout << "Main application finished." << std::endl;
    return 0;
}