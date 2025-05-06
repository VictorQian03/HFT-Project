#ifndef ORDER_BOOK_TESTS_H
#define ORDER_BOOK_TESTS_H

class OrderBook;
class OptimizedOrderBook; 

void run_all_unit_tests();

void test_original_add_order();
void test_original_delete_order();
void test_original_modify_order();
void test_original_empty_level_cleanup();
void test_original_get_order();
void test_original_add_duplicate_id();
void test_original_modify_nonexistent();
void test_original_delete_nonexistent();
void test_original_invalid_params();


void test_optimized_add_order();
void test_optimized_delete_order();
void test_optimized_modify_order();
void test_optimized_empty_level_cleanup();
void test_optimized_get_order();
void test_optimized_pool_usage();
void test_optimized_atomic_counter();
void test_optimized_add_duplicate_id();
void test_optimized_modify_nonexistent();
void test_optimized_delete_nonexistent();
void test_optimized_invalid_params();
void test_optimized_pool_exhaustion();

void stress_test_original_book(OrderBook& book, int num_operations, bool verbose = false);
void stress_test_optimized_book(OptimizedOrderBook& book, int num_operations, bool verbose = false);

#endif 