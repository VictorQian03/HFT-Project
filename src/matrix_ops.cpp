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
    std::fill_n(result, rows, 0.0);
    for(int i = 0; i < rows; ++i) {
        double sum = 0.0;
        for(int k = 0; k < cols; ++k) {
            sum += matrix[i * cols + k] * vector[k];
        }
        result[i] = sum;
    }
}

// Implemented by Team Member 2
void multiply_mv_col_major(const double* matrix, int rows, int cols, const double* vector, double* result) {
    check_null(matrix, "matrix");
    check_null(vector, "vector");
    check_null(result, "result");
    
    std::fill_n(result, rows, 0.0);
    for(int i = 0; i < cols; ++i) {
        for(int j = 0; j < rows; ++j) {
            result[j] += matrix[i*rows+j] * vector[i];
        }

    }
}

// Implemented by Team Member 3
void multiply_mm_naive(const double* matrixA, int rowsA, int colsA, const double* matrixB, int rowsB, int colsB, double* result) {
    check_null(matrixA, "matrixA");
    check_null(matrixB, "matrixB");
    check_null(result, "result");
    if (colsA != rowsB) {
        throw std::invalid_argument("Incompatible dimensions for matrix multiplication.");
    }
    
    std::fill_n(result, rowsA * colsB, 0.0);

    for (int i=0; i<rowsA; ++i){
        for (int j=0; j<colsB; ++j){
            double sum = 0.;
            for (int k=0; k<colsA; ++k){
                sum += matrixA[i*colsA + k] * matrixB[k*colsA + j];
            }
            result[i*colsB + j] = sum;
        }
    }
}

// Implemented by Team Member 4
void multiply_mm_transposed_b(const double* matrixA, int rowsA, int colsA, const double* matrixB_transposed, int rowsB_T, int colsB_T, double* result) {
    check_null(matrixA, "matrixA");
    check_null(matrixB_transposed, "matrixB_transposed");
    check_null(result, "result");

    // assumes that matrixB_transposed is already the transpose of B when passed into this function,
    // but because we want to compute AB, not AB_T, our dimension checking is different
    if (colsA != colsB_T) {
        throw std::invalid_argument("Incompatible dimensions for matrix multiplication (A * B^T).");
    }

    std::fill_n(result, rowsA * rowsB_T, 0.0);

    for (int i = 0; i < rowsA; ++i) {
        for (int j = 0; j < rowsB_T; ++j) {
            double sum = 0.0;
            for (int k = 0; k < colsB_T; ++k) {  // colsA == colsB_T == rowsB
                sum += matrixA[i * colsA + k] * matrixB_transposed[j * colsB_T + k];
            }
            result[i * rowsB_T + j] = sum;
        }
    }

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
    std::fill_n(result, rowsA * colsB, 0.0);
}