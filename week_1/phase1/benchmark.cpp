#include "benchmark.h"
#include <vector>
#include <numeric> 
#include <cmath>  
#include <iostream>
#include <iomanip> 
using std::pair;
using std::vector;
using std::function;
using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::milli;
using std::accumulate;
using std::inner_product;
using std::sqrt;

pair<double, double> time_function_ms(function<void()> func, int runs) {
    if (runs <= 0) {
        return {0.0, 0.0};
    }

    vector<double> timings_ms;
    timings_ms.reserve(runs);

    for (int i = 0; i < runs; ++i) {
        auto start = high_resolution_clock::now();
        func();
        auto end = high_resolution_clock::now();
        duration<double, milli> duration_ms = end - start;
        timings_ms.push_back(duration_ms.count());
    }

    if (timings_ms.empty()) {
        return {0.0, 0.0};
    }

    double sum = accumulate(timings_ms.begin(), timings_ms.end(), 0.0);
    double avg = sum / timings_ms.size();

    double sq_sum = inner_product(timings_ms.begin(), timings_ms.end(), timings_ms.begin(), 0.0);
    double std_dev = 0.0;
    if (timings_ms.size() > 1) {
         std_dev = sqrt((sq_sum / timings_ms.size()) - (avg * avg));
    }

    return {avg, std_dev};
}