#include "SageStoreServer.hpp"

#include "Database/SqliteDatabaseManager.hpp"
#include "Database/RepositoryManager.hpp"
#include "Network/HttpServer.hpp"
#include "BusinessLogic\BusinessLogic.hpp"

#include "SpdlogConfig.hpp"

int main()
{
    SpdlogConfig::init<SpdlogConfig::LogLevel::Trace>();
    SPDLOG_INFO("SageStoreServer started");

    const std::string SERVER_ADDRESS{"127.0.0.1"};
    const unsigned short SERVER_PORT{8001};

    RepositoryManager repositoryManager(std::make_shared<SqliteDatabaseManager>(DB_PATH, CREATE_DB_SQL_FILE_PATH));
    BusinessLogic businessLogicFacade(repositoryManager);
    HttpServer httpServer(
        SERVER_ADDRESS,
        SERVER_PORT,
        std::bind(
            &BusinessLogic::executeTask,
            &businessLogicFacade,
            std::placeholders::_1,
            std::placeholders::_2));

    SPDLOG_INFO("Starting SageStoreServer on address: {} port: {}", SERVER_ADDRESS, SERVER_PORT);
    SageStoreServer server(repositoryManager, businessLogicFacade, httpServer);
    server.run();

    SPDLOG_INFO("SageStoreServer finished with code=0");
    return 0;
}
