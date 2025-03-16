#pragma once

#include <ctime>
#include <fstream>
#include <string_view>


class Log {
    inline static std::ofstream _filestream {"Ked.log", std::ios::app};

    enum class LogLevel {
        ERROR,
        WARN,
        INFO,
        DEBUG
    };

    static std::string_view levelToStr(LogLevel level) {
        using enum LogLevel;
        switch (level) {
            case ERROR:
                return "ERROR";
            case WARN:
                return "WARNING";
            case INFO:
                return "INFO";
            case DEBUG:
                return "DEBUG";
        }
    }


    static std::string formatTimestamp() {
        std::time_t result = std::time(nullptr);
        char timestamp[20];
        std::strftime(timestamp, 20, "%F %T", std::localtime(&result));
        return timestamp;
    }

    static void log(LogLevel level, std::string_view handle, std::string_view message) {
        if (_filestream.is_open()) {
            _filestream << formatTimestamp() << " "  << handle << " " << "[" << levelToStr(level) << "]: " << message << std::endl;
        }
    }

public:
    Log() = delete;
    Log(const Log&) = delete;
    Log operator=(const Log&) = delete;

    static constexpr void info(std::string_view handle, std::string_view msg) {
        log(LogLevel::INFO, handle, msg);
    }

    static constexpr void err(std::string_view handle, std::string_view msg) {
        log(LogLevel::ERROR, handle, msg);
    }

    static constexpr void warn(std::string_view handle, std::string_view msg) {
        log(LogLevel::WARN, handle, msg);
    }

    static constexpr void deb(std::string_view handle, std::string_view msg) {
        log(LogLevel::DEBUG, handle, msg);
    }
};
