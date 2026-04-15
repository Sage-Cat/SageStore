#include "SageStoreServer.hpp"

#include <cstdlib>
#include <string>

#include "BusinessLogic/BusinessLogic.hpp"
#include "Database/RepositoryManager.hpp"
#include "Database/SqliteDatabaseManager.hpp"
#include "Network/HttpServer.hpp"
#include "ServerStartupConfig.hpp"

#include "common/SpdlogConfig.hpp"

int main()
{
    SpdlogConfig::init<SpdlogConfig::LogLevel::Trace>(
        {.loggerName = "SageStoreServer", .logFileStem = "sagestore-server"});
    SPDLOG_INFO("SageStoreServer started");

    const std::string serverAddress =
        ServerStartupConfig::envOrDefault("SAGESTORE_SERVER_ADDRESS", "127.0.0.1");
    const unsigned short serverPort =
        ServerStartupConfig::envOrDefaultPort("SAGESTORE_SERVER_PORT", 8001);
    const std::string dbPath =
        ServerStartupConfig::envOrDefault("SAGESTORE_DB_PATH", DB_PATH);
    const std::string createDbPath =
        ServerStartupConfig::envOrDefault("SAGESTORE_DB_SQL_PATH", CREATE_DB_SQL_FILE_PATH);

    RepositoryManager repositoryManager(
        std::make_shared<SqliteDatabaseManager>(dbPath, createDbPath));
    BusinessLogic businessLogicFacade(repositoryManager);
    HttpServer httpServer(serverAddress, serverPort,
                          std::bind(&BusinessLogic::executeTask, &businessLogicFacade,
                                    std::placeholders::_1, std::placeholders::_2));

    SPDLOG_INFO("Starting SageStoreServer on address: {} port: {}", serverAddress, serverPort);
    SageStoreServer server(repositoryManager, businessLogicFacade, httpServer);
    server.run();

    constexpr int exitCode = 0;
    SPDLOG_INFO("SageStoreServer finished with code={}", exitCode);
    SpdlogConfig::shutdown();
    return exitCode;
}
