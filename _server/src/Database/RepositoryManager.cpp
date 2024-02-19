#include "RepositoryManager.hpp"

#include "DatabaseManager.hpp"

#include "UsersRepository.hpp"
#include "RolesRepository.hpp"

#include "SpdlogConfig.hpp"

RepositoryManager::RepositoryManager(std::shared_ptr<DatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("RepositoryManager::RepositoryManager");
}

std::shared_ptr<UsersRepository> RepositoryManager::getUsersRepository()
{
    SPDLOG_TRACE("RepositoryManager::getUsersRepository");
    if (!m_usersRepository)
    {
        m_usersRepository = std::make_shared<UsersRepository>(m_dbManager);
    }
    return m_usersRepository;
}

std::shared_ptr<RolesRepository> RepositoryManager::getRolesRepository()
{
    SPDLOG_TRACE("RepositoryManager::getRolesRepository");
    if (!m_rolesRepository)
    {
        m_rolesRepository = std::make_shared<RolesRepository>(m_dbManager);
    }
    return m_rolesRepository;
}
