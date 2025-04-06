#ifndef MATRIX_OPS_H
#define MATRIX_OPS_H

#include <stdexcept>

void multiply_mv_row_major(const double* matrix, int rows, int cols, const double* vector, double* result);

void multiply_mv_col_major(const double* matrix, int rows, int cols, const double* vector, double* result);

void multiply_mm_naive(const double* matrixA, int rowsA, int colsA, const double* matrixB, int rowsB, int colsB, double* result);

void multiply_mm_transposed_b(const double* matrixA, int rowsA, int colsA, const double* matrixB_transposed, int rowsB_T, int colsB_T, double* result);

void multiply_mm_transposed_b_noinline(const double* matrixA, int rowsA, int colsA, const double* matrixB_transposed, int rowsB_T, int colsB_T, double* result);

void multiply_mm_optimized(const double* matrixA, int rowsA, int colsA, const double* matrixB, int rowsB, int colsB, double* result /*, potentially other params like blockSize*/);


void multiply_mm_optimized_noinline(const double* matrixA, int rowsA, int colsA, const double* matrixB, int rowsB, int colsB, double* result /*, potentially other params like blockSize*/);

void multiply_mm_optimized(const double* matrixA, int rowsA, int colsA, const double* matrixB, int rowsB, int colsB, double* result /*, potentially other params like blockSize*/);


#endif
