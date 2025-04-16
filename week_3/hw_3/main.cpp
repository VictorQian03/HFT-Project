#include <iostream>
#include <string>
#include <utility>
#include <cassert>
#include "trade.h"
#include "tradehandle.h"

// 3.2
void test_scope_cleanup() {
    Trade* rawTradePtr = new Trade("GAP", 18.70);
    TradeHandle handle(rawTradePtr);
    std::cout << "Symbol: " << handle->symbol << std::endl;
    std::cout << "Price: " << handle->price << std::endl;
    handle->price = 19.00;
    std::cout << "New Price: " << (*handle).price << std::endl;
}

int main() {
    // 1.2 
    Trade* googTradePtr = nullptr; 
    Trade* tradeArrayPtr = nullptr;
    const int arraySize = 5;
    // Dynamically allocate a single Trade object
    try {
        googTradePtr = new Trade("GOOG", 150.0);
    } catch (const std::bad_alloc& e) {
        std::cerr << "Memory allocation failed" << std::endl;
        return 1; 
    }
    // Dynamically allocate an array of 5 Trade objects
    try {
        tradeArrayPtr = new Trade[arraySize];
    } catch (const std::bad_alloc& e) {
        delete googTradePtr;
        googTradePtr = nullptr;
        std::cerr << "Memory allocation failed" << std::endl;
        return 1; 
    }
    // Use the objects, then delete them correctly
    std::cout << "Symbol: " << googTradePtr->symbol << ", Price: " << googTradePtr->price << std::endl;
    googTradePtr->price = 160.00;
    std::cout << "Updated Price: " << googTradePtr->price << std::endl;
    tradeArrayPtr[0] = Trade("MSFT", 385.73);
    tradeArrayPtr[1] = Trade("NVDA", 112.20);
    tradeArrayPtr[2] = Trade("AMZN", 179.59);
    tradeArrayPtr[3] = Trade("AAPL", 202.14);
    tradeArrayPtr[4] = Trade("TSLA", 254.11);
    for(int i = 0; i < arraySize; ++i) {
        std::cout << "Symbol: " << tradeArrayPtr[i].symbol
                  << ", Price: " << tradeArrayPtr[i].price << std::endl;
    }
    delete googTradePtr;  
    googTradePtr = nullptr;
    delete[] tradeArrayPtr;
    tradeArrayPtr = nullptr;
    /* Questions
    1. If we use delete on the array, we might only call the destructor for the first element 
    (or an unpredictable subset) rather than each object in the array. It may leave some 
    memory unfreed or corrupt the heap structure (memory leak problems).
    2. If we do not call delete at all, we will have a memory leak. 
    3. No, if we call delete twice on the same pointer, we will have a segmentation fault. 
    */
    std::cout << "--- Starting TradeHandle Test ---" << std::endl;

    test_scope_cleanup();

    std::cout << "\n--- TradeHandle Test Finished ---" << std::endl;
    return 0;
}

// g++ main.cpp tradehandle.cpp -o test_app -std=c++20