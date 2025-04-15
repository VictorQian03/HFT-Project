
#include <map>
class PriceLevel {
public:
    double price;
    int quantity;
    PriceLevel(double p, int q) : price(p), quantity(q) { }
};


class MarketSnapshot {
    std::map<double, std::unique_ptr<PriceLevel>, std::greater<double>> bids;
    std::map<double, std::unique_ptr<PriceLevel>> asks;
public:
    MarketSnapshot();
    const PriceLevel* get_best_bid() const;
    const PriceLevel* get_best_ask() const;
    void update_bid(double price, int qty);
    void update_ask(double price, int qty);


};