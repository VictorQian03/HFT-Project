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

void benchmark_add_orders_original(OrderBook& book, const std::vector<TestOrderData>& orders_to_add) {
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& o : orders_to_add) {
        book.addOrder(o.id, o.price, o.quantity, o.isBuy);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Original Book - Add " << orders_to_add.size() << " Orders: " << elapsed.count() << " ms" << std::endl;
}

void benchmark_modify_orders_original(OrderBook& book, const std::vector<TestOrderData>& modified_params) {
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& m : modified_params) {
        book.modifyOrder(m.id, m.price, m.quantity);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Original Book - Modify " << modified_params.size() << " Orders: " << elapsed.count() << " ms" << std::endl;
}

void benchmark_delete_orders_original(OrderBook& book, const std::vector<TestOrderData>& orders_to_delete) {
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& o : orders_to_delete) {
        book.deleteOrder(o.id);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Original Book - Delete " << orders_to_delete.size() << " Orders: " << elapsed.count() << " ms" << std::endl;
}

const size_t UNROLL_FACTOR = 4; 

void benchmark_add_orders_optimized_unrolled(OptimizedOrderBook& book, const std::vector<TestOrderData>& orders_to_add) {
    auto start = std::chrono::high_resolution_clock::now();
    size_t i = 0;
    size_t n = orders_to_add.size();
    for (i = 0; i + UNROLL_FACTOR <= n; i += UNROLL_FACTOR) {
        book.addOrder(orders_to_add[i].id, orders_to_add[i].price, orders_to_add[i].quantity, orders_to_add[i].isBuy);
        book.addOrder(orders_to_add[i+1].id, orders_to_add[i+1].price, orders_to_add[i+1].quantity, orders_to_add[i+1].isBuy);
        book.addOrder(orders_to_add[i+2].id, orders_to_add[i+2].price, orders_to_add[i+2].quantity, orders_to_add[i+2].isBuy);
        book.addOrder(orders_to_add[i+3].id, orders_to_add[i+3].price, orders_to_add[i+3].quantity, orders_to_add[i+3].isBuy);
    }
    for (; i < n; ++i) {
        book.addOrder(orders_to_add[i].id, orders_to_add[i].price, orders_to_add[i].quantity, orders_to_add[i].isBuy);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Optimized Book - Add (Unrolled " << UNROLL_FACTOR << ") " << orders_to_add.size() << " Orders: " << elapsed.count() << " ms" << std::endl;
}

void benchmark_modify_orders_optimized_unrolled(OptimizedOrderBook& book, const std::vector<TestOrderData>& modified_params) {
    auto start = std::chrono::high_resolution_clock::now();
    size_t i = 0;
    size_t n = modified_params.size();
    for (i = 0; i + UNROLL_FACTOR <= n; i += UNROLL_FACTOR) {
        book.modifyOrder(modified_params[i].id, modified_params[i].price, modified_params[i].quantity);
        book.modifyOrder(modified_params[i+1].id, modified_params[i+1].price, modified_params[i+1].quantity);
        book.modifyOrder(modified_params[i+2].id, modified_params[i+2].price, modified_params[i+2].quantity);
        book.modifyOrder(modified_params[i+3].id, modified_params[i+3].price, modified_params[i+3].quantity);
    }
    for (; i < n; ++i) {
        book.modifyOrder(modified_params[i].id, modified_params[i].price, modified_params[i].quantity);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Optimized Book - Modify (Unrolled " << UNROLL_FACTOR << ") " << modified_params.size() << " Orders: " << elapsed.count() << " ms" << std::endl;
}

void benchmark_delete_orders_optimized_unrolled(OptimizedOrderBook& book, const std::vector<TestOrderData>& orders_to_delete) {
    auto start = std::chrono::high_resolution_clock::now();
    size_t i = 0;
    size_t n = orders_to_delete.size();
    for (i = 0; i + UNROLL_FACTOR <= n; i += UNROLL_FACTOR) {
        book.deleteOrder(orders_to_delete[i].id);
        book.deleteOrder(orders_to_delete[i+1].id);
        book.deleteOrder(orders_to_delete[i+2].id);
        book.deleteOrder(orders_to_delete[i+3].id);
    }
    for (; i < n; ++i) {
        book.deleteOrder(orders_to_delete[i].id);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Optimized Book - Delete (Unrolled " << UNROLL_FACTOR << ") " << orders_to_delete.size() << " Orders: " << elapsed.count() << " ms" << std::endl;
}

void run_all_benchmarks(int current_num_orders) {
    std::cout << "\n--- Testing with " << current_num_orders << " base orders ---" << std::endl;
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
    { 
        OrderBook original_book;
        benchmark_add_orders_original(original_book, orders_data);
        benchmark_modify_orders_original(original_book, modified_params);
        benchmark_delete_orders_original(original_book, orders_data); 
    }

    std::cout << "\n--- Benchmarking OptimizedOrderBook ---" << std::endl;
    { 
        OptimizedOrderBook optimized_book(current_num_orders + 100); 
        benchmark_add_orders_optimized_unrolled(optimized_book, orders_data);
        benchmark_modify_orders_optimized_unrolled(optimized_book, modified_params);
        benchmark_delete_orders_optimized_unrolled(optimized_book, orders_data); 
        if (optimized_book.getActiveOrderCount() != 0 || optimized_book.getPoolUsedCount() != 0) {
            std::cout << "Warning (Optimized): Orders or pool objects not fully cleared after benchmarks for size " << current_num_orders << std::endl;
        }
    }
}

int main() {
    std::cout << std::fixed << std::setprecision(3);   
    std::cout << "\n\n--- Starting Full Performance Benchmark Suite ---" << std::endl;

    const int num_orders_small = 10000;
    const int num_orders_medium = 100000;
    const int num_orders_large = 500000; 

    std::vector<int> test_sizes = {num_orders_small, num_orders_medium, num_orders_large};

    for (int current_num_orders : test_sizes) {
        run_all_benchmarks(current_num_orders);
    }

    std::cout << "\n--- All Benchmarks Complete ---" << std::endl;

    return 0;
}