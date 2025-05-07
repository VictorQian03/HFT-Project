#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <cassert>   
#include <stdexcept> 
#include <iomanip>   

#include "../include/orderbook.h"
#include "../include/optimized_orderbook.h"

void run_original(){
    std::cout << "---Running Original Order Book---" << std::endl;
    OrderBook original_order_book;

    std::cout << "adding Buy order 001, bid 100.0 for 30" << std::endl;
    original_order_book.addOrder("001", 100.0, 30, true);
    std::cout << "adding Buy order 002, bid 105.0 for 45" << std::endl;
    original_order_book.addOrder("002", 105.0, 45, true);

    std::cout << "adding Sell order 003, for 45 at 120.0" << std::endl;
    original_order_book.addOrder("003", 120.0, 45, false);
    std::cout << "adding Sell order 004, for 50 at 125.0" << std::endl;
    original_order_book.addOrder("004", 125.0, 50, false);

    std::cout << "\nInitial Order Book:" << std::endl;
    original_order_book.displayFullBook();

    std::cout << "\nModifying Buy order 002 to price 102.0 and quantity 60" << std::endl;
    original_order_book.modifyOrder("002", 102.0, 60);
    original_order_book.displayFullBook();

    std::cout << "\nModifying Sell order 004 to price 123.0 and quantity 40" << std::endl;
    original_order_book.modifyOrder("004", 123.0, 40);
    original_order_book.displayFullBook();

    std::cout << "\nDeleting Buy order 001" << std::endl;
    original_order_book.deleteOrder("001");
    original_order_book.displayFullBook();

    std::cout << "\nDeleting Sell order 003" << std::endl;
    original_order_book.deleteOrder("003");
    original_order_book.displayFullBook();
}

void run_optimized(){
    std::cout << "---Running Optimized Order Book---" << std::endl;
    OptimizedOrderBook optimized_order_book(10);

    std::cout << "adding Buy order 001, bid 100.0 for 30" << std::endl;
    optimized_order_book.addOrder("001", 100.0, 30, true);
    std::cout << "adding Buy order 002, bid 105.0 for 45" << std::endl;
    optimized_order_book.addOrder("002", 105.0, 45, true);

    std::cout << "adding Sell order 003, for 45 at 120.0" << std::endl;
    optimized_order_book.addOrder("003", 120.0, 45, false);
    std::cout << "adding Sell order 004, for 50 at 125.0" << std::endl;
    optimized_order_book.addOrder("004", 125.0, 50, false);

    std::cout << "\nInitial Order Book:" << std::endl;
    optimized_order_book.displayFullBook();

    std::cout << "\nModifying Buy order 002 to price 102.0 and quantity 60" << std::endl;
    optimized_order_book.modifyOrder("002", 102.0, 60);
    optimized_order_book.displayFullBook();

    std::cout << "\nModifying Sell order 004 to price 123.0 and quantity 40" << std::endl;
    optimized_order_book.modifyOrder("004", 123.0, 40);
    optimized_order_book.displayFullBook();

    std::cout << "\nDeleting Buy order 001" << std::endl;
    optimized_order_book.deleteOrder("001");
    optimized_order_book.displayFullBook();

    std::cout << "\nDeleting Sell order 003" << std::endl;
    optimized_order_book.deleteOrder("003");
    optimized_order_book.displayFullBook();
}


int main(){
    run_original();
    run_optimized();
    return 0;
};