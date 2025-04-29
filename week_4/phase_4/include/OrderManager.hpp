#pragma once
#include "Order.hpp"
#include "OrderBook.hpp"
#include <unordered_map>
#include <memory>  
#include <string>
#include <ostream>
#include <stdexcept> 
#include <type_traits> 

enum class OrderState {
    New,
    Filled,
    PartiallyFilled,
    Cancelled,
    Invalid
};

inline std::string to_string(OrderState state) {
    switch (state) {
        case OrderState::New: return "New";
        case OrderState::PartiallyFilled: return "PartiallyFilled";
        case OrderState::Filled: return "Filled";
        case OrderState::Cancelled: return "Cancelled";
        default: return "Invalid";
    }
}

inline std::ostream& operator<<(std::ostream& os, OrderState state) {
    return os << to_string(state);  // reuse the above function
}

template <typename PriceType, typename OrderIdType>
class OrderManager {
    static_assert(std::is_integral<OrderIdType>::value, "Order ID must be an integer");

private:
    using OrderPtr = std::shared_ptr<Order<PriceType, OrderIdType>>;
    std::unordered_map<OrderIdType, OrderPtr> orders;
    std::unordered_map<OrderIdType, OrderState> states;

public:
    OrderManager() = default;
    OrderPtr getOrder(OrderIdType id) {
        auto it = orders.find(id);
        return (it != orders.end() ? it->second : nullptr);
    }

    OrderState getState(OrderIdType id) {
        auto it = states.find(id);
        return (it != states.end() ? it->second : OrderState::Invalid);
    }

    bool addOrder(OrderPtr& order_ptr) {
        if (!order_ptr) 
            return false;
        auto id = order_ptr->id;
        if (orders.count(id) > 0) 
            return false;
        orders.emplace(id, order_ptr);
        states.emplace(id, OrderState::New);
        return true;
    }

    bool cancelOrder(OrderIdType id) {
        auto oit = orders.find(id);
        if (oit == orders.end()) 
            return false;
        states[id] = OrderState::Cancelled;
        orders.erase(oit);
        return true;
    }

    bool updateOrderFill(OrderIdType id, int filled_quantity) {
        auto oit = orders.find(id);
        if (oit == orders.end()) 
            return false;

        auto sit = states.find(id);
        if (sit == states.end() ||
            sit->second == OrderState::Cancelled ||
            sit->second == OrderState::Filled)
        {
            return false;
        }

        const auto& order = oit->second;
        if (filled_quantity < 0 || filled_quantity > order->quantity) 
            return false;

        if (filled_quantity == order->quantity) {
            sit->second = OrderState::Filled;
            orders.erase(oit);
        }
        else if (filled_quantity > 0) {
            sit->second = OrderState::PartiallyFilled;
        }

        return true;
    }
};


