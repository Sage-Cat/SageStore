#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <spdlog/sinks/stdout_color_sinks.h>
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

template <LogLevel level> inline void init()
{
    spdlog::default_logger()->set_level(static_cast<spdlog::level::level_enum>(level));
    spdlog::set_pattern("[%t] [%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
}
} // namespace SpdlogConfig
