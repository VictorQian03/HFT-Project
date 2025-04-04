#include <iostream>
#include <vector>
#include <chrono>
#include <random>

const int SIZE = 4096;

// Basic function to access matrix elements
int getElement(const std::vector<std::vector<int>>& matrix, int row, int col) {
    return matrix[row][col];
}

// Optimized function to access matrix elements (not used)
inline int getElementOptimized(const std::vector<int>& row, int col) {
    return row[col];
}

// Basic function to add two integers
int add(int a, int b) {
    return a + b;
}

// Optimized function to add two integers
inline int addOptimized(int a, int b) {
    return a + b;
}

// Unoptimized summation function
long long sumMatrixBasic(const std::vector<std::vector<int>>& matrix) {
    long long sum = 0;
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            sum = add(sum, getElement(matrix, i, j));
        }
    }
    return sum;
}

// optimized summation function
long long sumMatrixOptimized(const std::vector<std::vector<int>>& matrix) {
    long long sum = 0;
    for(int i = 0; i < SIZE; ++i) {
        const int* rowPtr = matrix[i].data();
        for(int j = 0; j < SIZE; ++j) {
            sum = addOptimized(sum, rowPtr[j]);
        }
    }
    return sum;
}

int main() {
    // Generate a large random matrix
    std::vector<std::vector<int>> matrix(SIZE, std::vector<int>(SIZE));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(-100, 100);
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            matrix[i][j] = distrib(gen);
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    long long sum = sumMatrixBasic(matrix);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Basic Sum: " << sum << std::endl;
    std::cout << "Basic Time: " << duration.count() << " microseconds" << std::endl;

    // Students will implement their optimized version here
    auto start_optimized = std::chrono::high_resolution_clock::now();
    long long optimized_sum = sumMatrixOptimized(matrix);
    auto end_optimized = std::chrono::high_resolution_clock::now();
    auto duration_optimized = std::chrono::duration_cast<std::chrono::microseconds>(end_optimized - start_optimized);

    std::cout << "Optimized Sum: " << optimized_sum << std::endl;
    std::cout << "Optimized Time: " << duration_optimized.count() << " microseconds" << std::endl;

    return 0;
}

// g++ -O3 -march=native -std=c++20 -o matrix_sum matrix_sum.cpp

// On average, the optimized version should be faster than the basic version by about 150 microseconds on my hardware, which meets my target execution time.
// Example output:
/*
Basic Sum: 98151
Basic Time: 1232 microseconds
Optimized Sum: 98151
Optimized Time: 1083 microseconds
*/

/*
The optimizations that I made in the sumMatrixOptimized function help reduce overhead within the innermost loop: 
Inside the inner loop, the optimized version gets a direct pointer to the beginning of each row before the inner loop starts.
This is done via the line: const int* rowPtr = matrix[i].data();
The performance gain comes from eliminating the repeated overhead of vector element access within the inner loop by switching to 
direct pointer access for each row.
This single operation avoids repeatedly calling the vector’s subscript operator in the inner loop. While the compiler might inline such calls, 
explicitly using a pointer guarantees that I am directly accessing contiguous memory. 
The optimized version then accesses elements using pointer arithmetic via rowPtr[j]
Using the raw pointer rowPtr[j] translates more directly into efficient memory addressing instructions for the CPU. 
Sequential access allows the CPU to load entire cache lines efficiently. 
This reduces the number of cache misses compared to accessing each element via a function call like getElement(), 
which may involve additional overhead or less predictable memory access patterns.

It should be noted that the compiler automatically inlines small functions like getElement and add with optimization flag -O3 enabled (as well as loop unrolling and other optimizations).
Since I explicitly retrieve a pointer to a row and use inline functions, my optimizations ensure that the inner loop is as tight and efficient as possible. 
It makes the intended data access pattern clearer and reduce the risk of overlooked overhead, regardless of whether the compiler recognizes and optimizes the higher-level abstractions.
*/