#include "OrderProcessing.hpp" 
#include "Analytics.hpp"       

#include <iostream>
#include <string>
#include <vector>   
#include <thread>  
#include <chrono>   
#include <stdexcept> 

int main() {
    int numProducers = 4;          
    int ordersPerProducer = 25000; 
    
    int numConsumers = NUM_CONSUMERS_FOR_BARRIER; 

    // --- Output Simulation Configuration ---
    std::cout << "Starting High-Frequency Trading Simulation..." << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Producers: " << numProducers << std::endl;
    std::cout << "  Orders per Producer: " << ordersPerProducer << std::endl;
    std::cout << "  Total Orders to be Generated: " << numProducers * ordersPerProducer << std::endl;
    std::cout << "  Consumers: " << numConsumers << " (fixed by std::barrier size: NUM_CONSUMERS_FOR_BARRIER)" << std::endl;
    std::cout << "  Concurrent Queue Capacity: " << ORDER_QUEUE_CAPACITY << std::endl;
    std::cout << "  Max Concurrent Order Processing Slots (Semaphore): " << processingSemaphore.max() << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "Initializing and starting order processing..." << std::endl;

    auto overallStartTime = std::chrono::high_resolution_clock::now();

    // --- Start Order Processing Phase ---
    startOrderProcessing(numProducers, numConsumers, ordersPerProducer);

    auto processingEndTime = std::chrono::high_resolution_clock::now();
    auto processingDuration = std::chrono::duration_cast<std::chrono::milliseconds>(processingEndTime - overallStartTime);

    std::cout << "\n=============================================" << std::endl;
    std::cout << "Order Processing Phase Complete." << std::endl;
    std::cout << "  Total time for order generation and processing: " << processingDuration.count() << " ms." << std::endl;
    std::cout << "  Total orders successfully processed: " << ordersProcessed.load(std::memory_order_relaxed) << std::endl;
    std::cout << "=============================================" << std::endl;

    // --- Start Trade Analytics Phase ---
    std::cout << "Starting Trade Analytics..." << std::endl;
    
    auto analyticsStartTime = std::chrono::high_resolution_clock::now();
    runTradeAnalytics(); 

    auto analyticsEndTime = std::chrono::high_resolution_clock::now();
    auto analyticsDuration = std::chrono::duration_cast<std::chrono::milliseconds>(analyticsEndTime - analyticsStartTime);

    std::cout << "  Time for analytics computation: " << analyticsDuration.count() << " ms." << std::endl;
    std::cout << "=============================================" << std::endl;

    auto overallEndTime = std::chrono::high_resolution_clock::now();
    auto overallDuration = std::chrono::duration_cast<std::chrono::milliseconds>(overallEndTime - overallStartTime);
    std::cout << "Total Simulation Time: " << overallDuration.count() << " ms." << std::endl;
    std::cout << "Simulation Complete." << std::endl;

    return 0;
}