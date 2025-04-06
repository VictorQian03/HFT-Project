#ifndef ORDER_H
#define ORDER_H

#include <chrono>

struct alignas(64) Order {
    int instrument_id;
    double price;
    bool is_buy; 
    std::chrono::high_resolution_clock::time_point timestamp;
};

#endif