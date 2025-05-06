#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include <string>
#include <map>
#include <unordered_map>
#include <utility> 

struct Order {
    std::string id;  
    double price;    
    int quantity;   
    bool isBuy;     

    Order(std::string oid = "", double p = 0.0, int q = 0, bool side = true)
        : id(std::move(oid)), price(p), quantity(q), isBuy(side) {}
};

class OrderBook {
private:
    std::map<double, std::unordered_map<std::string, Order>> orderLevels;

    std::unordered_map<std::string, Order> orderLookup;

public:
    OrderBook(); 

    void addOrder(const std::string& id, double price, int quantity, bool isBuy);

    void modifyOrder(const std::string& id, double newPrice, int newQuantity);

    void deleteOrder(const std::string& id);

    bool getOrder(const std::string& id, Order& outOrder) const;

    void displayFullBook() const;
};

#endif