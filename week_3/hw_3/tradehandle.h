#ifndef TRADEHANDLE_H
#define TRADEHANDLE_H
#include "trade.h"

class TradeHandle {
    Trade* ptr;

    public:
        TradeHandle(Trade* p);
        ~TradeHandle();
        TradeHandle(const TradeHandle&) = delete;
        TradeHandle& operator=(const TradeHandle&) = delete;
        TradeHandle(TradeHandle&& other);
        TradeHandle& operator=(TradeHandle&& other);
        Trade* operator->();
        Trade& operator*();
};










# endif