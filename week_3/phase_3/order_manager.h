
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
    int place_order(Side side, double price, int qty);
    void cancel(int id);
    void handle_fill(int id, int filled_qty);
    bool resolved_conflicts(Side side, double price, int quantity);
    void print_active_orders() const;
private:
    std::map<int, std::unique_ptr<MyOrder>> orders;
};

#endif