#pragma once

#include "ConcurrentQueue.hpp"
#include <string>
#include <chrono>
#include <atomic>
#include <vector>
#include <shared_mutex>
#include <unordered_map>
#include <semaphore>
#include <barrier>
#include <latch>
#include <mutex>

// Order & latency record
struct Order {
    int                                          orderId;
    std::string                                  symbol;
    int                                          quantity;
    double                                       price;
    bool                                         isBuy;
    std::chrono::high_resolution_clock::time_point enqueueTime;
    std::chrono::high_resolution_clock::time_point dequeueTime;
    std::chrono::nanoseconds                     processing_latency_ns;
    std::chrono::nanoseconds                     queue_time_ns;
};

struct LatencyRecord {
    int                           order_id;
    std::chrono::nanoseconds      queue_duration_ns;
    std::chrono::nanoseconds      processing_duration_ns;
};

// Shared globals
constexpr size_t ORDER_QUEUE_CAPACITY = 1024;
constexpr int    NUM_CONSUMERS_FOR_BARRIER = 4;

extern MemoryPolicy                             g_memory_policy;
extern ConcurrentQueue<Order, ORDER_QUEUE_CAPACITY> orderQueue;

extern std::unordered_map<std::string,std::pair<int,int>> orderBook;
extern std::shared_mutex                               orderBookMutex;

extern std::atomic<int>     ordersProcessed;
extern std::atomic<int>     totalExpectedOrders;

// now allow all consumers to run in parallel:
extern std::counting_semaphore<NUM_CONSUMERS_FOR_BARRIER> processingSemaphore;

extern std::barrier<>              consumerSyncBarrier;
extern std::latch                  startLatch;

extern std::vector<LatencyRecord> latencyDataStore;
extern std::mutex                 latencyDataMutex;

// Main API
void orderProducer(int producerId, int numOrders);
void orderConsumer(int consumerId);
void startOrderProcessing(int numProducers,
                          int numConsumers,
                          int ordersPerProducer);

// Reset between benchmark runs
void reset_all(int numProducers, int ordersPerProducer);