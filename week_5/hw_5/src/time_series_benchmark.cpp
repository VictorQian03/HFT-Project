#include "../include/time_series_processor.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>

// Generate random price data
std::vector<double> generate_price_data(size_t size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> dist(100.0, 10.0);  // Mean: 100, StdDev: 10

    std::vector<double> prices(size);
    for (size_t i = 0; i < size; ++i) {
        prices[i] = dist(gen);
    }
    return prices;
}

// Run benchmark for a specific implementation
double benchmark_implementation(
    const TimeSeriesProcessor& processor,
    const std::vector<double>& prices,
    bool use_simd) {
    
    auto start = std::chrono::high_resolution_clock::now();
    
    if (use_simd) {
        processor.compute_moving_average_simd(prices);
    } else {
        processor.compute_moving_average_standard(prices);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

int main() {
    const size_t window_size = 1000;
    const std::vector<size_t> test_sizes = { 10000, 100000, 1000000,10000000};
    
    TimeSeriesProcessor processor(window_size);
    
    std::cout << "Time Series Processing Benchmark\n"
              << "==============================\n\n";
    
    std::cout << std::setw(12) << "Size" << " | "
              << std::setw(12) << "Standard (s)" << " | "
              << std::setw(12) << "SIMD (s)" << " | "
              << std::setw(12) << "Speedup" << "\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (size_t size : test_sizes) {
        auto prices = generate_price_data(size);
        
        // Warm up
        processor.compute_moving_average_standard(prices);
        processor.compute_moving_average_simd(prices);
        
        // Run benchmarks
        double standard_time = benchmark_implementation(processor, prices, false);
        double simd_time = benchmark_implementation(processor, prices, true);
        
        double speedup = standard_time / simd_time;
        
        std::cout << std::setw(12) << size << " | "
                  << std::fixed << std::setprecision(6)
                  << std::setw(12) << standard_time << " | "
                  << std::setw(12) << simd_time << " | "
                  << std::setw(12) << speedup << "\n";
    }
    
    return 0;
} 