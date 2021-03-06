/*
 * vgscribe - sykhro (c) 2019
 * licensed under the zlib license
 * ---
 * Manages logging.
 */

#pragma once

#include <vector>
#include <memory>
#include <fmt/format.h>

#include "common.h"

#define L_ERROR(...) LogManager::instance().Log(LogLevel::Error, __FILE__, __LINE__, __VA_ARGS__)
#define L_WARN(...) LogManager::instance().Log(LogLevel::Warning, __FILE__, __LINE__, __VA_ARGS__)
#define L_INFO(...) LogManager::instance().Log(LogLevel::Info, __FILE__, __LINE__, __VA_ARGS__)
#define L_DEBUG(...) LogManager::instance().Log(LogLevel::Debug, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief Levels of severity
 *
 * Used in log messages to indicate
 * the severity of the information presented.
 */
enum class LogLevel : int { Error = 0, Warning, Info, Debug };

/**
 * @brief Log entry
 *
 * Contains everything that should be pushed
 * by a sink.
 */
struct Entry {
    LogLevel level;
    const char *file;
    u32 line;
    std::string message;
};

/**
 * @brief Log sink
 *
 * Virtual interface to be implemented by the GUI.
 */
class Sink {
   public:
    virtual ~Sink(){};
    virtual bool Push(const Entry &e) = 0;

    [[nodiscard]] bool enabled() const noexcept { return m_enabled; };
    void setEnabled(bool status) { m_enabled = status; };

    [[nodiscard]] LogLevel logLevel() const noexcept { return m_level; }
    void setLogLevel(LogLevel newlevel) { m_level = newlevel; }

   protected:
#ifndef NDEBUG
    LogLevel m_level = LogLevel::Debug;
#else
    LogLevel m_level = LogLevel::Warning;
#endif
    bool m_enabled = true;
};

/**
 * @brief A class to manage the active loggers.
 */
class LogManager {
   public:
    static LogManager &instance() {
        static LogManager mainlogger;
        return mainlogger;
    };

    static void Stop();

    template <typename... Args>
    void Log(LogLevel level, const char *file, u32 line, const char *fmt, const Args &... args) {
        if (m_sinks.empty()) {
            return;
        }

        Entry e{level, file, line, fmt::format(fmt, args...)};

        for (auto &sink : m_sinks) {
            sink->Push(e);
        }
    }

    /**
     * @brief Registers a new log sink
     *
     * @param s The sink to be added
     */
    void addSink(Sink *s) { m_sinks.emplace_back(s); }

    /**
     * @brief Registers a new log sink
     *
     * @param s The sink to be added
     */
    void removeSink(Sink *s) {
        auto it = std::find(m_sinks.begin(), m_sinks.end(), s);
        if (it != m_sinks.end()) {
            m_sinks.erase(it);
        }
    }

   private:
    LogManager() = default;
    ~LogManager() = default;

    LogManager(const LogManager &) = delete;
    LogManager &operator=(const LogManager &) = delete;
    LogManager(LogManager &&) = delete;
    LogManager &operator=(LogManager &&) = delete;

    std::vector<Sink *> m_sinks{};
};
