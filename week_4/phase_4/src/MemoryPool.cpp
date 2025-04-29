#include "MemoryPool.hpp"
#include <new>

// Constructor: Initializes  memory pool with specific block size and pool size
MemoryPool::MemoryPool(std::size_t blockSize, std::size_t poolSize)
    : blockSize(blockSize), poolSize(poolSize), pool(nullptr), freeList(nullptr) {
    // Allocate memory for the pool with size of blockSize * poolSize
    pool = ::operator new(blockSize * poolSize);

    // Initialize the free list-- link list
    freeList = reinterpret_cast<void**>(pool);
    void** current = freeList;
    for (std::size_t i = 0; i < poolSize - 1; ++i) {
        // uh oh hopefully this works
        *current = reinterpret_cast<void*>(reinterpret_cast<char*>(current) + blockSize);
        current = reinterpret_cast<void**>(*current);
    }
    *current = nullptr;  // Last block is nullptr
}

// Destructor: Deallocates the entire pool memory
MemoryPool::~MemoryPool() {
    ::operator delete(pool);
}

// Allocate memory for a single block from the pool
void* MemoryPool::allocate() {
    if (freeList == nullptr) {
        return nullptr;  // No free blocks left in the pool
    }

    // pop the first block from the free list
    void* block = freeList;
    freeList = *reinterpret_cast<void**>(freeList);  // Move the freeList pointer to the next block, this should be valid
    // again, hopefully this works
    return block;
}

// deallocate a block of memory and return it to the free list
void MemoryPool::deallocate(void* pointer) {
    // add the block back to the free list
    *reinterpret_cast<void**>(pointer) = freeList;  // Link the deallocated block to the free list
    freeList = pointer;  // update the free list to point to the newly freed block
}
