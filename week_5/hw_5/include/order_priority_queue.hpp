#pragma once
#include <vector>
#include <functional>
#include <stdexcept>
#include <string>

struct Order {
    double price;
    int timestamp;
    std::string orderId; 
    
    Order(double p = 0.0, int ts = 0, std::string id = "")
        : price(p), timestamp(ts), orderId(std::move(id)) {}

    // Comparison operators
    bool operator<(const Order& other) const {
        return price < other.price;
    }

    bool operator>(const Order& other) const {
        return price > other.price;
    }

    bool operator==(const Order& other) const {
        return price == other.price && timestamp == other.timestamp && orderId == other.orderId;
    }
};

// Custom comparator for max heap (highest price first)
struct OrderCompare {
    bool operator()(const Order& a, const Order& b) const {
        return a.price < b.price;  // For max heap
    }
};

template<typename T, typename Compare = std::less<T>>
class OrderPriorityQueue {
public:
    explicit OrderPriorityQueue(const Compare& comp = Compare())
        : compare_(comp) {}

   
    void push(const T& value);
    void pop();
    const T& top() const;
    bool empty() const { return heap_.empty(); }
    size_t size() const { return heap_.size(); }
    void clear() { heap_.clear(); }

   
    void reserve(size_t capacity) { heap_.reserve(capacity); }
    const std::vector<T>& get_heap() const { return heap_; }

private:
    std::vector<T> heap_;
    Compare compare_;


    void heapify_up(size_t index);
    void heapify_down(size_t index);
    size_t parent(size_t index) const { return (index - 1) / 2; }
    size_t left_child(size_t index) const { return 2 * index + 1; }
    size_t right_child(size_t index) const { return 2 * index + 2; }
    bool has_left_child(size_t index) const { return left_child(index) < heap_.size(); }
    bool has_right_child(size_t index) const { return right_child(index) < heap_.size(); }
}; 