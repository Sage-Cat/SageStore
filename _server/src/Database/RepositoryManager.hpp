#pragma once

#include <memory>
#include <type_traits>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"

class User;
class Role;

class RepositoryManager
{
public:
    explicit RepositoryManager(std::shared_ptr<IDatabaseManager> dbManager);

    [[nodiscard]] virtual auto getUsersRepository() -> std::shared_ptr<IRepository<User>>;
    [[nodiscard]] virtual auto getRolesRepository() -> std::shared_ptr<IRepository<Role>>;

private:
    std::shared_ptr<IDatabaseManager> m_dbManager;

    // Repositories
    std::shared_ptr<IRepository<User>> m_usersRepository;
    std::shared_ptr<IRepository<Role>> m_rolesRepository;
};
