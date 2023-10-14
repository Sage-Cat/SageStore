#include "SageStoreServer.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

void initSpdlog()
{
    auto console = spdlog::stdout_color_mt("console");
    console->set_level(spdlog::level::info);
    console->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    console->flush_on(spdlog::level::info);
}

int main()
{
    initSpdlog();
    SPDLOG_INFO("SageStoreServer started");

    SageStoreServer server;
    server.run();

    SPDLOG_INFO("SageStoreServer finished with code=0");
    return 0;
}