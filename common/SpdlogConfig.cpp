#include "SpdlogConfig.hpp"

namespace SpdlogConfig
{
    void init(LogLevel level)
    {
        spdlog::default_logger()->set_level(static_cast<spdlog::level::level_enum>(level));

        // Set the pattern - include %t for thread ID
        spdlog::set_pattern("[%t] [%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    }
}
