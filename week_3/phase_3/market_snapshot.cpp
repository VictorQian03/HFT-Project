using namespace std;
#include <iostream>
#include "logger.h"
#include "market_snapshot.h"
#include <sstream>
#include <iomanip>

std::string format_price(double price) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << price;
    return ss.str();
}

MarketSnapshot::MarketSnapshot() {
  bids = std::map<double, std::unique_ptr<PriceLevel>, std::greater<double>>();  //descending
  asks = std::map<double, std::unique_ptr<PriceLevel>>(); //ascending

}

const PriceLevel* MarketSnapshot::get_best_bid() const {
    if (bids.empty())
        return nullptr;
    return bids.begin()->second.get();
}

const PriceLevel* MarketSnapshot::get_best_ask() const {
    if (asks.empty())
        return nullptr;
    return asks.begin()->second.get();
}

void MarketSnapshot::update_bid(double price, int qty) {
    auto it = bids.find(price);
    if (it != bids.end()) {  // it is an existing bid, can be removed or updated
        bool is_best_bid = (it == bids.begin());  // Check if it was the best bid before modification
        if (qty == 0) {
            bids.erase(it);  // Remove bid
            if (is_best_bid) {
                Logger::get_instance()->log("[Market] Best Bid: " + format_price(price) + " Removed");
            } else {
                Logger::get_instance()->log("[Market] Bid: " + format_price(price) + " Removed");
            }
        }
        else {
            it->second->quantity = qty;  // Update existing bid's quantity
            if (is_best_bid) {
                Logger::get_instance()->log("[Market] Best Bid: " + format_price(price) + " Updated with quantity: "+std::to_string(qty));
            } else {
                Logger::get_instance()->log("[Market] Bid: " + format_price(price) +  " Updated with quantity: "+std::to_string(qty));
            }
        }
    } else {
        // New bid
        bids[price] = std::make_unique<PriceLevel>(price, qty);

        // Check if this is the best bid (either first bid or better than current best)
        if (bids.size() == 1 || price ==bids.begin()->first) {

            Logger::get_instance()->log("[Market] Best Bid: " + format_price(price) + "x" + std::to_string(qty));
        } else {
            Logger::get_instance()->log("[Market] New Bid: " + format_price(price) + "x" + std::to_string(qty));

        }
    }
}


void MarketSnapshot::update_ask(double price, int qty) {
    auto it = asks.find(price);
    if (it != asks.end()) {  // it is an existing ask, can be removed or updated
        bool is_best_ask = (it == asks.begin());  // Check if it was the best ask before modification
        if (qty == 0) {
            asks.erase(it);  // Remove ask
            if (is_best_ask) {
                Logger::get_instance()->log("[Market] Best Ask: " + format_price(price) + " Removed");

            } else {
                Logger::get_instance()->log("[Market] Ask: " + format_price(price) + " Removed");
            }
        }
        else {
            it->second->quantity = qty;  // Update existing ask's quantity
            if (is_best_ask) {
                Logger::get_instance()->log("[Market] Best Ask: " + format_price(price) + " Updated with quantity: "+std::to_string(qty));
            } else {
                Logger::get_instance()->log("[Market] Ask: " + format_price(price) +  " Updated with quantity: "+std::to_string(qty));
            }
        }
    } else {
        // New ask
        asks[price] = std::make_unique<PriceLevel>(price, qty);

        // Check if this is the best ask (either first ask or better than current best)
        if (asks.size() == 1 || price == asks.begin()->first) {

            Logger::get_instance()->log("[Market] Best Ask: " + format_price(price) + "x" + std::to_string(qty));
        } else {
            Logger::get_instance()->log("[Market] New Ask: " + format_price(price) + "x" + std::to_string(qty));
        }
    }
}

