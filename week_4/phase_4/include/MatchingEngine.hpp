// MatchingEngine.hpp
#pragma once
#include <iostream>
#include <algorithm>  // for std::min

namespace MatchingEngine {

  template<typename BidMap, typename AskMap>
  void matchOrder(BidMap& bids, AskMap& asks) {
    // Keep matching while the best bid ≥ best ask
    while (!bids.empty() && !asks.empty()) {
      auto bid_it = bids.begin();
      auto ask_it = asks.begin();

      if (bid_it->first < ask_it->first)
        break;                        // no more crosses

      auto& buyOrder  = bid_it->second;
      auto& sellOrder = ask_it->second;

      // Determine how many contracts to trade
      auto matchedQty = std::min(buyOrder->quantity,
                                 sellOrder->quantity);

      
      buyOrder->quantity  -= matchedQty;
      sellOrder->quantity -= matchedQty;

      // Remove fully filled orders, otherwise leave partial
      if (buyOrder->quantity <= 0) {
        bids.erase(bid_it);
        std::cout<<"Bought on top of the ask"<<std::endl;
      }
      if (sellOrder->quantity <= 0) {
        asks.erase(ask_it);
        std::cout<<"Sold on top of bid"<<std::endl;
      }
     
    }
  }

}