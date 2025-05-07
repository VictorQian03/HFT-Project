#include "../include/order_priority_queue.hpp"

template<typename T, typename Compare>
void OrderPriorityQueue<T, Compare>::push(const T& value) {
    heap_.push_back(value);
    heapify_up(heap_.size() - 1);
}

template<typename T, typename Compare>
void OrderPriorityQueue<T, Compare>::pop() {
    if (empty()) {
        throw std::runtime_error("Priority queue is empty");
    }
    
    // Move the last element to the root
    heap_[0] = std::move(heap_.back());
    heap_.pop_back();
    
    if (!empty()) {
        heapify_down(0);
    }
}

template<typename T, typename Compare>
const T& OrderPriorityQueue<T, Compare>::top() const {
    if (empty()) {
        throw std::runtime_error("Priority queue is empty");
    }
    return heap_[0];
}

template<typename T, typename Compare>
void OrderPriorityQueue<T, Compare>::heapify_up(size_t index) {
    while (index > 0) {
        size_t parent_idx = parent(index);
        if (compare_(heap_[parent_idx], heap_[index])) {
            std::swap(heap_[parent_idx], heap_[index]);
            index = parent_idx;
        } else {
            break;
        }
    }
}

template<typename T, typename Compare>
void OrderPriorityQueue<T, Compare>::heapify_down(size_t index) {
    while (has_left_child(index)) {
        size_t max_child = left_child(index);
        
        // Check if right child exists and is greater than left child
        if (has_right_child(index) && 
            compare_(heap_[max_child], heap_[right_child(index)])) {
            max_child = right_child(index);
        }
        
        // If current node is greater than or equal to max child, we're done
        if (!compare_(heap_[index], heap_[max_child])) {
            break;
        }
        
        // Swap with the greater child
        std::swap(heap_[index], heap_[max_child]);
        index = max_child;
    }
}

// Explicit template instantiations
template class OrderPriorityQueue<Order>;
template class OrderPriorityQueue<Order, std::greater<Order>>;
template class OrderPriorityQueue<Order, OrderCompare>; 