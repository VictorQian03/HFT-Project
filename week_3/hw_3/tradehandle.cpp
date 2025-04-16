#include "tradehandle.h"
#include <iostream>
#include <cassert>

// 3.1
TradeHandle::TradeHandle(Trade* p) : ptr(p) {
}

TradeHandle::~TradeHandle() {
    delete ptr;
    std::cout << "TradeHandle destructor called, Trade object deleted." << std::endl;
}

TradeHandle::TradeHandle(TradeHandle&& other) : ptr(other.ptr) { 
    other.ptr = nullptr; 
}

TradeHandle& TradeHandle::operator=(TradeHandle&& other) {
    if(this != &other) {
        delete ptr;
        ptr = other.ptr;
        other.ptr = nullptr;
    }
    return *this;
}

Trade* TradeHandle::operator->(){
    assert(ptr != nullptr);
    return ptr;
}

Trade& TradeHandle::operator*(){
    assert(ptr != nullptr);
    return *ptr;
}

/*
1. We delete the copy constructor because we would like to enforce unique ownership and 
avoid double deletion of the same object.
2. Move semantics are allowed because they provide a mechanism to transfer ownership 
safely without violating the unique ownership principle.
3. If we don't define a destructor, the compiler will implicitly generate a default one.
However, since ptr is a raw pointer and not an object with its own destructor that manages memory,
the default destructor will not call delete ptr;. So the Trade object that was allocated  
and passed to the TradeHandle's constructor will never be deallocated, which leads to a memory leak.
*/