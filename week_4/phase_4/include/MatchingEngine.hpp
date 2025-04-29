#pragma once
#include <algorithm>
namespace MatchingEngine {

template<typename BidMap, typename AskMap>
void matchTop(BidMap& bids, AskMap& asks) {
    if (bids.empty() || asks.empty()) return;
    auto bid_it = bids.begin();
    auto ask_it = asks.begin();
    // match when best bid ≥ best ask
     if (bid_it->first >= ask_it->first) {
        auto& buyOrder = bid_it->second;
        auto& sellOrder = ask_it->second;
       
        // Determine matched quantity
        auto matchedQty = std::min(buyOrder->quantity, sellOrder->quantity);
        
        // Update quantities
        buyOrder->quantity  -= matchedQty;
        sellOrder->quantity -= matchedQty;
        
        // Erase fully filled orders
        if (buyOrder->quantity <= 0) {
            bids.erase(bid_it);
            
        }
        if (sellOrder->quantity <= 0) {
            asks.erase(ask_it);
        }
    }
}

} 
