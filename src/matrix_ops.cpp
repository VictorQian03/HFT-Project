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


void check_null_slow(const void* ptr, const char* name) {
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
                sum += matrixA[i*colsA + k] * matrixB[k*colsB + j]; // access for matrixB is very inefficient
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


void multiply_mm_transposed_b_noinline(const double* matrixA, int rowsA, int colsA, const double* matrixB_transposed, int rowsB_T, int colsB_T, double* result) {
    check_null_slow(matrixA, "matrixA");
    check_null_slow(matrixB_transposed, "matrixB_transposed");
    check_null_slow(result, "result");

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
void multiply_mm_optimized(const double* matrixA, int rowsA, int colsA, const double* matrixB, int rowsB, int colsB, double* result)
{
    check_null(matrixA, "matrixA");
    check_null(matrixB, "matrixB");
    check_null(result, "result");
    if (colsA != rowsB) {
        throw std::invalid_argument("Incompatible dimensions for matrix multiplication.");
    }

    std::fill_n(result, rowsA * colsB, 0.0);

    const int blockSize = 64;
    // Iterate over blocks
    for (int i0 = 0; i0 < rowsA; i0 += blockSize) {
        for (int j0 = 0; j0 < colsB; j0 += blockSize) {
            for (int k0 = 0; k0 < colsA; k0 += blockSize) {
                int i_max = std::min(i0 + blockSize, rowsA);
                int j_max = std::min(j0 + blockSize, colsB);
                int k_max = std::min(k0 + blockSize, colsA);

                // Perform multiplication for the current blocks using i, k, j loop order
                for (int i = i0; i < i_max; ++i) {
                    for (int k = k0; k < k_max; ++k) {
                        const double a_ik = matrixA[i * colsA + k];
                        for (int j = j0; j < j_max; ++j) {
                            result[i * colsB + j] += a_ik * matrixB[k * colsB + j];
                        }
                    }
                }
            }
        }
    }
}


// Implemented by the team collaboratively
void multiply_mm_optimized_noinline(const double* matrixA, int rowsA, int colsA, const double* matrixB, int rowsB, int colsB, double* result)
{
    check_null_slow(matrixA, "matrixA");
    check_null_slow(matrixB, "matrixB");
    check_null_slow(result, "result");
    if (colsA != rowsB) {
        throw std::invalid_argument("Incompatible dimensions for matrix multiplication.");
    }

    std::fill_n(result, rowsA * colsB, 0.0);

    const int blockSize = 64;
    // Iterate over blocks
    for (int i0 = 0; i0 < rowsA; i0 += blockSize) {
        for (int j0 = 0; j0 < colsB; j0 += blockSize) {
            for (int k0 = 0; k0 < colsA; k0 += blockSize) {
                int i_max = std::min(i0 + blockSize, rowsA);
                int j_max = std::min(j0 + blockSize, colsB);
                int k_max = std::min(k0 + blockSize, colsA);

                // Perform multiplication for the current blocks using i, k, j loop order
                for (int i = i0; i < i_max; ++i) {
                    for (int k = k0; k < k_max; ++k) {
                        const double a_ik = matrixA[i * colsA + k];
                        for (int j = j0; j < j_max; ++j) {
                            result[i * colsB + j] += a_ik * matrixB[k * colsB + j];
                        }
                    }
                }
            }
        }
    }
}
