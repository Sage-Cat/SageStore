#pragma once

class RepositoryManager;
class BusinessLogic;
class HttpServer;

class SageStoreServer
{
public:
    SageStoreServer(RepositoryManager &repositoryManager,
                    BusinessLogic &businessLogicFacade,
                    HttpServer &httpServer);
    ~SageStoreServer() = default;

    void run();

private:
    RepositoryManager &m_repositoryManager;
    BusinessLogic &m_businessLogicFacade;
    HttpServer &m_httpServer;
};
