#include "SageStoreClient.hpp"

#include <iostream>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

void initSpdlog()
{
    // Create color multi threaded logger
    auto console = spdlog::stdout_color_mt("console");

    // Set the log level
    console->set_level(spdlog::level::info);

    // Set the pattern
    console->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

    // Flush the log entries automatically
    console->flush_on(spdlog::level::info);
}

int main()
{
    std::cout << "Standard output is working" << std::endl;

    initSpdlog();
    SPDLOG_INFO("SageStoreClient started"); // Make sure you're using the macro SPDLOG_INFO, not the function

    SageStoreClient client;
    client.run();
}
