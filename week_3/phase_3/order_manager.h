
#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H
#include <map>

enum class OrderStatus { New, Filled, PartiallyFilled, Cancelled };

enum class Side { Buy, Sell };

struct MyOrder {
    int id;
    Side side;
    double price;
    int quantity;
    int filled = 0;
    OrderStatus status = OrderStatus::New;
};

class OrderManager {
public:
    OrderManager();
    int place_order(const std::string& symbol, double price, int quantity);
    void cancel_order(int order_id);
    void handle_fill(int order_id, int fill_quantity);
    void print_status() const;
private:
    std::unordered_map<int, std::unique_ptr<MyOrder>> orders;
};

#endif