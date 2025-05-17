#include "Analytics.hpp"
#include <numeric>
#include <algorithm>
#include <iostream>
#include <iomanip>

double calculateAverageQueueTime(const std::vector<LatencyRecord>& data) {
    if (data.empty()) return 0.0;

    long long total = std::reduce(
        data.begin(), data.end(), 0LL,
        [](long long sum, const LatencyRecord& r){
            return sum + r.queue_duration_ns.count();
        }
    );

    return double(total) / data.size();
}

double calculateAverageProcessingTime(const std::vector<LatencyRecord>& data) {
    if (data.empty()) return 0.0;

    long long total = std::reduce(
        data.begin(), data.end(), 0LL,
        [](long long sum, const LatencyRecord& r){
            return sum + r.processing_duration_ns.count();
        }
    );

    return double(total) / data.size();
}

void printLatencyPercentiles(
    const std::vector<LatencyRecord>& data,
    const std::function<std::chrono::nanoseconds(const LatencyRecord&)>& extractor,
    const std::string& name)
{
    if (data.empty()) {
        std::cout << "No data for " << name << " percentiles.\n";
        return;
    }

    std::vector<long long> latencies(data.size());
    std::transform(
        data.begin(), data.end(),
        latencies.begin(),
        [&](const LatencyRecord& r){
            return extractor(r).count();
        }
    );

    std::sort(latencies.begin(), latencies.end());

    auto idx = [&](double p) {
        size_t i = static_cast<size_t>(p * latencies.size());
        return (i >= latencies.size() ? latencies.size() - 1 : i);
    };

    std::cout << name << " Latency Percentiles (ns):\n"
              << "  Min:           " << latencies.front()        << "\n"
              << "  50th (Median): " << latencies[idx(0.50)]    << "\n"
              << "  90th:          " << latencies[idx(0.90)]    << "\n"
              << "  95th:          " << latencies[idx(0.95)]    << "\n"
              << "  99th:          " << latencies[idx(0.99)]    << "\n";
    if (latencies.size() > 1) {
        std::cout << "  99.9th:        " << latencies[idx(0.999)] << "\n";
    }
    std::cout << "  Max:           " << latencies.back()         << "\n\n";
}

void runTradeAnalytics() {
    std::cout << "\nTrade Analytics:\n"
              << std::fixed << std::setprecision(2);

    const auto& data = latencyDataStore;
    if (data.empty()) {
        std::cout << "No latency data collected to analyze.\n";
        return;
    }

    std::cout << "Analyzing " << data.size() << " latency records.\n";

    double avgQ = calculateAverageQueueTime(data);
    double avgP = calculateAverageProcessingTime(data);
    std::cout << "Average Order Queue Time:      "
              << (avgQ / 1'000.0) << " μs (" << avgQ << " ns)\n"
              << "Average Order Processing Time: "
              << (avgP / 1'000.0) << " μs (" << avgP << " ns)\n";

    long long totalE2E = std::transform_reduce(
        data.begin(), data.end(),
        0LL,
        std::plus<>(),
        [&](const LatencyRecord& r){
            return r.queue_duration_ns.count()
                 + r.processing_duration_ns.count();
        }
    );
    double avgE2E = double(totalE2E) / data.size();
    std::cout << "Average End-to-End Latency:    "
              << (avgE2E / 1'000.0) << " μs (" << avgE2E << " ns)\n\n";

    printLatencyPercentiles(
        data,
        [](const LatencyRecord& r){ return r.queue_duration_ns; },
        "Queue"
    );
    printLatencyPercentiles(
        data,
        [](const LatencyRecord& r){ return r.processing_duration_ns; },
        "Processing"
    );
}