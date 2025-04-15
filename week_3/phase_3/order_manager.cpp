//
// Created by Amy Wang on 4/14/25.
//

#include "order_manager.h"

OrderManager::OrderManager() {
    }

void OrderManager::handle_fill(int order_id, int fill_quantity) {
    auto it = orders.find(order_id);
    if (it != orders.end()) {
        MyOrder* order = it->second.get();
        order->filled += fill_quantity;
        if (order->filled >= order->quantity) {
            order->status = OrderStatus::Filled;
        } else {
            order->status = OrderStatus::PartiallyFilled;
        }
    }
}