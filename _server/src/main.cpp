#include "SageStoreServer.hpp"

#include "SpdlogConfig.hpp"

int main()
{
    SpdlogConfig::init();
    SPDLOG_INFO("SageStoreServer started");

    SageStoreServer server;
    server.run();

    SPDLOG_INFO("SageStoreServer finished with code=0");
    return 0;
}