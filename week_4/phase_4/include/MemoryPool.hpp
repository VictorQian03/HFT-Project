#pragma once

#include <cstddef>

// not a template class, so implementation in .cpp file

class MemoryPool {
public:
    // Constructor
    MemoryPool(std::size_t blockSize, std::size_t poolSize);

    // Destructor
    ~MemoryPool();

    // Allocate memory from pool
    void* allocate();

    // Deallocate memory back to pool
    void deallocate(void* pointer);

private:
    void* pool;            // Raw memory for the pool
    void** freeList;       // Linked list of free blocks
    std::size_t blockSize; // Size of each block
    std::size_t poolSize;  // Number of blocks in the pool
};

