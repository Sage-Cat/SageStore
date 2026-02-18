#pragma once

#include <memory>
#include <string>

#include "DataSpecs.hpp"
#include "IBusinessModule.hpp"

#include "Database/IRepository.hpp"

#include "common/Entities/Role.hpp"
#include "common/Entities/User.hpp"

class RepositoryManager;

class UsersModule : public IBusinessModule {
public:
    UsersModule(RepositoryManager &repositoryManager);
    virtual ~UsersModule() override;

    virtual ResponseData executeTask(const RequestData &requestData) override;

private:
    ResponseData loginUser(const Dataset &request);

    ResponseData getUsers();
    void addUser(const Dataset &request);
    void editUser(const Dataset &request, const std::string &userId);
    void deleteUser(const std::string &userId);

    ResponseData getRoles();
    void addRole(const Dataset &request);
    void updateRole(const Dataset &request, const std::string &resourceId);
    void deleteRole(const std::string &resourceId);

private:
    std::shared_ptr<IRepository<Common::Entities::User>> m_usersRepository;
    std::shared_ptr<IRepository<Common::Entities::Role>> m_rolesRepository;
};
