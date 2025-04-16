#ifndef TRADE_H
#define TRADE_H
#include <string>

// 1.1
struct Trade {
    std::string symbol;
    double price;

    Trade(const std::string& sym, double p)
        : symbol(sym), price(p) {};
    
    Trade() : symbol(""), price(0.0) {};
};


#endif