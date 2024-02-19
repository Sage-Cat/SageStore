#pragma once

#include <memory>

class DatabaseManager;
class UsersRepository;
class RolesRepository;

class RepositoryManager
{
public:
    explicit RepositoryManager(std::shared_ptr<DatabaseManager> dbManager);

    std::shared_ptr<UsersRepository> getUsersRepository();
    std::shared_ptr<RolesRepository> getRolesRepository();

private:
    std::shared_ptr<DatabaseManager> m_dbManager;

    // Repositories
    std::shared_ptr<UsersRepository> m_usersRepository;
    std::shared_ptr<RolesRepository> m_rolesRepository;
};
