#include "OrderProcessing.hpp"
#include "Analytics.hpp"

#include <chrono>
#include <iostream>
#include <vector>

int main() {
    constexpr int numProducers      = 4;
    constexpr int ordersPerProducer = 25'000;
    constexpr int numConsumers      = NUM_CONSUMERS_FOR_BARRIER;

    struct PolicyInfo {
        MemoryPolicy policy;
        const char*  name;
    };

    std::vector<PolicyInfo> policies = {
        { MemoryPolicy::RAR,    "RAR (relaxed/acq/rel)" },
        { MemoryPolicy::Fence,  "Fence + RAR"           },
        { MemoryPolicy::SeqCst, "SeqCst"                }
    };

    for (auto& p : policies) {
        g_memory_policy = p.policy;
        std::cout << "\n=== Benchmark: " << p.name << " ===\n";

        // reset everything
        reset_all(numProducers, ordersPerProducer);

        // processing phase timing
        auto t0 = std::chrono::high_resolution_clock::now();
        startOrderProcessing(numProducers, numConsumers, ordersPerProducer);
        auto t1 = std::chrono::high_resolution_clock::now();

        // analytics phase timing
        auto a0 = std::chrono::high_resolution_clock::now();
        runTradeAnalytics();  
        auto a1 = std::chrono::high_resolution_clock::now();

        auto proc_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        auto anal_ms = std::chrono::duration_cast<std::chrono::milliseconds>(a1 - a0).count();

        std::cout
            << "Processing Phase: " << proc_ms << " ms\n"
            << "Analytics Phase:  " << anal_ms << " ms\n";
    }

    return 0;
}