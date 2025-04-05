#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <memory>

#include "matrix_ops.h"
#include "benchmark.h"
using std::cout;
using std::cerr;
using std::abs;
using std::endl;
using std::srand;
using std::time;

void initialize_data(double* data, size_t size) {
    if (!data) return;
    for (size_t i = 0; i < size; ++i) {
        data[i] = static_cast<double>(rand()) / RAND_MAX * 2.0 - 1.0; // Random values between -1.0 and 1.0
    }
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
    cout << "test_mm_naive: Not implemented yet.\n" << endl;
    return true;
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


int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(nullptr)));

    cout << "=== Running Tests ===\n" << endl;
    bool all_tests_passed = true;
    all_tests_passed &= test_mv_row_major();
    all_tests_passed &= test_mv_col_major();
    all_tests_passed &= test_mm_naive();
    all_tests_passed &= test_mm_transposed_b();

    if (all_tests_passed) {
        cout << "\n=== All Correctness Tests Passed (Placeholders) ===\n" << endl;
    } else {
        cout << "\n=== Some Correctness Tests Failed (Placeholders) ===\n" << endl;
    }

    cout << "\n=== Program Finished ===\n" << endl;

    return 0;
}