#ifndef __UTIL_LOGGER_H__
#define __UTIL_LOGGER_H__

#include <memory>
#include <string_view>

#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/spdlog.h"

namespace Util::Log {
class Logger
{
public:
    Logger(const std::string &loggerName)
    {
        if (!dailySink) { // Can't construct multi logger in multi-thread
            dailySink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(LogFileName, 0, 0);
        }
        logger_ = std::make_shared<spdlog::logger>(loggerName, dailySink);
    }

    template <typename... Args>
    void debug(const std::string_view text, Args &&... args)
    {
        logger_->debug(text, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void info(const std::string_view text, Args &&... args)
    {
        logger_->info(text, std::forward<Args>(args)...);
        logger_->flush();
    }

    template <typename... Args>
    void warn(const std::string_view text, Args &&... args)
    {
        logger_->warn(text, std::forward<Args>(args)...);
        logger_->flush();
    }

    template <typename... Args>
    void error(const std::string_view text, Args &&... args)
    {
        logger_->error(text, std::forward<Args>(args)...);
        logger_->flush();
    }

    template <typename... Args>
    void critical(const std::string_view text, Args &&... args)
    {
        logger_->critical(text, std::forward<Args>(args)...);
        logger_->flush();
    }

private:
    std::shared_ptr<spdlog::logger> logger_ = nullptr;

    static inline std::shared_ptr<spdlog::sinks::daily_file_sink_mt> dailySink = nullptr;
    static inline const std::string LogFileName = "logs/quant-crypto.log";
};
} // namespace Util::Log
#endif // __UTIL_LOGGER_H__