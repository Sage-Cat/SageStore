#include "SpdlogConfig.hpp"

namespace SpdlogConfig
{
    void init(LogLevel level)
    {
        spdlog::default_logger()->set_level(static_cast<spdlog::level::level_enum>(level));
    }
}