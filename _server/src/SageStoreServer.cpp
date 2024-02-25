#include "SageStoreServer.hpp"

#include "Database/RepositoryManager.hpp"
#include "BusinessLogic\BusinessLogic.hpp"
#include "Network/HttpServer.hpp"

#include "SpdlogConfig.hpp"

SageStoreServer::SageStoreServer(RepositoryManager &repositoryManager,
                                 BusinessLogic &businessLogicFacade,
                                 HttpServer &httpServer)
    : m_repositoryManager(repositoryManager),
      m_businessLogicFacade(businessLogicFacade),
      m_httpServer(httpServer)
{
    SPDLOG_TRACE("SageStoreServer::SageStoreServer");
}

void SageStoreServer::run()
{
    SPDLOG_TRACE("SageStoreServer::run");
    try
    {
        m_httpServer.run();
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR("Server failed to start: {}", e.what());
    }
}