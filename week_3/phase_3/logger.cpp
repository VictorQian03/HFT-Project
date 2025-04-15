#include "logger.h"
#include <ctime>
#include <iomanip>
#include <iostream>

Logger* Logger::instance = nullptr;

Logger::Logger() : echo_to_console(true) {}

Logger* Logger::get_instance() {
    if (!instance) {
        instance = new Logger();
    }
    return instance;
}

void Logger::init(const std::string& filename, bool echo) {
    echo_to_console = echo;
    // Open file in output mode and truncate it (erase previous content)
    log_file.open(filename, std::ios::out | std::ios::trunc);
    if (!log_file.is_open()) {
        std::cerr << "Failed to open log file: " << filename << "\n";
    }
    log("Logger initialized");
}
void Logger::log(const std::string& message) {
    // Write to the log file if open.
    if (log_file.is_open()) {
        log_file << message << "\n";
        log_file.flush();
    }
    // Optionally echo to the console.
    if (echo_to_console) {
        std::cout << message << "\n";
    }
}

void Logger::close() {
    if (log_file.is_open()) {
        log_file.close();
    }
}

Logger::~Logger() {
    close();
}