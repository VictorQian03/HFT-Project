#include "../include/optimized_orderbook.h"
#include "../include/unoptimized_orderbook.h"
#include <iostream>
#include <chrono>
#include <random>
#include <string>
#include <vector>
#include <unordered_set>

using namespace std::chrono;

// Helper function to generate random orders
std::vector<std::tuple<std::string, double, int, bool>> generateRandomOrders(size_t count) {
    std::vector<std::tuple<std::string, double, int, bool>> orders;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> priceDist(1.0, 1000.0);
    std::uniform_int_distribution<> qtyDist(1, 1000);
    std::uniform_int_distribution<> buyDist(0, 1);

    for (size_t i = 0; i < count; ++i) {
        orders.emplace_back(
            "ORDER_" + std::to_string(i),
            priceDist(gen),
            qtyDist(gen),
            buyDist(gen) == 1
        );
    }
    return orders;
}

// Specialized benchmark for OptimizedOrderBook
void runOptimizedBenchmark(const std::vector<std::tuple<std::string, double, int, bool>>& orders) {
    const int NUM_ORDERS_ADD = orders.size();
    const int NUM_ORDERS_MODIFY = NUM_ORDERS_ADD / 2;
    const int NUM_ORDERS_DELETE = NUM_ORDERS_ADD / 4;
    const int NUM_PRICE_QUERIES = 10000;
    const int NUM_BBO_LOOKUPS = 10000;

    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::vector<std::string> order_ids;
    order_ids.reserve(NUM_ORDERS_ADD);

    OptimizedOrderBook ob(NUM_ORDERS_ADD);

    // Test 1: Adding orders
    {
        std::cout << "\nTest 1: Adding " << NUM_ORDERS_ADD << " orders." << std::endl;
        auto start = high_resolution_clock::now();

        for (const auto& [id, price, qty, isBuy] : orders) {
            ob.addOrder(id, price, qty, isBuy);
            order_ids.push_back(id);
        }

        auto end = high_resolution_clock::now();
        duration<double> elapsed = end - start;
        std::cout << "  Time taken: " << elapsed.count() << " seconds." << std::endl;
        std::cout << "  Active Orders: " << ob.getActiveOrderCount() << std::endl;
        std::cout << "  Pool Used: " << ob.getPoolUsedCount() << ", Free: " << ob.getPoolFreeCount() << std::endl;
    }

    // Test 2: Modifying orders
    {
        std::cout << "\nTest 2: Modifying " << NUM_ORDERS_MODIFY << " random orders." << std::endl;
        auto start = high_resolution_clock::now();

        std::uniform_int_distribution<size_t> id_idx_dist(0, order_ids.size() - 1);
        std::uniform_real_distribution<double> new_price_dist(94.0, 106.0); 
        std::uniform_int_distribution<int> new_qty_dist(1, 1200);

        for (int i = 0; i < NUM_ORDERS_MODIFY; ++i) {
            std::string id_to_modify = order_ids[id_idx_dist(rng)];
            double newPrice = new_price_dist(rng);
            int newQuantity = new_qty_dist(rng);
            ob.modifyOrder(id_to_modify, newPrice, newQuantity);
        }

        auto end = high_resolution_clock::now();
        duration<double> elapsed = end - start;
        std::cout << "  Time taken: " << elapsed.count() << " seconds." << std::endl;
        std::cout << "  Active Orders: " << ob.getActiveOrderCount() << std::endl;
        std::cout << "  Pool Used: " << ob.getPoolUsedCount() << ", Free: " << ob.getPoolFreeCount() << std::endl;
    }

    // Test 3: Deleting orders
    {
        std::cout << "\nTest 3: Deleting " << NUM_ORDERS_DELETE << " random orders." << std::endl;
        auto start = high_resolution_clock::now();

        std::uniform_int_distribution<size_t> id_idx_dist(0, order_ids.size() - 1);
        std::vector<std::string> ids_to_delete;
        ids_to_delete.reserve(NUM_ORDERS_DELETE);

        std::unordered_set<size_t> indices_to_delete;
        while(indices_to_delete.size() < (size_t)NUM_ORDERS_DELETE && indices_to_delete.size() < order_ids.size()) {
            indices_to_delete.insert(id_idx_dist(rng));
        }
        for(size_t idx : indices_to_delete) {
            ids_to_delete.push_back(order_ids[idx]);
        }

        for (const auto& id : ids_to_delete) {
            ob.deleteOrder(id);
        }

        auto end = high_resolution_clock::now();
        duration<double> elapsed = end - start;
        std::cout << "  Time taken: " << elapsed.count() << " seconds." << std::endl;
        std::cout << "  Active Orders: " << ob.getActiveOrderCount() << std::endl;
        std::cout << "  Pool Used: " << ob.getPoolUsedCount() << ", Free: " << ob.getPoolFreeCount() << std::endl;
    }

    // Test 4: Querying price levels
    {
        std::cout << "\nTest 4: Querying " << NUM_PRICE_QUERIES << " random price levels." << std::endl;
        auto start = high_resolution_clock::now();
        std::vector<double> active_prices = ob.getAllActivePrices();
        int found_levels_count = 0;
        size_t prices_count = active_prices.size();
        if (prices_count > 0) {
            std::uniform_int_distribution<size_t> price_idx_dist(0, prices_count - 1);
            for (int i = 0; i < NUM_PRICE_QUERIES; ++i) {
                double price_to_query = active_prices[price_idx_dist(rng)];
                const auto* orders = ob.getOrdersAtPrice(price_to_query);
                if (orders != nullptr) {
                    found_levels_count++;
                }
            }
            std::cout << "  Price levels found: " << found_levels_count << " out of " << NUM_PRICE_QUERIES << " queries." << std::endl;
        } else {
            std::cout << "  Warning: No active prices found to query. Price levels found: 0" << std::endl;
        }
        auto end = high_resolution_clock::now();
        duration<double> elapsed = end - start;
        std::cout << "  Time taken: " << elapsed.count() << " seconds." << std::endl;
    }

    // Test 5: Best Bid/Offer lookups
    {
        std::cout << "\nTest 5: Performing " << NUM_BBO_LOOKUPS << " Best Bid/Offer lookups." << std::endl;
        auto start = high_resolution_clock::now();

        int bid_found_count = 0;
        int ask_found_count = 0;

        for (int i = 0; i < NUM_BBO_LOOKUPS; ++i) {
            auto best_bid = ob.getBestBidLevel();
            if (best_bid.second != nullptr) {
                bid_found_count++;
            }

            auto best_ask = ob.getBestAskLevel();
            if (best_ask.second != nullptr) { 
                ask_found_count++;
            }
        }

        auto end = high_resolution_clock::now();
        duration<double> elapsed = end - start;
        std::cout << "  Time taken: " << elapsed.count() << " seconds." << std::endl;
        std::cout << "  Best Bids found: " << bid_found_count << std::endl;
        std::cout << "  Best Offers found: " << ask_found_count << std::endl;
    }
}

// Specialized benchmark for UnoptimizedOrderBook
void runUnoptimizedBenchmark(const std::vector<std::tuple<std::string, double, int, bool>>& orders) {
    const int NUM_ORDERS_ADD = orders.size();
    const int NUM_ORDERS_MODIFY = NUM_ORDERS_ADD / 2;
    const int NUM_ORDERS_DELETE = NUM_ORDERS_ADD / 4;
    const int NUM_PRICE_QUERIES = 10000;
    const int NUM_BBO_LOOKUPS = 10000;

    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::vector<std::string> order_ids;
    order_ids.reserve(NUM_ORDERS_ADD);

    UnoptimizedOrderBook ob;

    // Test 1: Adding orders
    {
        std::cout << "\nTest 1: Adding " << NUM_ORDERS_ADD << " orders." << std::endl;
        auto start = high_resolution_clock::now();

        for (const auto& [id, price, qty, isBuy] : orders) {
            ob.addOrder(id, price, qty, isBuy);
            order_ids.push_back(id);
        }

        auto end = high_resolution_clock::now();
        duration<double> elapsed = end - start;
        std::cout << "  Time taken: " << elapsed.count() << " seconds." << std::endl;
        std::cout << "  Active Orders: " << ob.getActiveOrderCount() << std::endl;
    }

    // Test 2: Modifying orders
    {
        std::cout << "\nTest 2: Modifying " << NUM_ORDERS_MODIFY << " random orders." << std::endl;
        auto start = high_resolution_clock::now();

        std::uniform_int_distribution<size_t> id_idx_dist(0, order_ids.size() - 1);
        std::uniform_real_distribution<double> new_price_dist(94.0, 106.0); 
        std::uniform_int_distribution<int> new_qty_dist(1, 1200);

        for (int i = 0; i < NUM_ORDERS_MODIFY; ++i) {
            std::string id_to_modify = order_ids[id_idx_dist(rng)];
            double newPrice = new_price_dist(rng);
            int newQuantity = new_qty_dist(rng);
            ob.modifyOrder(id_to_modify, newPrice, newQuantity);
        }

        auto end = high_resolution_clock::now();
        duration<double> elapsed = end - start;
        std::cout << "  Time taken: " << elapsed.count() << " seconds." << std::endl;
        std::cout << "  Active Orders: " << ob.getActiveOrderCount() << std::endl;
    }

    // Test 3: Deleting orders
    {
        std::cout << "\nTest 3: Deleting " << NUM_ORDERS_DELETE << " random orders." << std::endl;
        auto start = high_resolution_clock::now();

        std::uniform_int_distribution<size_t> id_idx_dist(0, order_ids.size() - 1);
        std::vector<std::string> ids_to_delete;
        ids_to_delete.reserve(NUM_ORDERS_DELETE);

        std::unordered_set<size_t> indices_to_delete;
        while(indices_to_delete.size() < (size_t)NUM_ORDERS_DELETE && indices_to_delete.size() < order_ids.size()) {
            indices_to_delete.insert(id_idx_dist(rng));
        }
        for(size_t idx : indices_to_delete) {
            ids_to_delete.push_back(order_ids[idx]);
        }

        for (const auto& id : ids_to_delete) {
            ob.deleteOrder(id);
        }

        auto end = high_resolution_clock::now();
        duration<double> elapsed = end - start;
        std::cout << "  Time taken: " << elapsed.count() << " seconds." << std::endl;
        std::cout << "  Active Orders: " << ob.getActiveOrderCount() << std::endl;
    }

    // Test 4: Querying price levels
    {
        std::cout << "\nTest 4: Querying " << NUM_PRICE_QUERIES << " random price levels." << std::endl;
        auto start = high_resolution_clock::now();
        std::vector<double> active_prices = ob.getAllActivePrices();
        int found_levels_count = 0;
        size_t prices_count = active_prices.size();
        if (prices_count > 0) {
            std::uniform_int_distribution<size_t> price_idx_dist(0, prices_count - 1);
            for (int i = 0; i < NUM_PRICE_QUERIES; ++i) {
                double price_to_query = active_prices[price_idx_dist(rng)];
                const auto* orders = ob.getOrdersAtPrice(price_to_query);
                if (orders != nullptr) {
                    found_levels_count++;
                }
            }
            std::cout << "  Price levels found: " << found_levels_count << " out of " << NUM_PRICE_QUERIES << " queries." << std::endl;
        } else {
            std::cout << "  Warning: No active prices found to query. Price levels found: 0" << std::endl;
        }
        auto end = high_resolution_clock::now();
        duration<double> elapsed = end - start;
        std::cout << "  Time taken: " << elapsed.count() << " seconds." << std::endl;
    }

    // Test 5: Best Bid/Offer lookups
    {
        std::cout << "\nTest 5: Performing " << NUM_BBO_LOOKUPS << " Best Bid/Offer lookups." << std::endl;
        auto start = high_resolution_clock::now();

        int bid_found_count = 0;
        int ask_found_count = 0;

        for (int i = 0; i < NUM_BBO_LOOKUPS; ++i) {
            auto best_bid = ob.getBestBidLevel();
            if (best_bid.second != nullptr) {
                bid_found_count++;
            }

            auto best_ask = ob.getBestAskLevel();
            if (best_ask.second != nullptr) { 
                ask_found_count++;
            }
        }

        auto end = high_resolution_clock::now();
        duration<double> elapsed = end - start;
        std::cout << "  Time taken: " << elapsed.count() << " seconds." << std::endl;
        std::cout << "  Best Bids found: " << bid_found_count << std::endl;
        std::cout << "  Best Offers found: " << ask_found_count << std::endl;
    }
}

int main() {
    const size_t NUM_ORDERS = 100000;
    std::cout << "Generating " << NUM_ORDERS << " random orders..." << std::endl;
    auto orders = generateRandomOrders(NUM_ORDERS);

    std::cout << "\nBenchmarking Optimized OrderBook:" << std::endl;
    runOptimizedBenchmark(orders);

    std::cout << "\nBenchmarking Unoptimized OrderBook:" << std::endl;
    runUnoptimizedBenchmark(orders);

    return 0;
}
