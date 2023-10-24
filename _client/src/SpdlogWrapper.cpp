#include "SpdlogWrapper.hpp"

namespace SpdlogWrapper
{

    void init(LogLevel level)
    {
        auto console = spdlog::stdout_color_mt("console");
        console->set_level(static_cast<spdlog::level::level_enum>(level));
        console->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
        console->flush_on(static_cast<spdlog::level::level_enum>(level));
    }

}