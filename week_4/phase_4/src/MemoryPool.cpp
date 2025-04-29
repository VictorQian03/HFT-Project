#include "../include/MemoryPool.hpp"
#include <new>
#include <stdexcept>
#include <cassert>

// Constructor: Initializes memory pool with specific block size and pool size
MemoryPool::MemoryPool(std::size_t blockSize_param,
                       std::size_t poolSize_param)
    : blockSize(blockSize_param),
      poolSize(poolSize_param),
      pool(nullptr),
      freeList(nullptr)
{
    // 1) Allocate one big chunk
    pool = ::operator new(blockSize * poolSize);

    // 2) Carve it into a singly‐linked free list
    auto  buffer = static_cast<char*>(pool);
    for (std::size_t i = 0; i < poolSize; ++i) {
        // compute address of this slot
        void** slot = reinterpret_cast<void**>(buffer + i * blockSize);
        // link to next slot
        if (i + 1 < poolSize) {
            *slot = buffer + (i + 1) * blockSize;
        } else {
            *slot = nullptr;
        }
    }
    // head of free list is the first slot
    freeList = reinterpret_cast<void**>(buffer);
}

// Destructor: tear down the pool
MemoryPool::~MemoryPool() {
    ::operator delete(pool);
}

// Allocate one block; pop from free list
void* MemoryPool::allocate() {
    if (freeList == nullptr) {
        throw std::bad_alloc();  // pool exhausted
    }
    // take the head
    void* block = freeList;
    // advance to next free slot
    freeList = reinterpret_cast<void**>(*freeList);
    return block;
}

// Return a block back to the free list
void MemoryPool::deallocate(void* pointer) {
    if (pointer == nullptr) {
        return;
    }
    // push it back onto the front
    void** slot = reinterpret_cast<void**>(pointer);
    *slot      = freeList;
    freeList   = slot;
}