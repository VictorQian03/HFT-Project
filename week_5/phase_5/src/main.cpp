#include "../include/orderbook.h"
#include <iostream>

int main() {
    OrderBook book;

    std::cout << "Initial empty book:" << std::endl;
    book.displayFullBook();

    std::cout << "\nAdding orders..." << std::endl;
    book.addOrder("BUY001", 100.50, 10, true);   
    book.addOrder("SELL001", 101.00, 5,  false);  
    book.addOrder("BUY002", 100.50, 20, true);   
    book.addOrder("BUY003", 99.80,  15, true);  
    book.addOrder("SELL002", 101.00, 8,  false);  
    book.addOrder("SELL003", 101.20, 12, false);  
    book.addOrder("MIXED_LVL_BUY", 100.75, 5, true); 
    book.addOrder("MIXED_LVL_SELL", 100.75, 7, false); 

    book.displayFullBook();

    std::cout << "\nModifying BUY002 (100.50 Q20 Buy) to Price 100.60, Qty 25..." << std::endl;
    book.modifyOrder("BUY002", 100.60, 25);
    book.displayFullBook();

    std::cout << "\nModifying SELL001 (101.00 Q5 Sell) to Price 101.00, Qty 3 (same price, new quantity)..." << std::endl;
    book.modifyOrder("SELL001", 101.00, 3);
    book.displayFullBook();
    
    std::cout << "\nDeleting BUY001 (100.50 Q10 Buy)..." << std::endl;
    book.deleteOrder("BUY001");
    book.displayFullBook();

    std::cout << "\nDeleting SELL003 (101.20 Q12 Sell - last order at this price level)..." << std::endl;
    book.deleteOrder("SELL003"); 
    book.displayFullBook();

    std::cout << "\nAttempting to modify a non-existent order (XYZ)..." << std::endl;
    book.modifyOrder("XYZ", 200.0, 100);

    std::cout << "\nAttempting to delete a non-existent order (ABC)..." << std::endl;
    book.deleteOrder("ABC");
    
    std::cout << "\nChecking getOrder for BUY003:" << std::endl;
    Order foundOrder;
    if(book.getOrder("BUY003", foundOrder)) {
        std::cout << "Found: ID=" << foundOrder.id << ", P=" << foundOrder.price 
                  << ", Q=" << foundOrder.quantity << ", isBuy=" << (foundOrder.isBuy ? "YES" : "NO") << std::endl;
    } else {
        std::cout << "Order BUY003 not found." << std::endl;
    }

    std::cout << "\nAdding an order with an ID that was previously deleted (BUY001)..." << std::endl;
    book.addOrder("BUY001", 99.99, 50, true); 
    book.displayFullBook();

    std::cout << "\nDeleting SELL002 (101.00 Q8 Sell). SELL001 is also at 101.00 (Sell, Q3)..." << std::endl;
    book.deleteOrder("SELL002"); 
    book.displayFullBook();
    
    std::cout << "\nDeleting SELL001 (101.00 Q3 Sell). Now no Sells at 101.00..." << std::endl;
    book.deleteOrder("SELL001"); 
    book.displayFullBook();
    
    std::cout << "\nDeleting MIXED_LVL_SELL (100.75 Q7 Sell)..." << std::endl;
    book.deleteOrder("MIXED_LVL_SELL");
    book.displayFullBook(); 
    
    std::cout << "\nDeleting MIXED_LVL_BUY (100.75 Q5 Buy)..." << std::endl;
    book.deleteOrder("MIXED_LVL_BUY"); 
    book.displayFullBook();

    return 0;
}