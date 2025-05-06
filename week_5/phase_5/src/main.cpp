#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <iomanip>
#include "../include/orderbook.h"     
#include "../include/optimized_orderbook.h"

struct TestOrderData {
    std::string id;
    double price;
    int quantity;
    bool isBuy;
};

struct BenchmarkTimings {
    double add_ms = 0.0;
    double modify_ms = 0.0;
    double delete_ms = 0.0;
    double total_ms() const { return add_ms + modify_ms + delete_ms; }
};

std::vector<TestOrderData> generate_orders(int count, double basePrice = 100.0, int priceRange = 1000) {
    std::vector<TestOrderData> orders;
    orders.reserve(count);
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> price_dist(basePrice - (double)priceRange/200.0, basePrice + (double)priceRange/200.0);
    std::uniform_int_distribution<int> quantity_dist(1, 100);
    std::uniform_int_distribution<int> side_dist(0, 1);

    for (int i = 0; i < count; ++i) {
        orders.push_back({
            "ORD" + std::to_string(i),
            std::round(price_dist(rng) * 100.0) / 100.0,
            quantity_dist(rng),
            side_dist(rng) == 1
        });
    }
    return orders;
}

BenchmarkTimings benchmark_add_orders_original_timed(OrderBook& book, const std::vector<TestOrderData>& orders_to_add) {
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& o : orders_to_add) {
        book.addOrder(o.id, o.price, o.quantity, o.isBuy);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Original Book - Add " << orders_to_add.size() << " Orders: " << elapsed.count() << " ms" << std::endl;
    return {elapsed.count(), 0.0, 0.0};
}

BenchmarkTimings benchmark_modify_orders_original_timed(OrderBook& book, const std::vector<TestOrderData>& modified_params) {
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& m : modified_params) {
        book.modifyOrder(m.id, m.price, m.quantity);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Original Book - Modify " << modified_params.size() << " Orders: " << elapsed.count() << " ms" << std::endl;
    return {0.0, elapsed.count(), 0.0};
}

BenchmarkTimings benchmark_delete_orders_original_timed(OrderBook& book, const std::vector<TestOrderData>& orders_to_delete) {
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& o : orders_to_delete) {
        book.deleteOrder(o.id);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Original Book - Delete " << orders_to_delete.size() << " Orders: " << elapsed.count() << " ms" << std::endl;
    return {0.0, 0.0, elapsed.count()};
}

const size_t UNROLL_FACTOR = 4; 

BenchmarkTimings benchmark_add_orders_optimized_unrolled_timed(OptimizedOrderBook& book, const std::vector<TestOrderData>& orders_to_add) {
    auto start = std::chrono::high_resolution_clock::now();
    size_t i = 0; size_t n = orders_to_add.size();
    for (i = 0; i + UNROLL_FACTOR <= n; i += UNROLL_FACTOR) { 
        book.addOrder(orders_to_add[i].id, orders_to_add[i].price, orders_to_add[i].quantity, orders_to_add[i].isBuy);
        book.addOrder(orders_to_add[i+1].id, orders_to_add[i+1].price, orders_to_add[i+1].quantity, orders_to_add[i+1].isBuy);
        book.addOrder(orders_to_add[i+2].id, orders_to_add[i+2].price, orders_to_add[i+2].quantity, orders_to_add[i+2].isBuy);
        book.addOrder(orders_to_add[i+3].id, orders_to_add[i+3].price, orders_to_add[i+3].quantity, orders_to_add[i+3].isBuy);
    }
    for (; i < n; ++i) { book.addOrder(orders_to_add[i].id, orders_to_add[i].price, orders_to_add[i].quantity, orders_to_add[i].isBuy); }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Optimized Book - Add (Unrolled " << UNROLL_FACTOR << ") " << orders_to_add.size() << " Orders: " << elapsed.count() << " ms" << std::endl;
    return {elapsed.count(), 0.0, 0.0};
}

BenchmarkTimings benchmark_modify_orders_optimized_unrolled_timed(OptimizedOrderBook& book, const std::vector<TestOrderData>& modified_params) {
    auto start = std::chrono::high_resolution_clock::now();
    size_t i = 0; size_t n = modified_params.size();
    for (i = 0; i + UNROLL_FACTOR <= n; i += UNROLL_FACTOR) { 
        book.modifyOrder(modified_params[i].id, modified_params[i].price, modified_params[i].quantity);
        book.modifyOrder(modified_params[i+1].id, modified_params[i+1].price, modified_params[i+1].quantity);
        book.modifyOrder(modified_params[i+2].id, modified_params[i+2].price, modified_params[i+2].quantity);
        book.modifyOrder(modified_params[i+3].id, modified_params[i+3].price, modified_params[i+3].quantity);
    }
    for (; i < n; ++i) { book.modifyOrder(modified_params[i].id, modified_params[i].price, modified_params[i].quantity); }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Optimized Book - Modify (Unrolled " << UNROLL_FACTOR << ") " << modified_params.size() << " Orders: " << elapsed.count() << " ms" << std::endl;
    return {0.0, elapsed.count(), 0.0};
}

BenchmarkTimings benchmark_delete_orders_optimized_unrolled_timed(OptimizedOrderBook& book, const std::vector<TestOrderData>& orders_to_delete) {
    auto start = std::chrono::high_resolution_clock::now();
    size_t i = 0; size_t n = orders_to_delete.size();
    for (i = 0; i + UNROLL_FACTOR <= n; i += UNROLL_FACTOR) {
        book.deleteOrder(orders_to_delete[i].id);
        book.deleteOrder(orders_to_delete[i+1].id);
        book.deleteOrder(orders_to_delete[i+2].id);
        book.deleteOrder(orders_to_delete[i+3].id);
    }
    for (; i < n; ++i) { book.deleteOrder(orders_to_delete[i].id); }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Optimized Book - Delete (Unrolled " << UNROLL_FACTOR << ") " << orders_to_delete.size() << " Orders: " << elapsed.count() << " ms" << std::endl;
    return {0.0, 0.0, elapsed.count()};
}

std::vector<int> collected_order_sizes;
std::vector<double> collected_times_original_total_s;
std::vector<double> collected_times_optimized_total_s;

void run_all_benchmarks(int current_num_orders) {
    std::cout << "\n--- Testing with " << current_num_orders << " base orders ---" << std::endl;
    collected_order_sizes.push_back(current_num_orders); 

    std::vector<TestOrderData> orders_data = generate_orders(current_num_orders);
    std::vector<TestOrderData> modified_params;
    modified_params.reserve(orders_data.size());
    std::mt19937 rng_modify(std::chrono::steady_clock::now().time_since_epoch().count() + current_num_orders); 
    std::uniform_real_distribution<double> price_delta_dist(-0.5, 0.5);
    std::uniform_int_distribution<int> quantity_delta_dist(-10, 10);
    for(const auto& o_orig : orders_data) {
        modified_params.push_back({
            o_orig.id,
            std::max(1.0, std::round((o_orig.price + price_delta_dist(rng_modify)) * 100.0) / 100.0),
            std::max(1, o_orig.quantity + quantity_delta_dist(rng_modify)),
            o_orig.isBuy
        });
    }

    std::cout << "\n--- Benchmarking Original OrderBook ---" << std::endl;
    BenchmarkTimings original_total_time;
    {
        OrderBook original_book;
        original_total_time.add_ms = benchmark_add_orders_original_timed(original_book, orders_data).add_ms;
        original_total_time.modify_ms = benchmark_modify_orders_original_timed(original_book, modified_params).modify_ms;
        original_total_time.delete_ms = benchmark_delete_orders_original_timed(original_book, orders_data).delete_ms;
    }
    std::cout << "Original Book - Total time for " << current_num_orders << " ops sequence: " << original_total_time.total_ms() << " ms" << std::endl;
    collected_times_original_total_s.push_back(original_total_time.total_ms() / 1000.0); 

    std::cout << "\n--- Benchmarking OptimizedOrderBook ---" << std::endl;
    BenchmarkTimings optimized_total_time;
    {
        OptimizedOrderBook optimized_book(current_num_orders + 100);
        optimized_total_time.add_ms = benchmark_add_orders_optimized_unrolled_timed(optimized_book, orders_data).add_ms;
        optimized_total_time.modify_ms = benchmark_modify_orders_optimized_unrolled_timed(optimized_book, modified_params).modify_ms;
        optimized_total_time.delete_ms = benchmark_delete_orders_optimized_unrolled_timed(optimized_book, orders_data).delete_ms;
    }
    std::cout << "Optimized Book - Total time for " << current_num_orders << " ops sequence: " << optimized_total_time.total_ms() << " ms" << std::endl;
    collected_times_optimized_total_s.push_back(optimized_total_time.total_ms() / 1000.0); 
}


int main() {
    std::cout << std::fixed << std::setprecision(3);   
    std::cout << "\n\n--- Starting Full Performance Benchmark Suite ---" << std::endl;

    const int num_orders_mini = 1000;
    const int num_orders_tiny = 5000;
    const int num_orders_small = 10000;
    const int num_orders_medium = 50000;
    const int num_orders_large = 100000; 

    std::vector<int> test_sizes = {num_orders_mini, num_orders_tiny, num_orders_small, num_orders_medium, num_orders_large};

    for (int current_num_orders : test_sizes) {
        run_all_benchmarks(current_num_orders);
    }

    std::cout << "\n--- All Benchmarks Complete ---" << std::endl;

    std::cout << "\n--- Data for Plotting ---" << std::endl;
    std::cout << "OrderSizes:";
    for (int size : collected_order_sizes) { std::cout << " " << size; }
    std::cout << std::endl;

    std::cout << "OriginalTotalTimes_s:";
    for (double time_s : collected_times_original_total_s) { std::cout << " " << std::fixed << std::setprecision(5) << time_s; }
    std::cout << std::endl;

    std::cout << "OptimizedTotalTimes_s:";
    for (double time_s : collected_times_optimized_total_s) { std::cout << " " << std::fixed << std::setprecision(5) << time_s; }
    std::cout << std::endl;

    return 0;
}