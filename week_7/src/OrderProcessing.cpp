#include "OrderProcessing.hpp"
#include <iostream>
#include <thread>
#include <cassert>
#include <new>  

// Definitions of externs
MemoryPolicy g_memory_policy = MemoryPolicy::RAR;
ConcurrentQueue<Order, ORDER_QUEUE_CAPACITY> orderQueue;
std::unordered_map<std::string,std::pair<int,int>> orderBook;
std::shared_mutex                               orderBookMutex;

std::atomic<int>                         ordersProcessed{0};
std::atomic<int>                         totalExpectedOrders{0};
std::counting_semaphore<NUM_CONSUMERS_FOR_BARRIER> processingSemaphore(NUM_CONSUMERS_FOR_BARRIER);

std::barrier<>  consumerSyncBarrier(NUM_CONSUMERS_FOR_BARRIER);
std::latch      startLatch(1);

std::vector<LatencyRecord> latencyDataStore;
std::mutex                 latencyDataMutex;

// Reset helper
void reset_all(int numProducers, int ordersPerProducer) {
    // reset the queue
    orderQueue.reset();

    // clear the book
    {
        std::unique_lock lk(orderBookMutex);
        orderBook.clear();
    }

    // clear latency data
    {
        std::lock_guard lk(latencyDataMutex);
        latencyDataStore.clear();
    }

    // counters
    ordersProcessed.store(0, std::memory_order_relaxed);
    totalExpectedOrders.store(
        numProducers * ordersPerProducer,
        std::memory_order_relaxed
    );

    // destroy & re-create latch and barrier in place
    startLatch.~latch();
    consumerSyncBarrier.~barrier();
    new (&startLatch) std::latch(1);
    new (&consumerSyncBarrier) std::barrier<>(NUM_CONSUMERS_FOR_BARRIER);

    // reset semaphore to full count
    while (processingSemaphore.try_acquire()) {
    }
    for (int i = 0; i < NUM_CONSUMERS_FOR_BARRIER; ++i) {
        processingSemaphore.release();
    }
}

// Producer
void orderProducer(int producerId, int numOrders) {
    for (int i = 0; i < numOrders; ++i) {
        Order order;
        order.orderId     = producerId * 10000 + i;
        order.symbol      = (i % 3 == 0) ? "AAPL"
                           : (i % 3 == 1) ? "GOOGL"
                                          : "MSFT";
        order.quantity    = 10 + (i % 10);
        order.price       = (order.symbol == "AAPL")  ? 150.0 + (i*0.01)
                            : (order.symbol == "GOOGL") ? 2500.0 + (i*0.05)
                                                         : 300.0  + (i*0.02);
        order.isBuy       = ((producerId + i) % 2 == 0);
        order.enqueueTime = std::chrono::high_resolution_clock::now();

        while (!orderQueue.push(order)) {
            std::this_thread::yield();
        }
    }
}

// Consumer
void orderConsumer(int consumerId) {
    startLatch.wait();

    while (true) {
        Order order;
        if (!orderQueue.pop(order)) {
            if (ordersProcessed.load(std::memory_order_acquire)
                >= totalExpectedOrders.load(std::memory_order_acquire))
            {
                break;
            }
            continue;  
        }

        order.dequeueTime = std::chrono::high_resolution_clock::now();

        processingSemaphore.acquire();
        auto procStart = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(
            std::chrono::microseconds(100 + (order.orderId % 50))
        );
        auto procEnd = std::chrono::high_resolution_clock::now();
        processingSemaphore.release();

        {
            std::unique_lock lk(orderBookMutex);
            auto& e = orderBook[order.symbol];
            if (order.isBuy)  e.first  += order.quantity;
            else              e.second += order.quantity;
        }

        order.queue_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            order.dequeueTime - order.enqueueTime
        );
        order.processing_latency_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            procEnd - procStart
        );
        {
            std::lock_guard lk(latencyDataMutex);
            latencyDataStore.push_back({
                order.orderId,
                order.queue_time_ns,
                order.processing_latency_ns
            });
        }

        ordersProcessed.fetch_add(1, std::memory_order_relaxed);
    }

    consumerSyncBarrier.arrive_and_wait();
}

// Orchestrator
void startOrderProcessing(int numProducers,
                          int numConsumers,
                          int ordersPerProducer)
{
    assert(numConsumers == NUM_CONSUMERS_FOR_BARRIER
           && "NUM_CONSUMERS_FOR_BARRIER must match");

    totalExpectedOrders.store(
        numProducers * ordersPerProducer,
        std::memory_order_relaxed
    );

    std::vector<std::thread> producers, consumers;
    producers.reserve(numProducers);
    consumers.reserve(numConsumers);

    for (int i = 0; i < numProducers; ++i) {
        producers.emplace_back(orderProducer, i, ordersPerProducer);
    }

    for (int i = 0; i < numConsumers; ++i) {
        consumers.emplace_back(orderConsumer, i);
    }

    startLatch.count_down();

    for (auto& t : producers) t.join();
    for (auto& t : consumers) t.join();
}