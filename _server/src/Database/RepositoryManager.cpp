#include "RepositoryManager.hpp"

#include "RoleRepository.hpp"
#include "UserRepository.hpp"

#include "common/SpdlogConfig.hpp"

RepositoryManager::RepositoryManager(std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("RepositoryManager::RepositoryManager");

    m_dbManager->open();
}

RepositoryManager::~RepositoryManager() { m_dbManager->close(); }

auto RepositoryManager::getUserRepository() -> std::shared_ptr<IRepository<Common::Entities::User>>
{
    SPDLOG_TRACE("RepositoryManager::getUserRepository");
    if (!m_usersRepository) {
        m_usersRepository = std::make_shared<UserRepository>(m_dbManager);
    }
    return m_usersRepository;
}

auto RepositoryManager::getRoleRepository() -> std::shared_ptr<IRepository<Common::Entities::Role>>
{
    SPDLOG_TRACE("RepositoryManager::getRoleRepository");
    if (!m_rolesRepository) {
        m_rolesRepository = std::make_shared<RoleRepository>(m_dbManager);
    }
    return m_rolesRepository;
}
