#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <vector>
#include <stdexcept> 
#include <list>      
#include <memory>   
#include <iostream>  

template <typename T>
class ObjectPool {
private:
    std::vector<T> pool_;
    std::list<size_t> free_indices_; 
    size_t capacity_;
    size_t used_count_;

public:
    ObjectPool(size_t initial_capacity = 0) : capacity_(0), used_count_(0) {
        if (initial_capacity > 0) {
            reserve(initial_capacity);
        }
    }

    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) return;

        pool_.resize(new_capacity); 
        for (size_t i = capacity_; i < new_capacity; ++i) {
            free_indices_.push_back(i); 
        }
        capacity_ = new_capacity;
    }

    template <typename... Args>
    T* allocate(Args&&... args) {
        if (free_indices_.empty()) {
            throw std::runtime_error("ObjectPool: Out of memory. No free objects.");
        }

        size_t index = free_indices_.front();
        free_indices_.pop_front();

        T* obj_ptr = new (&pool_[index]) T(std::forward<Args>(args)...);
        
        used_count_++;
        return obj_ptr;
    }

    void deallocate(T* obj_ptr) {
        if (!obj_ptr) return;

        if (obj_ptr < &pool_[0] || obj_ptr > &pool_[capacity_ -1]) {
            std::cerr << "Warning: Attempting to deallocate an object not from this pool." << std::endl;
            return;
        }

        obj_ptr->~T();

        size_t index = static_cast<size_t>(obj_ptr - &pool_[0]);

        free_indices_.push_back(index);
        used_count_--;
    }

    size_t capacity() const { return capacity_; }
    size_t used_count() const { return used_count_; }
    size_t free_count() const { return free_indices_.size(); }
};

#endif 