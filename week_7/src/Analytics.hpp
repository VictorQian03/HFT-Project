#pragma once

#include "OrderProcessing.hpp"
#include <vector>
#include <chrono>
#include <functional>
#include <string>

void runTradeAnalytics();

double calculateAverageQueueTime(const std::vector<LatencyRecord>& data);
double calculateAverageProcessingTime(const std::vector<LatencyRecord>& data);

void printLatencyPercentiles(
    const std::vector<LatencyRecord>& data,
    const std::function<std::chrono::nanoseconds(const LatencyRecord&)>& extractor,
    const std::string& name);