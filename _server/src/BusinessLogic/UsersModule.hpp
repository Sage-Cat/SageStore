#pragma once

#include <memory>
#include <string>

#include "DataSpecs.hpp"
#include "IBusinessModule.hpp"

#include "Database/IRepository.hpp"

class RepositoryManager;

struct User;
struct Role;

class UsersModule : public IBusinessModule {
public:
    UsersModule(RepositoryManager &repositoryManager);
    virtual ~UsersModule() override;

    virtual ResponseData executeTask(const RequestData &requestData) override;

private:
    ResponseData loginUser(const Dataset &request);
    ResponseData registerUser(const Dataset &request);

    ResponseData getRoles();
    ResponseData addNewRole(const Dataset &request);
    ResponseData updateRoles(const Dataset &request, const std::string &resourseId);
    ResponseData deleteRole(const std::string &resourseId);

private:
    std::shared_ptr<IRepository<User>> m_usersRepository;
    std::shared_ptr<IRepository<Role>> m_rolesRepository;
};
