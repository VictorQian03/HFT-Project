#include "market_data_feed.h" 
#include "trade_engine.h"     
#include "market_data.h"     
#include <vector>             
#include <chrono>           
#include <iostream>          

int main() {
    std::vector<MarketData> market_feed_data;

    MarketDataFeed generator(market_feed_data);

    std::cout << "Generating market data..." << std::endl;
    auto start_gen = std::chrono::high_resolution_clock::now();

    generator.generateData(100000); 

    auto end_gen = std::chrono::high_resolution_clock::now();
    auto gen_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_gen - start_gen).count();
    std::cout << "Data generation took: " << gen_time << " ms" << std::endl;

    TradeEngine engine(market_feed_data);

    std::cout << "Processing trades..." << std::endl;
    auto start_process = std::chrono::high_resolution_clock::now();

    engine.process();

    auto end_process = std::chrono::high_resolution_clock::now();
    auto process_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_process - start_process).count();
    std::cout << "Trade processing took: " << process_time << " ms" << std::endl;

    engine.reportStats();

    auto total_runtime = std::chrono::duration_cast<std::chrono::milliseconds>(end_process - start_gen).count();
    std::cout << "Total Runtime (ms):       " << total_runtime << std::endl;

    return 0; 
}