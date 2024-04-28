#pragma once

#include <memory>
#include <type_traits>

#include "Database/IDatabaseManager.hpp"
#include "Database/IRepository.hpp"

class User;
class Role;

class RepositoryManager
{
public:
    explicit RepositoryManager(std::shared_ptr<IDatabaseManager> dbManager);
    ~RepositoryManager();

    [[nodiscard]] virtual auto getUserRepository() -> std::shared_ptr<IRepository<User>>;
    [[nodiscard]] virtual auto getRoleRepository() -> std::shared_ptr<IRepository<Role>>;

private:
    std::shared_ptr<IDatabaseManager> m_dbManager;

    // Repositories
    std::shared_ptr<IRepository<User>> m_usersRepository;
    std::shared_ptr<IRepository<Role>> m_rolesRepository;
};
