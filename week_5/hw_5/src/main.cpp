#include "../include/robinhood_hash_table.hpp"
#include <unordered_map>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <string>
#include <numeric>

using namespace std::chrono;

struct Result {
    double insert_ms = 0;
    double lookup_ms = 0;
};

Result benchmark_robinhood(const std::vector<std::string>& keys, const std::vector<double>& values) {
    RobinHoodHashTable<std::string, double> rh_table;
    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < keys.size(); ++i) {
        rh_table.insert(keys[i], values[i]);
    }
    auto end = high_resolution_clock::now();
    double insert_ms = duration_cast<milliseconds>(end - start).count();

    start = high_resolution_clock::now();
    for (size_t i = 0; i < keys.size(); ++i) {
        volatile auto v = rh_table.find(keys[i]);
    }
    end = high_resolution_clock::now();
    double lookup_ms = duration_cast<milliseconds>(end - start).count();

    return {insert_ms, lookup_ms};
}

Result benchmark_stdmap(const std::vector<std::string>& keys, const std::vector<double>& values) {
    std::unordered_map<std::string, double> std_map;
    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < keys.size(); ++i) {
        std_map[keys[i]] = values[i];
    }
    auto end = high_resolution_clock::now();
    double insert_ms = duration_cast<milliseconds>(end - start).count();

    start = high_resolution_clock::now();
    for (size_t i = 0; i < keys.size(); ++i) {
        volatile auto v = std_map.find(keys[i]);
    }
    end = high_resolution_clock::now();
    double lookup_ms = duration_cast<milliseconds>(end - start).count();

    return {insert_ms, lookup_ms};
}

void print_avg(const std::string& label, const std::vector<Result>& results) {
    double avg_insert = 0, avg_lookup = 0;
    for (const auto& r : results) {
        avg_insert += r.insert_ms;
        avg_lookup += r.lookup_ms;
    }
    avg_insert /= results.size();
    avg_lookup /= results.size();
    std::cout << label << " (avg over " << results.size() << " runs):\n"
              << "  Insert: " << avg_insert << " ms\n"
              << "  Lookup: " << avg_lookup << " ms\n";
}

int main() {
    std::vector<size_t> test_sizes = { 100000,1000000, 10000000};
    constexpr int num_trials = 5;

    for (size_t N : test_sizes) {
        std::cout << "\n=== Benchmark for N = " << N << " ===\n";
        std::vector<Result> rh_results, std_results;

        for (int trial = 0; trial < num_trials; ++trial) {
            std::vector<std::string> keys;
            std::vector<double> values;
            keys.reserve(N);
            values.reserve(N);

            // Generate random keys and values
            std::mt19937 gen(42 + trial); // Different seed for each trial
            std::uniform_int_distribution<> char_dist('A', 'Z');
            std::uniform_real_distribution<> val_dist(1.0, 1000.0);

            for (size_t i = 0; i < N; ++i) {
                std::string sym(5, ' ');  // Generate 3-letter symbols
                for (auto& c : sym) c = char_dist(gen);
                keys.push_back(sym);
                values.push_back(val_dist(gen));
            }

            rh_results.push_back(benchmark_robinhood(keys, values));
            std_results.push_back(benchmark_stdmap(keys, values));
        }

        print_avg("Robin Hood Hash Table", rh_results);
        print_avg("std::unordered_map", std_results);
    }
}