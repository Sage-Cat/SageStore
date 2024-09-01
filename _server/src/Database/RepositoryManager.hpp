#pragma once

#include <memory>
#include <type_traits>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"

#include "common/Entities/Role.hpp"
#include "common/Entities/User.hpp"

class RepositoryManager {
public:
    explicit RepositoryManager(std::shared_ptr<IDatabaseManager> dbManager);
    ~RepositoryManager();

    [[nodiscard]] virtual auto getUserRepository()
        -> std::shared_ptr<IRepository<Common::Entities::User>>;
    [[nodiscard]] virtual auto getRoleRepository()
        -> std::shared_ptr<IRepository<Common::Entities::Role>>;

private:
    std::shared_ptr<IDatabaseManager> m_dbManager;

    // Repositories
    std::shared_ptr<IRepository<Common::Entities::User>> m_usersRepository;
    std::shared_ptr<IRepository<Common::Entities::Role>> m_rolesRepository;
};
