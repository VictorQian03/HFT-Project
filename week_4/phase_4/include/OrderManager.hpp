#pragma once
#include "Order.hpp"
#include "OrderBook.hpp"
#include <unordered_map>

enum class OrderState {
    New,
    Filled,
    PartiallyFilled,
    Cancelled,
    Invalid
};

std::string to_string(OrderState state) {
    switch (state) {
        case OrderState::New: return "New";
        case OrderState::PartiallyFilled: return "PartiallyFilled";
        case OrderState::Filled: return "Filled";
        case OrderState::Cancelled: return "Cancelled";
        default: return "Invalid";
    }
}

std::ostream& operator<<(std::ostream& os, OrderState state) {
    return os << to_string(state);  // reuse the above function
}

template <typename PriceType, typename OrderIdType>
class OrderManager {
    static_assert(std::is_integral<OrderIdType>::value, "Order ID must be an integer");

private:
    using OrderPtr = std::shared_ptr<Order<PriceType, OrderIdType>>;
    // using OrderPtr = std::unique_ptr<Order<PriceType, OrderIdType>>;
    std::unordered_map<OrderIdType, OrderPtr> orders;
    std::unordered_map<OrderIdType, OrderState> states;

public:
    OrderPtr getOrder(OrderIdType id) {
        auto new_ptr = nullptr;
        if (orders.contains(id)) {
            new_ptr = orders[id];
        }
        // should increase ref count for this order
        return new_ptr;
    }

    OrderState getState(OrderIdType id) {
        auto state = OrderState::Invalid;
        if (states.contains(id)) {
            state = states[id];
        }
        return state;
    }

    bool addOrder(OrderIdType id, OrderPtr ptr) {
        if (!(orders.contains(id) || states.contains(id))) {
            return false;
        }
        orders.insert({id, ptr});
        states.insert({id, OrderState::New});

        return true;
    }

    bool cancelOrder(OrderIdType id) {
        if (!(orders.contains(id) || states.contains(id))) {
            return false;
        }
        orders.erase(id);
        states[id] = OrderState::Cancelled;

        return true;
    }

    bool fillOrder(OrderIdType id, int quantity) {
        if (!orders.contains(id)) {
            return false;
        }

        auto total_quantity = orders[id]->quantity;
        if (quantity >= total_quantity) {
            states[id] = OrderState::Filled;
        }
        else {
            states[id] = OrderState::PartiallyFilled;
        }

        return true;
    }
};


