#include "../include/MarketDataFeed.hpp"
#include "../include/OrderBook.hpp"
#include "../include/MatchingEngine.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread> 
#include <atomic>

static OrderBook<double, uint64_t>* gBook     = nullptr;
static std::atomic<uint64_t>*     gNextOrder = nullptr;

static std::mt19937_64 gRng{std::random_device{}()};
static std::uniform_int_distribution<int> gSideDist(0,1);
static std::uniform_real_distribution<double> gOffsetDist(0.0, 1.0);


void handleTick(const MarketData& data) {
    // Supposed to push the data to the OrderBook/MatchingEngine, but just print it for now
    std::cout << "Received Tick: " << data << std::endl;

   
   
    
    int    qty   = 10;

    uint64_t oid = (*gNextOrder)++;                       
    
    bool ok_buy = gBook->addOrder(oid, data.symbol, data.bid_price, qty, true);
    
    if (!ok_buy) {
        std::cerr << "[OrderBook] pool exhausted, dropping order\n";
        return;
    }

    oid = (*gNextOrder)++;     
    bool ok_sell = gBook->addOrder(oid, data.symbol, data.ask_price, qty, false);
    
     if (!ok_sell) {
        std::cerr << "[OrderBook] pool exhausted, dropping order\n";
        return;
    }
    
    gBook->matchTop();
    gBook->printBook();
      
}

int main() {
    // Define symbols for the simulation
    std::vector<std::string> symbols = {"PRIV"};

    // Create the Market Data Feed simulator
    MarketDataFeed feed(symbols);

    OrderBook<double, uint64_t> book(100000);
    std::atomic<uint64_t>       nextOrder{1};

    // 3) point globals at them
    gBook     = &book;
    gNextOrder = &nextOrder;

    // Register handler function as the callback
    feed.registerCallback(handleTick);

    // Optionally set a custom tick delay
    feed.setTickDelay(500000);

   

    // Start the feed in a separate thread
    feed.start();

    // Let the simulation run for a while
    std::cout << "Simulation running for 10 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Stop the feed
    std::cout << "Stopping simulation..." << std::endl;
    feed.stop();

    std::cout << "Main application finished." << std::endl;
    return 0;
}