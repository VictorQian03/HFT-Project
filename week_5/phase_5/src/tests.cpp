#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <cassert>   
#include <stdexcept> 
#include <iomanip>   

#include "../include/orderbook.h"
#include "../include/optimized_orderbook.h"
#include "../include/tests.h"


#define RUN_TEST(test_function) \
    std::cout << "Running " #test_function "..." << std::endl; \
    test_function(); \
    std::cout << #test_function " PASSED." << std::endl;

void test_original_add_order() {
    OrderBook book;
    book.addOrder("ORD001", 50.10, 100, true); 
    Order order_check;
    assert(book.getOrder("ORD001", order_check));
    assert(order_check.id == "ORD001");
    assert(order_check.price == 50.10);
    assert(order_check.quantity == 100);
    assert(order_check.isBuy == true);

    book.addOrder("ORD002", 50.20, 50, false); 
    assert(book.getOrder("ORD002", order_check));
    assert(order_check.id == "ORD002");
    assert(order_check.price == 50.20);
    assert(order_check.quantity == 50);
    assert(order_check.isBuy == false);
}

void test_original_delete_order() {
    OrderBook book;
    book.addOrder("ORD001", 50.10, 100, true);
    book.deleteOrder("ORD001");
    Order order_check;
    assert(!book.getOrder("ORD001", order_check)); 
}

void test_original_modify_order() {
    OrderBook book;
    book.addOrder("ORD001", 50.10, 100, true);
    book.modifyOrder("ORD001", 50.15, 150);
    Order order_check;
    assert(book.getOrder("ORD001", order_check));
    assert(order_check.price == 50.15);
    assert(order_check.quantity == 150);
    assert(order_check.isBuy == true); 

    book.modifyOrder("ORD001", 49.00, 25);
    assert(book.getOrder("ORD001", order_check));
    assert(order_check.price == 49.00);
    assert(order_check.quantity == 25);
}

void test_original_empty_level_cleanup() {
    OrderBook book;
    book.addOrder("ORD001", 50.10, 100, true);
    book.addOrder("ORD002", 50.10, 50, true); 
    book.deleteOrder("ORD001");
    Order temp;
    assert(book.getOrder("ORD002", temp));


    book.deleteOrder("ORD002");
}

void test_original_get_order() {
    OrderBook book;
    Order o;
    assert(!book.getOrder("NONEXIST", o)); 
    book.addOrder("ORD001", 50.10, 100, true);
    assert(book.getOrder("ORD001", o));
    assert(o.id == "ORD001");
}

void test_original_add_duplicate_id() {
    OrderBook book;
    book.addOrder("DUP001", 10.0, 10, true);
    book.addOrder("DUP001", 11.0, 20, true); 
    Order o;
    assert(book.getOrder("DUP001", o));
    assert(o.price == 11.0); 
    assert(o.quantity == 20);
}


void test_original_modify_nonexistent() {
    OrderBook book;
    book.modifyOrder("NONEXIST", 10.0, 10);
    Order o;
    assert(!book.getOrder("NONEXIST", o));
}

void test_original_delete_nonexistent() {
    OrderBook book;
    book.deleteOrder("NONEXIST");
    Order o;
    assert(!book.getOrder("NONEXIST", o));
}

void test_original_invalid_params() {
    OrderBook book;
    book.addOrder("", 10.0, 10, true); 
    Order o;
    assert(!book.getOrder("", o));

    book.addOrder("ID1", 0.0, 10, true); 
    assert(!book.getOrder("ID1", o));

    book.addOrder("ID2", 10.0, 0, true); 
    assert(!book.getOrder("ID2", o));

    book.addOrder("ID3", 10.0, 10, true);
    book.modifyOrder("ID3", 0.0, 10); 
    assert(book.getOrder("ID3",o));
    assert(o.price == 10.0);

    book.modifyOrder("ID3", 10.0, 0); 
    assert(book.getOrder("ID3",o));
    assert(o.quantity == 10); 
}

void test_optimized_add_order() {
    OptimizedOrderBook book(10); 
    book.addOrder("ORD001", 50.10, 100, true);
    OrderOpt order_check;
    assert(book.getOrder("ORD001", order_check));
    assert(order_check.id == "ORD001");
    assert(order_check.price == 50.10);
    assert(book.getActiveOrderCount() == 1);
    assert(book.getPoolUsedCount() == 1);

    book.addOrder("ORD002", 50.20, 50, false);
    assert(book.getOrder("ORD002", order_check));
    assert(order_check.id == "ORD002");
    assert(book.getActiveOrderCount() == 2);
    assert(book.getPoolUsedCount() == 2);
}

void test_optimized_delete_order() {
    OptimizedOrderBook book(10);
    book.addOrder("ORD001", 50.10, 100, true);
    book.deleteOrder("ORD001");
    OrderOpt order_check;
    assert(!book.getOrder("ORD001", order_check));
    assert(book.getActiveOrderCount() == 0);
    assert(book.getPoolUsedCount() == 0); 
    assert(book.getPoolFreeCount() == 10); 
}

void test_optimized_modify_order() {
    OptimizedOrderBook book(10);
    book.addOrder("ORD001", 50.10, 100, true);
    book.modifyOrder("ORD001", 50.15, 150); 
    OrderOpt order_check;
    assert(book.getOrder("ORD001", order_check));
    assert(order_check.price == 50.15);
    assert(order_check.quantity == 150);
    assert(order_check.isBuy == true);
    assert(book.getActiveOrderCount() == 1); 
    assert(book.getPoolUsedCount() == 1);  

    book.addOrder("ORD002", 60.00, 20, false);
    book.modifyOrder("ORD001", 60.00, 30); 
    assert(book.getOrder("ORD001", order_check));
    assert(order_check.price == 60.00);
    assert(order_check.quantity == 30);

}


void test_optimized_empty_level_cleanup() {
    OptimizedOrderBook book(10);
    book.addOrder("ORD001", 50.10, 100, true);
    book.addOrder("ORD002", 50.10, 50, true);
    assert(book.getActiveOrderCount() == 2);
    book.deleteOrder("ORD001");
    assert(book.getActiveOrderCount() == 1);
    OrderOpt temp;
    assert(book.getOrder("ORD002", temp)); 

    book.deleteOrder("ORD002");
    assert(book.getActiveOrderCount() == 0);
}

void test_optimized_get_order() {
    OptimizedOrderBook book(10);
    OrderOpt o;
    assert(!book.getOrder("NONEXIST", o));
    book.addOrder("ORD001", 50.10, 100, true);
    assert(book.getOrder("ORD001", o));
    assert(o.id == "ORD001");
}

void test_optimized_pool_usage() {
    OptimizedOrderBook book(2); 
    assert(book.getPoolUsedCount() == 0);
    assert(book.getPoolFreeCount() == 2);

    book.addOrder("O1", 10, 1, true);
    assert(book.getPoolUsedCount() == 1);
    assert(book.getPoolFreeCount() == 1);

    book.addOrder("O2", 10, 1, true);
    assert(book.getPoolUsedCount() == 2);
    assert(book.getPoolFreeCount() == 0);

    book.deleteOrder("O1");
    assert(book.getPoolUsedCount() == 1);
    assert(book.getPoolFreeCount() == 1);

    book.deleteOrder("O2");
    assert(book.getPoolUsedCount() == 0);
    assert(book.getPoolFreeCount() == 2);
}

void test_optimized_atomic_counter() {
    OptimizedOrderBook book(5);
    assert(book.getActiveOrderCount() == 0);
    book.addOrder("A1", 1.0, 1, true);
    assert(book.getActiveOrderCount() == 1);
    book.addOrder("A2", 1.0, 1, true);
    assert(book.getActiveOrderCount() == 2);
    book.deleteOrder("A1");
    assert(book.getActiveOrderCount() == 1);
    book.deleteOrder("A2");
    assert(book.getActiveOrderCount() == 0);
}

void test_optimized_add_duplicate_id() {
    OptimizedOrderBook book(5);
    book.addOrder("DUP001", 10.0, 10, true);
    book.addOrder("DUP001", 11.0, 20, true); 
    OrderOpt o;
    assert(book.getOrder("DUP001", o));
    assert(o.price == 10.0); 
    assert(o.quantity == 10);
    assert(book.getActiveOrderCount() == 1); 
}

void test_optimized_modify_nonexistent() {
    OptimizedOrderBook book(5);
    book.modifyOrder("NONEXIST", 10.0, 10);
    OrderOpt o;
    assert(!book.getOrder("NONEXIST", o));
    assert(book.getActiveOrderCount() == 0);
}

void test_optimized_delete_nonexistent() {
    OptimizedOrderBook book(5);
    book.deleteOrder("NONEXIST");
    OrderOpt o;
    assert(!book.getOrder("NONEXIST", o));
    assert(book.getActiveOrderCount() == 0);
}

void test_optimized_invalid_params() {
    OptimizedOrderBook book(10);
    book.addOrder("", 10.0, 10, true); 
    OrderOpt o;
    assert(!book.getOrder("", o) && "Empty ID add check");
    assert(book.getActiveOrderCount() == 0);

    book.addOrder("ID1", 0.0, 10, true); 
    assert(!book.getOrder("ID1", o) && "Zero price add check");
    assert(book.getActiveOrderCount() == 0);

    book.addOrder("ID2", 10.0, 0, true); 
    assert(!book.getOrder("ID2", o) && "Zero quantity add check");
    assert(book.getActiveOrderCount() == 0);

    book.addOrder("ID3", 10.0, 10, true);
    assert(book.getActiveOrderCount() == 1);
    book.modifyOrder("ID3", 0.0, 10); 
    assert(book.getOrder("ID3",o));
    assert(o.price == 10.0 && "Invalid price modify check"); 

    book.modifyOrder("ID3", 10.0, 0); 
    assert(book.getOrder("ID3",o));
    assert(o.quantity == 10 && "Invalid quantity modify check"); 
    assert(book.getActiveOrderCount() == 1);
}

void test_optimized_pool_exhaustion() {
    OptimizedOrderBook book(1); 
    book.addOrder("O1", 10, 1, true); 
    assert(book.getPoolFreeCount() == 0);

    bool exception_thrown = false;
    try {
        book.addOrder("O2", 11, 1, true); 
    } catch (const std::runtime_error& e) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    assert(book.getActiveOrderCount() == 1); 
    OrderOpt o;
    assert(!book.getOrder("O2", o));
}

struct StressOp {
    enum Type { ADD, MODIFY, DELETE } type;
    std::string id;
    double price;
    int quantity;
    bool isBuy;
};

std::vector<StressOp> generate_stress_operations(int num_operations, int& id_counter, std::vector<std::string>& existing_ids) {
    std::vector<StressOp> ops;
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> price_dist(90.0, 110.0);
    std::uniform_int_distribution<int> quantity_dist(1, 100);
    std::uniform_int_distribution<int> side_dist(0, 1);
    std::uniform_int_distribution<int> op_type_dist(0, 2);

    ops.reserve(num_operations);

    for (int i = 0; i < num_operations; ++i) {
        StressOp::Type op_type = static_cast<StressOp::Type>(op_type_dist(rng));
        StressOp current_op;
        current_op.price = std::round(price_dist(rng) * 100.0) / 100.0;
        current_op.quantity = quantity_dist(rng);
        current_op.isBuy = side_dist(rng) == 1;

        if (existing_ids.empty() || op_type == StressOp::ADD || (existing_ids.size() < 5 && op_type != StressOp::ADD) ) { 
            current_op.type = StressOp::ADD;
            current_op.id = "STRESS" + std::to_string(id_counter++);
        } else if (op_type == StressOp::MODIFY) {
            current_op.type = StressOp::MODIFY;
            std::uniform_int_distribution<size_t> id_idx_dist(0, existing_ids.size() - 1);
            current_op.id = existing_ids[id_idx_dist(rng)];
        } else { 
            current_op.type = StressOp::DELETE;
            std::uniform_int_distribution<size_t> id_idx_dist(0, existing_ids.size() - 1);
            current_op.id = existing_ids[id_idx_dist(rng)];
        }
        ops.push_back(current_op);
    }
    return ops;
}


void stress_test_original_book(OrderBook& book, int num_total_ops, bool verbose) {
    std::cout << "\n--- Stress Testing Original OrderBook with " << num_total_ops << " operations ---" << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    int id_counter = 0;
    std::vector<std::string> existing_ids; 

    int ops_done = 0;
    while(ops_done < num_total_ops) {
        int batch_size = std::min(1000, num_total_ops - ops_done); 
        std::vector<StressOp> current_batch = generate_stress_operations(batch_size, id_counter, existing_ids);

        for(const auto& op : current_batch) {
            if (verbose && ops_done % (num_total_ops / 100 == 0 ? 1 : num_total_ops/100) == 0 ) { 
                 std::cout << "Original Stress Op " << ops_done << ": " << op.id 
                           << (op.type == StressOp::ADD ? " ADD" : (op.type == StressOp::MODIFY ? " MOD" : " DEL"))
                           << " P=" << op.price << " Q=" << op.quantity << std::endl;
            }
            switch(op.type) {
                case StressOp::ADD:
                    book.addOrder(op.id, op.price, op.quantity, op.isBuy);
                    if (std::find(existing_ids.begin(), existing_ids.end(), op.id) == existing_ids.end()) {
                        existing_ids.push_back(op.id);
                    }
                    break;
                case StressOp::MODIFY:
                    book.modifyOrder(op.id, op.price, op.quantity);
                    break;
                case StressOp::DELETE:
                    book.deleteOrder(op.id);
                    existing_ids.erase(std::remove(existing_ids.begin(), existing_ids.end(), op.id), existing_ids.end());
                    break;
            }
            ops_done++;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end_time - start_time;
    std::cout << "Original OrderBook Stress Test completed " << ops_done << " operations in " << elapsed.count() << " ms." << std::endl;
    std::cout << "Original Book final order count (approx based on existing_ids): " << existing_ids.size() << std::endl;
}

void stress_test_optimized_book(OptimizedOrderBook& book, int num_total_ops, bool verbose) {
    std::cout << "\n--- Stress Testing OptimizedOrderBook with " << num_total_ops << " operations ---" << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    int id_counter = 0;
    std::vector<std::string> existing_ids;

    int ops_done = 0;
    while(ops_done < num_total_ops) {
        int batch_size = std::min(1000, num_total_ops - ops_done);
        std::vector<StressOp> current_batch = generate_stress_operations(batch_size, id_counter, existing_ids);

        for(const auto& op : current_batch) {
            if (verbose && ops_done % (num_total_ops / 100 == 0 ? 1 : num_total_ops/100) == 0 ) {
                 std::cout << "Optimized Stress Op " << ops_done << ": " << op.id 
                           << (op.type == StressOp::ADD ? " ADD" : (op.type == StressOp::MODIFY ? " MOD" : " DEL"))
                           << " P=" << op.price << " Q=" << op.quantity << std::endl;
            }
            try {
                switch(op.type) {
                    case StressOp::ADD: {
                        OrderOpt temp_check; 
                        if (!book.getOrder(op.id, temp_check)) { 
                            book.addOrder(op.id, op.price, op.quantity, op.isBuy);
                            existing_ids.push_back(op.id);
                        } else {
                        }
                        break;
                    }
                    case StressOp::MODIFY:
                        book.modifyOrder(op.id, op.price, op.quantity);
                        break;
                    case StressOp::DELETE:
                        book.deleteOrder(op.id);
                        existing_ids.erase(std::remove(existing_ids.begin(), existing_ids.end(), op.id), existing_ids.end());
                        break;
                }
            } catch (const std::runtime_error& e) {
                std::cerr << "Stress Test (Optimized): Caught runtime_error (likely pool exhaustion): " << e.what() << " for op ID " << op.id << std::endl;
            }
            ops_done++;
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end_time - start_time;
    std::cout << "OptimizedOrderBook Stress Test completed " << ops_done << " operations in " << elapsed.count() << " ms." << std::endl;
    std::cout << "Optimized Book final active order count: " << book.getActiveOrderCount() << std::endl;
    std::cout << "Optimized Book pool: Used=" << book.getPoolUsedCount() << ", Free=" << book.getPoolFreeCount() << std::endl;
}

void run_all_unit_tests() {
    std::cout << "\n===== Running Unit Tests for Original OrderBook =====" << std::endl;
    RUN_TEST(test_original_add_order);
    RUN_TEST(test_original_delete_order);
    RUN_TEST(test_original_modify_order);
    RUN_TEST(test_original_empty_level_cleanup);
    RUN_TEST(test_original_get_order);
    RUN_TEST(test_original_add_duplicate_id);
    RUN_TEST(test_original_modify_nonexistent);
    RUN_TEST(test_original_delete_nonexistent);
    RUN_TEST(test_original_invalid_params);
    std::cout << "===== Original OrderBook Unit Tests Complete =====" << std::endl;

    std::cout << "\n===== Running Unit Tests for OptimizedOrderBook =====" << std::endl;
    RUN_TEST(test_optimized_add_order);
    RUN_TEST(test_optimized_delete_order);
    RUN_TEST(test_optimized_modify_order);
    RUN_TEST(test_optimized_empty_level_cleanup);
    RUN_TEST(test_optimized_get_order);
    RUN_TEST(test_optimized_pool_usage);
    RUN_TEST(test_optimized_atomic_counter);
    RUN_TEST(test_optimized_add_duplicate_id);
    RUN_TEST(test_optimized_modify_nonexistent);
    RUN_TEST(test_optimized_delete_nonexistent);
    RUN_TEST(test_optimized_invalid_params);
    RUN_TEST(test_optimized_pool_exhaustion);
    std::cout << "===== OptimizedOrderBook Unit Tests Complete =====" << std::endl;
}


int main() {
    run_all_unit_tests();

    OrderBook original_book_for_stress;
    stress_test_original_book(original_book_for_stress, 50000); 

    int stress_ops = 50000;
    OptimizedOrderBook optimized_book_for_stress(stress_ops + 1000); // Pool size
    stress_test_optimized_book(optimized_book_for_stress, stress_ops);

    return 0;
}
