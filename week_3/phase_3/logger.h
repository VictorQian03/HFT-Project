#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

class Logger {
public:
    static Logger* get_instance();

    // Initialize the logger with a filename and an option to echo to console.
    void init(const std::string& filename, bool echo = true);

    // Log a message with a timestamp.
    void log(const std::string& message);

    // Close the log file.
    void close();

    ~Logger();

private:
    Logger();  // Private constructor for singleton pattern.
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger* instance;
    std::ofstream log_file;
    bool echo_to_console;
};

#endif // LOGGER_H