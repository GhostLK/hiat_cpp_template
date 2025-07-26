#ifndef LOGGER_SERVICE_H
#define LOGGER_SERVICE_H

#include <memory>
#include <mutex>
#include <string>

#include "config_service.h"
#include "spdlog/common.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace sx
{

template <typename Derived>
class LoggerServiceBase
{
public:
    inline void trace(const std::string& message) { static_cast<Derived*>(this)->trace(message); }

    inline void debug(const std::string& message) { static_cast<Derived*>(this)->debug(message); }

    inline void info(const std::string& message) { static_cast<Derived*>(this)->info(message); }

    inline void warn(const std::string& message) { static_cast<Derived*>(this)->warn(message); }

    inline void error(const std::string& message) { static_cast<Derived*>(this)->error(message); }

    inline void critical(const std::string& message) {
        static_cast<Derived*>(this)->critical(message);
    }
};

namespace level
{
enum level_enum : int {
    trace = SPDLOG_LEVEL_TRACE,
    debug = SPDLOG_LEVEL_DEBUG,
    info = SPDLOG_LEVEL_INFO,
    warn = SPDLOG_LEVEL_WARN,
    err = SPDLOG_LEVEL_ERROR,
    critical = SPDLOG_LEVEL_CRITICAL,
    off = SPDLOG_LEVEL_OFF,
    n_levels
};

inline level::level_enum from_string(const std::string& level_str) {
    if (level_str == "trace") return level::trace;
    if (level_str == "debug") return level::debug;
    if (level_str == "info") return level::info;
    if (level_str == "warn") return level::warn;
    if (level_str == "err") return level::err;
    if (level_str == "critical") return level::critical;
    if (level_str == "off") return level::off;
    return level::info;
}

}  // namespace level

class SpdLoggerService : public LoggerServiceBase<SpdLoggerService>
{
public:
    /**
     * @brief Get the singleton instance of LoggerService
     * @return The instance of LoggerService
     */
    static SpdLoggerService& get_instance() {
        static SpdLoggerService instance;
        return instance;
    }

    /**
     * @brief Initialize with default parameters (fallback)
     * @param level The log level
     * @param log_file_name The log file name, if empty, the logger will only output to the console
     * @return true if initialization successful, false otherwise
     */
    bool init(level::level_enum level = level::info, const std::string& log_file_name = "") {
        spdlog::drop("console"); 
        logger_ = spdlog::stdout_color_mt("console");
        logger_->set_level(static_cast<spdlog::level::level_enum>(level));
        if (!log_file_name.empty()) {
            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file_name);
            logger_->sinks().push_back(file_sink);
        }
        spdlog::set_default_logger(logger_);
        spdlog::flush_on(spdlog::level::err);
        return true;
    }

    template <typename... Args>
    inline void trace(Args&&... args) {
        if (logger_) logger_->trace(std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void debug(Args&&... args) {
        if (logger_) logger_->debug(std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void info(Args&&... args) {
        if (logger_) logger_->info(std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void warn(Args&&... args) {
        if (logger_) logger_->warn(std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void error(Args&&... args) {
        if (logger_) logger_->error(std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void critical(Args&&... args) {
        if (logger_) logger_->critical(std::forward<Args>(args)...);
    }

    SpdLoggerService(const SpdLoggerService&) = delete;
    SpdLoggerService& operator=(const SpdLoggerService&) = delete;
    SpdLoggerService(SpdLoggerService&&) = delete;
    SpdLoggerService& operator=(SpdLoggerService&&) = delete;

private:
    SpdLoggerService() = default;
    ~SpdLoggerService() { spdlog::drop_all(); }

    std::shared_ptr<spdlog::logger> logger_;
};

class NullLoggerService : public LoggerServiceBase<NullLoggerService>
{
public:
    static NullLoggerService& get_instance() {
        static NullLoggerService instance;
        return instance;
    }

    template <typename... Args>
    inline void trace(Args&&... args) { }

    template<typename... Args>
    inline void debug(Args&&... args) { }

    template <typename... Args>
    inline void info(Args&&... args) { }

    template <typename... Args>
    inline void warn(Args&&... args) { }

    template <typename... Args>
    inline void error(Args&&... args) { }

    template <typename... Args>
    inline void critical(Args&&... args) { }

    NullLoggerService(const NullLoggerService&) = delete;
    NullLoggerService& operator=(const NullLoggerService&) = delete;
    NullLoggerService(NullLoggerService&&) = delete;
    NullLoggerService& operator=(NullLoggerService&&) = delete;

private:
    NullLoggerService() = default;
    ~NullLoggerService() = default;
};

using LoggerService = SpdLoggerService;

}  // namespace sx

#define SX_LOG_INIT(level, log_file_name) ::sx::LoggerService::get_instance().init(level, log_file_name)
#define SX_LOG_TRACE(...) ::sx::LoggerService::get_instance().trace(__VA_ARGS__)
#define SX_LOG_DEBUG(...) ::sx::LoggerService::get_instance().debug(__VA_ARGS__)
#define SX_LOG_INFO(...) ::sx::LoggerService::get_instance().info(__VA_ARGS__)
#define SX_LOG_WARN(...) ::sx::LoggerService::get_instance().warn(__VA_ARGS__)
#define SX_LOG_ERROR(...) ::sx::LoggerService::get_instance().error(__VA_ARGS__)
#define SX_LOG_CRITICAL(...) ::sx::LoggerService::get_instance().critical(__VA_ARGS__)

#endif  // LOGGER_SERVICE_H
