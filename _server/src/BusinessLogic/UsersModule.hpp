#pragma once

#include <string>
#include <memory>

#include "BusinessLogic/IBusinessModule.hpp"
#include "DataSpecs.hpp"

#include "Database/IRepository.hpp"

class RepositoryManager;
class User;
class Role;

class UsersModule : public IBusinessModule
{
public:
    UsersModule(RepositoryManager &repositoryManager);
    virtual ~UsersModule() override;

    virtual ResponseData executeTask(const RequestData &requestData) override;

private:
    ResponseData loginUser(const Dataset &request);
    ResponseData registerUser(const Dataset &request);

private:
    std::shared_ptr<IRepository<User>> m_usersRepository;
    std::shared_ptr<IRepository<Role>> m_rolesRepository;
};
