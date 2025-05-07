#include "../include/time_series_processor.hpp"
#include <numeric>
#include <stdexcept>

TimeSeriesProcessor::TimeSeriesProcessor(size_t window_size)
    : window_size_(window_size) {
    if (window_size == 0) {
        throw std::invalid_argument("Window size must be greater than 0");
    }
}

std::vector<double> TimeSeriesProcessor::compute_moving_average_standard(
    const std::vector<double>& prices) const {
    if (prices.size() < window_size_) {
        return {};
    }

    std::vector<double> result(prices.size() - window_size_ + 1);
    
    for (size_t i = 0; i <= prices.size() - window_size_; ++i) {
        double sum = 0.0;
        for (size_t j = 0; j < window_size_; ++j) {
            sum += prices[i + j];
        }
        result[i] = sum / window_size_;
    }
    
    return result;
}

std::vector<double> TimeSeriesProcessor::compute_moving_average_simd(
    const std::vector<double>& prices) const {
    if (prices.size() < window_size_) {
        return {};
    }

    const size_t result_size = prices.size() - window_size_ + 1;
    std::vector<double> result(result_size);
    
    // Process each window
    for (size_t i = 0; i < result_size; ++i) {
        float64x2_t sum = vdupq_n_f64(0.0);
        size_t j = 0;
        
        // Process pairs of elements using NEON
        for (; j + 2 <= window_size_; j += 2) {
            float64x2_t window = vld1q_f64(&prices[i + j]);
            sum = vaddq_f64(sum, window);
        }
        
        // Handle remaining elements
        double remaining_sum = 0.0;
        for (; j < window_size_; ++j) {
            remaining_sum += prices[i + j];
        }
        
        // Combine SIMD sum with remaining sum
        double sum_array[2];
        vst1q_f64(sum_array, sum);
        result[i] = (sum_array[0] + sum_array[1] + remaining_sum) / window_size_;
    }
    
    return result;
} 