#include "../include/MarketDataFeed.hpp"
#include "../include/OrderBook.hpp"
#include "../include/MemoryPool.hpp"
#include "../include/MatchingEngine.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread> 


// Typedefs for simplicity
using PriceType = double;
using OrderIdType = int;
using OrderType = Order<PriceType, OrderIdType>;
using AllocatorType = MemoryPool;
using OrderBookType = OrderBook<PriceType, OrderIdType, AllocatorType>;
using MatchingEngineType = MatchingEngine<PriceType, OrderIdType, AllocatorType>;

// Tell the pool how big each block is, and how many you want
constexpr std::size_t blockSize = sizeof(OrderType);
constexpr std::size_t poolSize  = 1024;


OrderBookType orderBook(sizeof(OrderType), 1024);
MatchingEngineType matchingEngine(orderBook);



// Callback function to handle market data ticks
void handleTick(const MarketData& data) {
    matchingEngine.processMarketData(data);
    orderBook.printOrders(); // Optional: print current book state
}

int main() {
    std::vector<std::string> symbols = {"PRIV", "VPC", "PCMM", "PCLO"};

    MarketDataFeed feed(symbols);
    feed.registerCallback(handleTick);
    feed.setTickDelay(500000); // 0.5 seconds per tick

    feed.start();

    std::cout << "Simulation running for 10 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));

    feed.stop();

    std::cout << "Stopping simulation..." << std::endl;
    return 0;
}
