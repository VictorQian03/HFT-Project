#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>
#include <functional> 
#include <vector>
#include <string>

struct BenchmarkResult {
    std::string name;
    int rowsA, colsA, colsB;
    double avg_time_ms;
    double std_dev_ms;
    int runs;
};

std::pair<double, double> time_function_ms(std::function<void()> func, int runs);

#endif 