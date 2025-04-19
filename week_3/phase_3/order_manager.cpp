
#include "order_manager.h"
#include "logger.h"
#include <sstream>
using namespace std;

OrderManager::OrderManager(){};

int OrderManager::place_order(Side side, double price, int quantity) {
    static int curr_id = 1;
    int id = curr_id;
    ++curr_id;

    std::unique_ptr<MyOrder> order = std::make_unique<MyOrder>();
    order->id = id;
    order->price = price;
    order->quantity = quantity;
    order->side = side;

    orders[id] = std::move(order);

    Logger::get_instance()->log("Placed Order ID " + std::to_string(id));
    return id;
}

void OrderManager::cancel(int id) {
    if (orders.count(id) > 0) {
        orders[id]->status = OrderStatus::Cancelled;
        orders[id].reset();
        orders.erase(id);
        Logger::get_instance()->log("Order ID " + std::to_string(id) + " successfully cancelled");
    }
    else {
        Logger::get_instance()->log("Order ID " + std::to_string(id) + " not cancelled: not found");
    }
}

void OrderManager::handle_fill(int id, int filled_quantity) { 
    if (orders.count(id) > 0) {
        orders[id]->filled += filled_quantity;
        if (orders[id]->filled >= orders[id]->quantity){
            orders[id]->status = OrderStatus::Filled;
            Logger::get_instance()->log("Order ID " + std::to_string(id) + " fully filled");
            orders[id].reset();
            orders.erase(id);
        }
        else {
            orders[id]->status = OrderStatus::PartiallyFilled;
            Logger::get_instance()->log("Order ID " + std::to_string(id) + " partially filled");
        }
    }
    else {
        Logger::get_instance()->log("Order ID " + std::to_string(id) + " not found");
    }
}

void OrderManager::print_active_orders() const {
    for (const auto& entry : orders) {
        int id = entry.first;
        const auto& order = entry.second;
        std::ostringstream oss;
        oss << "Order " << id << ": "
            << "Price=" << order->price
            << ", Quantity=" << order->quantity
            << ", Filled=" << order->filled
            << ", Status=" << static_cast<int>(order->status);

        Logger::get_instance()->log(oss.str());
    }
}
