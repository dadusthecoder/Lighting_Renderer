#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <mutex>
#include <windows.h>

//  ANSI colors for console output
static void EnableANSIColors() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}

enum class LogLevel {
     DEBUG,
    _INFO,
    _WARNING,
    _ERROR,
    _IMP // New "Important" log level
};

class Logger {
public:
    static void Init() {
        EnableANSIColors();
    }

    static Logger& GetInstance() {
        static Logger instance;
        return instance;
    }

    void SetLogFile(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (logFile_.is_open()) {
            logFile_.close();
        }
        logFile_.open(filepath, std::ios::out | std::ios::app);
        if (!logFile_) {
            std::cerr << "[ERROR] Failed to open log file: " << filepath << std::endl;
        }
    }

    void Log(LogLevel level, const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string logMessage = FormatLogMessage(level, message);

        // Add color to the message based on the log level
        std::string coloredMessage = GetColorCode(level) + logMessage + "\033[0m";

        // Print to console
        std::cout << coloredMessage << std::endl;

        // Write to file if enabled (without color codes)
        if (logFile_.is_open()) {
            logFile_ << logMessage << std::endl;
        }
    }

private:
    std::ofstream logFile_;
    std::mutex mutex_;

    Logger() = default;
    ~Logger() {
        if (logFile_.is_open()) {
            logFile_.close();
        }
    }

    std::string FormatLogMessage(LogLevel level, const std::string& message) {
        std::ostringstream oss;
        oss << "[" << GetCurrentTime() << "] [" << LogLevelToString(level) << "]   " << message;
        return oss.str();
    }

    std::string GetCurrentTime() {
        std::time_t now = std::time(nullptr);
        struct tm timeInfo;
        char buf[20];

        // localtime_s for thread-safe local time conversion
        if (localtime_s(&timeInfo, &now) == 0) {
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeInfo);
        }
        else {
            strncpy_s(buf, sizeof(buf), "UNKNOWN TIME", sizeof(buf) - 1);
            buf[sizeof(buf) - 1] = '\0'; // Ensure null termination
        }

        return std::string(buf);
    }

    std::string LogLevelToString(LogLevel level) {
        switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::_INFO: return "_INFO";
        case LogLevel::_WARNING: return "_WARN";
        case LogLevel::_ERROR: return "ERROR";
        case LogLevel::_IMP: return "__IMP";
        default: return "UNKNOWN";
        }
    }

    std::string GetColorCode(LogLevel level) {
        switch (level) {
        case LogLevel::DEBUG: return "\033[36m";   // Cyan
        case LogLevel::_INFO: return "\033[32m";    // Green
        case LogLevel::_WARNING: return "\033[33m"; // Yellow
        case LogLevel::_ERROR: return "\033[31m";   // Red
        case LogLevel::_IMP: return "\033[35m";     // Magenta
        default: return "\033[0m";                 // Default (no color)
        }
    }

    // Disable copy and assignment
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};


#ifdef LGT_DEBUG

#define LOG_DEBUG(message)   Logger::GetInstance().Log(LogLevel::DEBUG,   message)
#define LOG_INFO(message)    Logger::GetInstance().Log(LogLevel::_INFO,   message)
#define LOG_WARN(message)    Logger::GetInstance().Log(LogLevel::_WARNING,message)
#define LOG_ERROR(message)   Logger::GetInstance().Log(LogLevel::_ERROR,  message)
#define LOG_IMP(message)     Logger::GetInstance().Log(LogLevel::_IMP,    message)

#define LOG(level, message)  Logger::GetInstance().Log(level, message)

#else

#define LOG_DEBUG(message)   ((void)0)
#define LOG_INFO(message)    ((void)0)
#define LOG_WARN(message)    ((void)0)
#define LOG_ERROR(message)   ((void)0)
#define LOG_IMP(message)     ((void)0)
#define LOG(level, message)  ((void)0)

#endif

#endif


