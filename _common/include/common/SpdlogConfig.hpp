#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <cstddef>
#include <string>

#include <spdlog/spdlog.h>

namespace SpdlogConfig {
enum class LogLevel {
    Trace    = SPDLOG_LEVEL_TRACE,
    Debug    = SPDLOG_LEVEL_DEBUG,
    Info     = SPDLOG_LEVEL_INFO,
    Warn     = SPDLOG_LEVEL_WARN,
    Error    = SPDLOG_LEVEL_ERROR,
    Critical = SPDLOG_LEVEL_CRITICAL,
    Off      = SPDLOG_LEVEL_OFF
};

struct Options {
    std::string loggerName{"SageStore"};
    std::string logFileStem{"sagestore"};
    std::string logDirectory{};
    bool enableConsole{true};
    bool enableFile{true};
    std::size_t maxFileSizeBytes{5U * 1024U * 1024U};
    std::size_t maxFileCount{5U};
};

void initialize(spdlog::level::level_enum level, const Options &options = {});
void shutdown();

template <LogLevel level> inline void init(const Options &options = {})
{
    initialize(static_cast<spdlog::level::level_enum>(level), options);
}
} // namespace SpdlogConfig
