#pragma once

#include <memory>
#include <type_traits>

#include "IDatabaseManager.hpp"

class UsersRepository;
class RolesRepository;

class RepositoryManager
{
public:
    explicit RepositoryManager(std::shared_ptr<IDatabaseManager> dbManager);

    [[nodiscard]] auto getUsersRepository() -> std::shared_ptr<UsersRepository>;
    [[nodiscard]] auto getRolesRepository() -> std::shared_ptr<RolesRepository>;

private:
    std::shared_ptr<IDatabaseManager> m_dbManager;

    // Repositories
    std::shared_ptr<UsersRepository> m_usersRepository;
    std::shared_ptr<RolesRepository> m_rolesRepository;
};
