#pragma once
#include <vector>
#include <arm_neon.h>
#include <cstdint>

class TimeSeriesProcessor {
public:
    // Constructor with window size
    explicit TimeSeriesProcessor(size_t window_size);

    // Compute moving average using standard implementation
    std::vector<double> compute_moving_average_standard(const std::vector<double>& prices) const;

    // Compute moving average using SIMD (NEON) implementation
    std::vector<double> compute_moving_average_simd(const std::vector<double>& prices) const;

    // Get window size
    size_t get_window_size() const { return window_size_; }

private:
    size_t window_size_;
}; 