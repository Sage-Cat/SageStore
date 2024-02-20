#pragma once

class RepositoryManager;
class BusinessLogicFacade;
class HttpServer;

class SageStoreServer
{
public:
    SageStoreServer(RepositoryManager &repositoryManager,
                    BusinessLogicFacade &businessLogicFacade,
                    HttpServer &httpServer);
    ~SageStoreServer() = default;

    void run();

private:
    RepositoryManager &m_repositoryManager;
    BusinessLogicFacade &m_businessLogicFacade;
    HttpServer &m_httpServer;
};
