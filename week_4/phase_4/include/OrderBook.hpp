// orderbook should have

#pragma once
#include <map>
#include "Order.hpp"

using OrderType = Order<double, int>;

enum class Side { Bids, Asks };

class OrderBook{
private:
    std::map<Side, std::multimap<float, OrderType>> orderbook;
};
