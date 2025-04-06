#ifndef MARKET_DATA_FEED_H
#define MARKET_DATA_FEED_H

#include "market_data.h" 
#include <vector>
#include <chrono> 
using std::vector;

class MarketDataFeed {
public:
    MarketDataFeed(vector<MarketData>& ref);

    void generateData(int num_ticks);

private:
    vector<MarketData>& data;
};

#endif
