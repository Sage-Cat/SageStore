#pragma once

#include <string>
#include <memory>

#include "IBusinessModule.hpp"
#include "DataSpecs.hpp"

class UsersRepository;
class RolesRepository;

class UsersModule : public IBusinessModule
{
public:
    UsersModule(std::shared_ptr<UsersRepository> usersRepository,
                std::shared_ptr<RolesRepository> rolesRepository);
    virtual ~UsersModule() override;

    virtual ResponseData executeTask(const RequestData &requestData) override;

private:
    ResponseData loginUser(const Dataset &request);
    ResponseData registerUser(const Dataset &request);

private:
    std::shared_ptr<UsersRepository> m_usersRepository;
    std::shared_ptr<RolesRepository> m_rolesRepository;
};
