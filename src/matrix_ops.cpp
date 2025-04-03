#include "matrix_ops.h"
#include <iostream> 
#include <vector>   
#include <stdexcept> 
#include <cassert>   

inline void check_null(const void* ptr, const char* name) {
    if (!ptr) {
        throw std::invalid_argument(std::string(name) + " cannot be null.");
    }
}

// Implemented by Team Member 1
void multiply_mv_row_major(const double* matrix, int rows, int cols, const double* vector, double* result) {
    check_null(matrix, "matrix");
    check_null(vector, "vector");
    check_null(result, "result");
    std::cerr << "multiply_mv_row_major: Not implemented yet.\n"; // Placeholder message
}

// Implemented by Team Member 2
void multiply_mv_col_major(const double* matrix, int rows, int cols, const double* vector, double* result) {
    check_null(matrix, "matrix");
    check_null(vector, "vector");
    check_null(result, "result");
    std::cerr << "multiply_mv_col_major: Not implemented yet.\n"; // Placeholder message
}

// Implemented by Team Member 3
void multiply_mm_naive(const double* matrixA, int rowsA, int colsA, const double* matrixB, int rowsB, int colsB, double* result) {
    check_null(matrixA, "matrixA");
    check_null(matrixB, "matrixB");
    check_null(result, "result");
    if (colsA != rowsB) {
        throw std::invalid_argument("Incompatible dimensions for matrix multiplication.");
    }
    std::cerr << "multiply_mm_naive: Not implemented yet.\n"; // Placeholder message
}

// Implemented by Team Member 4
void multiply_mm_transposed_b(const double* matrixA, int rowsA, int colsA, const double* matrixB_transposed, int rowsB, int colsB, double* result) {
    check_null(matrixA, "matrixA");
    check_null(matrixB_transposed, "matrixB_transposed");
    check_null(result, "result");
    if (colsA != rowsB) { 
        throw std::invalid_argument("Incompatible dimensions for matrix multiplication (A * B^T).");
    }
    std::cerr << "multiply_mm_transposed_b: Not implemented yet.\n"; // Placeholder message
}

// Implemented by the team collaboratively
void multiply_mm_optimized(const double* matrixA, int rowsA, int colsA, const double* matrixB, int rowsB, int colsB, double* result /*, potentially other params like blockSize*/) {
    check_null(matrixA, "matrixA");
    check_null(matrixB, "matrixB");
    check_null(result, "result");
     if (colsA != rowsB) {
        throw std::invalid_argument("Incompatible dimensions for matrix multiplication.");
    }
    std::cerr << "multiply_mm_optimized: Not implemented yet.\n"; // Placeholder message
}