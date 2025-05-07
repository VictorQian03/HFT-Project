#include "../include/object_pool.h"
#include "../include/optimized_orderbook.h"
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <chrono>
#include <string>
#include <numeric>
#include <unordered_set> 
#include <algorithm>

struct RandomOrderParams {
    std::string id;
    double price;
    int quantity;
    bool isBuy;
};

RandomOrderParams generate_random_order(int id_counter, std::mt19937& rng,
                                       std::uniform_real_distribution<double>& price_dist,
                                       std::uniform_int_distribution<int>& qty_dist,
                                       std::uniform_int_distribution<int>& side_dist) {
    RandomOrderParams params;
    params.id = "order_" + std::to_string(id_counter);
    params.price = price_dist(rng);
    params.quantity = qty_dist(rng);
    // 0 for Buy, 1 for Sell
    params.isBuy = (side_dist(rng) == 0);
    return params;
}

int main() {
    const size_t INITIAL_POOL_CAPACITY = 100000;
    const int NUM_ORDERS_ADD = 50000;
    const int NUM_ORDERS_MODIFY = 20000; 
    const int NUM_ORDERS_DELETE = 10000; 
    const int NUM_PRICE_QUERIES = 10000; 
    const int NUM_BBO_LOOKUPS = 10000; 

    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> price_dist(95.0, 105.0); 
    std::uniform_int_distribution<int> qty_dist(1, 1000); 
    std::uniform_int_distribution<int> side_dist(0, 1); 
    std::vector<std::string> order_ids;
    order_ids.reserve(NUM_ORDERS_ADD);

    std::cout << "Benchmarking OptimizedOrderBook:" << std::endl;
    {
        OptimizedOrderBook ob(INITIAL_POOL_CAPACITY);
        std::cout << "\nTest 1: Adding " << NUM_ORDERS_ADD << " orders." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < NUM_ORDERS_ADD; ++i) {
            RandomOrderParams p = generate_random_order(i, rng, price_dist, qty_dist, side_dist);
            ob.addOrder(p.id, p.price, p.quantity, p.isBuy);
            order_ids.push_back(p.id); 
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "  Time taken: " << elapsed.count() << " seconds." << std::endl;
        std::cout << "  Active Orders: " << ob.getActiveOrderCount() << std::endl;
        std::cout << "  Pool Used: " << ob.getPoolUsedCount() << ", Free: " << ob.getPoolFreeCount() << std::endl;
    } 

    OptimizedOrderBook ob(INITIAL_POOL_CAPACITY);
    order_ids.clear(); 
    for (int i = 0; i < NUM_ORDERS_ADD; ++i) {
         RandomOrderParams p = generate_random_order(i, rng, price_dist, qty_dist, side_dist);
         ob.addOrder(p.id, p.price, p.quantity, p.isBuy);
         order_ids.push_back(p.id);
    }
     std::shuffle(order_ids.begin(), order_ids.end(), rng); 

    {
        std::cout << "\nTest 2: Modifying " << NUM_ORDERS_MODIFY << " random orders." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        std::uniform_int_distribution<size_t> id_idx_dist(0, order_ids.size() - 1);
        std::uniform_real_distribution<double> new_price_dist(94.0, 106.0); 
        std::uniform_int_distribution<int> new_qty_dist(1, 1200);

        for (int i = 0; i < NUM_ORDERS_MODIFY; ++i) {
            std::string id_to_modify = order_ids[id_idx_dist(rng)];
            double newPrice = new_price_dist(rng);
            int newQuantity = new_qty_dist(rng);
            ob.modifyOrder(id_to_modify, newPrice, newQuantity);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "  Time taken: " << elapsed.count() << " seconds." << std::endl;
        std::cout << "  Active Orders: " << ob.getActiveOrderCount() << std::endl; 
        std::cout << "  Pool Used: " << ob.getPoolUsedCount() << ", Free: " << ob.getPoolFreeCount() << std::endl;
    }

    {
        std::cout << "\nTest 3: Deleting " << NUM_ORDERS_DELETE << " random orders." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

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

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "  Time taken: " << elapsed.count() << " seconds." << std::endl;
        std::cout << "  Active Orders: " << ob.getActiveOrderCount() << std::endl; 
        std::cout << "  Pool Used: " << ob.getPoolUsedCount() << ", Free: " << ob.getPoolFreeCount() << std::endl;
    }

    {
        std::cout << "\nTest 4: Querying " << NUM_PRICE_QUERIES << " random price levels." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
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
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "  Time taken: " << elapsed.count() << " seconds." << std::endl;
    }

    {
        std::cout << "\nTest 5: Performing " << NUM_BBO_LOOKUPS << " Best Bid/Offer lookups." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

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

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "  Time taken: " << elapsed.count() << " seconds." << std::endl;
        std::cout << "  Best Bids found: " << bid_found_count << std::endl;
        std::cout << "  Best Offers found: " << ask_found_count << std::endl;
    }

    std::cout << "\nBenchmarking complete." << std::endl;
    return 0;
}
