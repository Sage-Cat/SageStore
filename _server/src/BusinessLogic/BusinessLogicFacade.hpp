#pragma once

#include <functional>
#include <memory>

#include "DataSpecs.hpp"

class RepositoryManager;
class UsersModule;

class BusinessLogicFacade
{
public:
    BusinessLogicFacade(RepositoryManager &repositoryManager);
    ~BusinessLogicFacade();

    void executeTask(RequestData requestData, BusinessLogicCallback callback);

private:
    // Modules
    std::unique_ptr<UsersModule> m_usersModule;
};
