#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <memory>
#include <random>
#include <stdexcept>
#include <functional>
#include <string>

#include "matrix_ops.h"
#include "benchmark.h"
#include "alignment.h"
using std::cout;
using std::cerr;
using std::vector;
using std::tuple;
using std::string;
using std::abs;
using std::endl;
using std::srand;
using std::time;
using std::flush;
using std::left;
using std::setw;
using std::right;

template <typename T>
using AlignedVector = std::vector<T, AlignedAllocator<T, 64>>;


std::vector<double> generate_random_vector(int size, bool aligned = false) {
    if (size <= 0) {
        throw std::invalid_argument("Vector size must be positive.");
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(0.0, 1.0); 

    if (aligned) {
        AlignedVector<double> vec(size);
        for (int i = 0; i < size; ++i) {
            vec[i] = distrib(gen);
        }
        // Copy to std::vector to match function signature
        return std::vector<double>(vec.begin(), vec.end());
    } else {
        std::vector<double> vec(size);
        for (int i = 0; i < size; ++i) {
            vec[i] = distrib(gen);
        }
        return vec;
    }
}


vector<double> generate_random_matrix(int rows, int cols, bool aligned = false) {
    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument("Matrix dimensions must be positive.");
    }
    return generate_random_vector(rows * cols, aligned); 
}

vector<double> transpose_matrix(const vector<double>& matrix, int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument("Matrix dimensions must be positive.");
    }
    if (matrix.size() != static_cast<size_t>(rows * cols)) {
         throw std::invalid_argument("Matrix data size does not match dimensions.");
    }

    vector<double> transposed(cols * rows); 
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            transposed[j * rows + i] = matrix[i * cols + j];
        }
    }
    return transposed;
}

bool check_result(const char* test_name, const double* calculated, const double* expected, size_t size, double tolerance = 1e-9) {
    for (size_t i = 0; i < size; ++i) {
        if (abs(calculated[i] - expected[i]) > tolerance) {
            cerr << "Test Failed: " << test_name << "\n";
            cerr << "  Mismatch at index " << i << ": Calculated=" << calculated[i]
                      << ", Expected=" << expected[i] << endl;
            return false;
        }
    }
    cout << "Test Passed: " << test_name << endl;
    return true;
}

bool test_mv_row_major() {
    cout << "\n--- Testing multiply_mv_row_major ---" << endl;
    const int rows = 2;
    const int cols = 3;
    const double matrix_data[] = {1.0, 2.0, 3.0,
                                  4.0, 5.0, 6.0};
    const double vector_data[] = {1.0, 2.0, 3.0};
    const double expected_result[] = {14.0, 32.0}; // (1*1 + 2*2 + 3*3), (4*1 + 5*2 + 6*3)
    double actual_result[rows];

    bool success = true;
    try {
        multiply_mv_row_major(matrix_data, rows, cols, vector_data, actual_result);

        success = check_result("multiply_mv_row_major correctness", actual_result, expected_result, rows);

    } catch (const std::exception& e) {
        cerr << "Test Failed: multiply_mv_row_major threw unexpected exception: " << e.what() << endl;
        success = false;
    }

    return success;
}

bool test_mv_col_major() {
    cout << "\n--- Testing multiply_mv_col_major ---" << endl;
    const int rows = 3;
    const int cols = 2;
    const double matrix_data[] = {
        1.0, 2.0, 3.0, // First column
        4.0, 5.0, 6.0  // Second column
    }; // Correct column-major representation
    const double vector_data[] = {1.0, 2.0}; // Vector to multiply
    const double expected_result[] = {9.0, 12.0, 15.0}; //expected
    double actual_result[rows];

    bool success = true;
    try {
        multiply_mv_col_major(matrix_data, rows, cols, vector_data, actual_result);

        success = check_result("multiply_mv_col_major correctness", actual_result, expected_result, rows);

    } catch (const std::exception& e) {
        cerr << "Test Failed: multiply_mv_col_major threw unexpected exception: " << e.what() << endl;
        success = false;
    }

    return success;
}

bool test_mm_naive() {
    cout << "\n--- Testing multiply_mm_naive ---\n" << endl;
    const int rowsA = 3;
    const int colsA = 3;
    const int rowsB = 3;
    const int colsB = 2;
    const double matrixA[] = {
        1.0, 2.0, 1.0,
        0.0, 1.0, 0.0,
        2.0, 3.0, 4.0
    };
    const double matrixB[] = {
        2.0, 5.0,
        6.0, 7.0,
        1.0, 8.0
    };
    const double expected_res[] = {
        15.0, 27.0,
        6.0, 7.0,
        26.0, 63.0
    };
    double actual_res[rowsA * colsB];

    bool success = true;
    try {
        multiply_mm_naive(matrixA, rowsA, colsA, matrixB, rowsB, colsB, actual_res);
        success = check_result("multiply_mm_naive correctness", actual_res, expected_res, rowsA * colsB);
    }
    catch (const std::exception& e) {
        cerr << "Test Failed: multiply_mm_naive threw unexpected exception: " << e.what() << endl;
        success = false;
    }

    return success;
}

bool test_mm_transposed_b() {
    cout << "\n--- Testing multiply_mm_transposed_b ---\n" << endl;
    const int rowsA = 3;
    const int colsA = 3;
    const int rowsB_T = 2;
    const int colsB_T = 3;
    const double matrixA[] = {
        1.0, 2.0, 1.0,
        0.0, 1.0, 0.0,
        2.0, 3.0, 4.0
    };
    const double matrixB_T[] = {
        2.0, 6.0, 1.0,
        5.0, 7.0, 8.0
    };
    const double expected_res[] = {
        15.0, 27.0,
        6.0, 7.0,
        26.0, 63.0
    };
    double actual_res[rowsA * rowsB_T];

    bool success = true;
    try {
        multiply_mm_transposed_b(matrixA, rowsA, colsA, matrixB_T, rowsB_T, colsB_T, actual_res);
        success = check_result("multiply_mm_transposed_b correctness", actual_res, expected_res, rowsA * rowsB_T);
    }
    catch (const std::exception& e) {
        cerr << "Test Failed: multiply_mm_transposed_b threw unexpected exception: " << e.what() << endl;
        success = false;
    }

    return success;
}

bool test_mm_optimized() {
    cout << "\n--- Testing multiply_mm_optimized ---\n" << endl;
    const int rowsA = 3;
    const int colsA = 3;
    const int rowsB = 3;
    const int colsB = 2;
    const double matrixA[] = {
        1.0, 2.0, 1.0,
        0.0, 1.0, 0.0,
        2.0, 3.0, 4.0
    };
    const double matrixB[] = {
        2.0, 5.0,
        6.0, 7.0,
        1.0, 8.0
    };
    const double expected_res[] = {
        15.0, 27.0,
        6.0, 7.0,
        26.0, 63.0
    };
    double actual_res[rowsA * colsB];

    bool success = true;
    try {
        multiply_mm_optimized(matrixA, rowsA, colsA, matrixB, rowsB, colsB, actual_res);
        success = check_result("multiply_mm_optimized correctness", actual_res, expected_res, rowsA * colsB);
    }
    catch (const std::exception& e) {
        cerr << "Test Failed: multiply_mm_optimized threw unexpected exception: " << e.what() << endl;
        success = false;
    }

    return success;
}

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(nullptr)));

    cout << "=== Running Tests ===\n" << endl;
    bool all_tests_passed = true;
    all_tests_passed &= test_mv_row_major();
    all_tests_passed &= test_mv_col_major();
    all_tests_passed &= test_mm_naive();
    all_tests_passed &= test_mm_transposed_b();
    all_tests_passed &= test_mm_optimized();

    if (all_tests_passed) {
        cout << "\n=== All Correctness Tests Passed ===\n" << endl;
    } else {
        cout << "\n=== Some Correctness Tests Failed ===\n" << endl;
        return 1;
    }

    cout << "\n=== Testing Program Finished ===\n" << endl;
    const int num_runs = 10;
    cout << "Starting benchmarks (" << num_runs << " runs per test):" << endl;

    // Define test sizes: {rowsA, colsA (==rowsB), colsB}
    vector<tuple<int, int, int>> test_sizes = {
        {10, 10, 10},       
        {100, 100, 100},   
        {500, 500, 500},    
        {1000, 1000, 1000}, 
        {200, 50, 100},     
        {50, 300, 80}      
    };

    vector<BenchmarkResult> results;

    for (bool aligned : {false, true}) {
        cout << "\n\n=== Benchmarking " << (aligned ? "[Aligned]" : "[Unaligned]") << " Data ===\n";

        for (const auto& sizes : test_sizes) {
            int rowsA = std::get<0>(sizes);
            int colsA = std::get<1>(sizes); 
            int rowsB = colsA;
            int colsB = std::get<2>(sizes);

            cout << "\nBenchmarking Size: A(" << rowsA << "x" << colsA << "), B(" << rowsB << "x" << colsB << ")" << endl;

            try {
                // --- Generate Data ---
                vector<double> matrixA = generate_random_matrix(rowsA, colsA, aligned);
                vector<double> matrixB = generate_random_matrix(rowsB, colsB, aligned);
                vector<double> vector_in = generate_random_vector(colsA, aligned); 

                // Result buffers
                vector<double> result_mv(rowsA);
                vector<double> result_mm(rowsA * colsB);

                auto label = aligned ? "[Aligned] " : "[Unaligned] ";

                // --- Benchmark Matrix-Vector (Row Major) ---
                cout << "  Benchmarking multiply_mv_row_major..." << flush;
                auto func_mv_row = [&]() {
                    multiply_mv_row_major(matrixA.data(), rowsA, colsA, vector_in.data(), result_mv.data());
                };
                auto timing_mv_row = time_function_ms(func_mv_row, num_runs);
                results.push_back({label + std::string("multiply_mv_row_major"), rowsA, colsA, 1, timing_mv_row.first, timing_mv_row.second, num_runs});
                cout << " Done." << endl;

                // --- Benchmark Matrix-Vector (Col Major) ---
                cout << "  Benchmarking multiply_mv_col_major..." << flush;
                vector<double> matrixA_col_major = transpose_matrix(matrixA, rowsA, colsA); 
                auto func_mv_col = [&]() {
                    multiply_mv_col_major(matrixA_col_major.data(), rowsA, colsA, vector_in.data(), result_mv.data());
                };
                auto timing_mv_col = time_function_ms(func_mv_col, num_runs);
                results.push_back({label + std::string("multiply_mv_col_major"), rowsA, colsA, 1, timing_mv_col.first, timing_mv_col.second, num_runs});
                cout << " Done." << endl;

                // --- Benchmark Matrix-Matrix (Naive) ---
                cout << "  Benchmarking multiply_mm_naive..." << flush;
                auto func_mm_naive = [&]() {
                    multiply_mm_naive(matrixA.data(), rowsA, colsA, matrixB.data(), rowsB, colsB, result_mm.data());
                };
                auto timing_mm_naive = time_function_ms(func_mm_naive, num_runs);
                results.push_back({label + std::string("multiply_mm_naive"), rowsA, colsA, colsB, timing_mm_naive.first, timing_mm_naive.second, num_runs});
                cout << " Done." << endl;

                // --- Benchmark Matrix-Matrix (Transposed B) ---
                cout << "  Benchmarking multiply_mm_transposed_b..." << flush;
                vector<double> matrixB_T = transpose_matrix(matrixB, rowsB, colsB);
                int rowsB_T = colsB;
                int colsB_T = rowsB; 
                auto func_mm_transposed = [&]() {
                    multiply_mm_transposed_b(matrixA.data(), rowsA, colsA, matrixB_T.data(), rowsB_T, colsB_T, result_mm.data());
                };
                auto timing_mm_transposed = time_function_ms(func_mm_transposed, num_runs);
                results.push_back({label + std::string("multiply_mm_transposed_b"), rowsA, colsA, colsB, timing_mm_transposed.first, timing_mm_transposed.second, num_runs});
                cout << " Done." << endl;

                // --- Benchmark Matrix-Matrix (Optimized) ---
                cout << "  Benchmarking multiply_mm_optimized..." << flush;
                auto func_mm_optimized = [&]() {
                    multiply_mm_optimized(matrixA.data(), rowsA, colsA, matrixB.data(), rowsB, colsB, result_mm.data());
                };
                auto timing_mm_optimized = time_function_ms(func_mm_optimized, num_runs);
                results.push_back({label + std::string("multiply_mm_optimized"), rowsA, colsA, colsB, timing_mm_optimized.first, timing_mm_optimized.second, num_runs});
                cout << " Done." << endl;

            } catch (const std::exception& e) {
                cerr << "\nError during benchmarking for size ("
                        << rowsA << "x" << colsA << " * " << rowsB << "x" << colsB << "): "
                        << e.what() << endl;
            }
        }
    }
    cout << "\n\n--- Benchmark Results (" << num_runs << " runs per test) ---\n";
    cout << left << setw(34) << "Function"
        << right << setw(8) << "RowsA"
        << setw(8) << "ColsA"
        << setw(8) << "ColsB"
        << setw(15) << "Avg Time (ms)"
        << setw(15) << "Std Dev (ms)"
        << endl;

    cout << string(88, '-') << endl;
    cout << std::fixed << std::setprecision(4);

    for (const auto& res : results) {
        cout << left  << setw(34) << res.name
            << right << setw(8)  << res.rowsA
            << setw(8)  << res.colsA
            << setw(8)  << res.colsB
            << setw(15) << res.avg_time_ms
            << setw(15) << res.std_dev_ms
            << endl;
    }
    cout << string(88, '-') << endl;

    return 0;

}