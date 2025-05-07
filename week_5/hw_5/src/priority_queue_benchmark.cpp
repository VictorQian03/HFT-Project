#include "../include/order_priority_queue.hpp"
#include <queue>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

using namespace std::chrono;

struct BenchmarkResult {
    double push_time_ms;
    double pop_time_ms;
};


BenchmarkResult benchmark_custom_queue(const std::vector<Order>& orders) {
    OrderPriorityQueue<Order, OrderCompare> queue;
    queue.reserve(orders.size());

    // Measure push time
    auto start = high_resolution_clock::now();
    for (const auto& order : orders) {
        queue.push(order);
    }
    auto end = high_resolution_clock::now();
    double push_time = duration_cast<milliseconds>(end - start).count();

    // Measure pop time
    start = high_resolution_clock::now();
    while (!queue.empty()) {
        queue.pop();
    }
    end = high_resolution_clock::now();
    double pop_time = duration_cast<milliseconds>(end - start).count();

    return {push_time, pop_time};
}

BenchmarkResult benchmark_std_queue(const std::vector<Order>& orders) {
    std::priority_queue<Order, std::vector<Order>, OrderCompare> queue;

    // Measure push time
    auto start = high_resolution_clock::now();
    for (const auto& order : orders) {
        queue.push(order);
    }
    auto end = high_resolution_clock::now();
    double push_time = duration_cast<milliseconds>(end - start).count();

    // Measure pop time
    start = high_resolution_clock::now();
    while (!queue.empty()) {
        queue.pop();
    }
    end = high_resolution_clock::now();
    double pop_time = duration_cast<milliseconds>(end - start).count();

    return {push_time, pop_time};
}

int main() {
    const std::vector<size_t> test_sizes = {100000, 1000000, 10000000};
    constexpr int num_trials = 5;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> price_dist(1.0, 1000.0);
    std::uniform_int_distribution<> timestamp_dist(0, 1000000);

    for (size_t N : test_sizes) {
        std::cout << "\n=== Benchmark for N = " << N << " ===\n";
        
        std::vector<Order> orders;
        orders.reserve(N);
        
        // Generate test data
        for (size_t i = 0; i < N; ++i) {
            orders.emplace_back(price_dist(gen), timestamp_dist(gen), "ORDER" + std::to_string(i));
        }

       
        BenchmarkResult custom_avg = {0, 0};
        BenchmarkResult std_avg = {0, 0};

        for (int trial = 0; trial < num_trials; ++trial) {
            auto custom_result = benchmark_custom_queue(orders);
            auto std_result = benchmark_std_queue(orders);

            custom_avg.push_time_ms += custom_result.push_time_ms;
            custom_avg.pop_time_ms += custom_result.pop_time_ms;
            std_avg.push_time_ms += std_result.push_time_ms;
            std_avg.pop_time_ms += std_result.pop_time_ms;
        }

        
        custom_avg.push_time_ms /= num_trials;
        custom_avg.pop_time_ms /= num_trials;
        std_avg.push_time_ms /= num_trials;
        std_avg.pop_time_ms /= num_trials;

       
        std::cout << "Custom Priority Queue (avg over " << num_trials << " runs):\n"
                  << "  Push: " << custom_avg.push_time_ms << " ms\n"
                  << "  Pop:  " << custom_avg.pop_time_ms << " ms\n"
                  << "std::priority_queue (avg over " << num_trials << " runs):\n"
                  << "  Push: " << std_avg.push_time_ms << " ms\n"
                  << "  Pop:  " << std_avg.pop_time_ms << " ms\n";
    }
} 