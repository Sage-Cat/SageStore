#include "Database/RepositoryManager.hpp"

#include "Database/UsersRepository.hpp"
#include "RolesRepository.hpp"

#include "SpdlogConfig.hpp"

RepositoryManager::RepositoryManager(std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("RepositoryManager::RepositoryManager");

    m_dbManager->open();
}

RepositoryManager::~RepositoryManager()
{
    m_dbManager->close();
}

auto RepositoryManager::getUsersRepository() -> std::shared_ptr<IRepository<User>>
{
    SPDLOG_TRACE("RepositoryManager::getUsersRepository");
    if (!m_usersRepository)
    {
        m_usersRepository = std::make_shared<UsersRepository>(m_dbManager);
    }
    return m_usersRepository;
}

auto RepositoryManager::getRolesRepository() -> std::shared_ptr<IRepository<Role>>
{
    SPDLOG_TRACE("RepositoryManager::getRolesRepository");
    if (!m_rolesRepository)
    {
        m_rolesRepository = std::make_shared<RolesRepository>(m_dbManager);
    }
    return m_rolesRepository;
}
