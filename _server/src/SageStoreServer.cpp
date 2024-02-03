#include "SageStoreServer.hpp"

#include "Network/HttpServer.hpp"
#include "BusinessLogic/BusinessLogicFacade.hpp"

#include "SpdlogConfig.hpp"

// TODO: consider moving or refactoring
const std::string SERVER_ADDRESS{"127.0.0.1"};
const unsigned short SERVER_PORT{8001};

SageStoreServer::SageStoreServer()
    : m_businessLogicFacade(std::make_unique<BusinessLogicFacade>()),
      m_httpServer(std::make_unique<HttpServer>(SERVER_ADDRESS, SERVER_PORT, *m_businessLogicFacade))
{
}

SageStoreServer::~SageStoreServer()
{
}

void SageStoreServer::run()
{
    SPDLOG_INFO("Starting SageStoreServer on address: {} port: {}", SERVER_ADDRESS, SERVER_PORT);
    try
    {
        m_httpServer->run();
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR("Server failed to start: {}", e.what());
    }
}