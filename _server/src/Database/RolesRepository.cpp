#include "RolesRepository.hpp"
#include "SpdlogConfig.hpp"

RolesRepository::RolesRepository(std::shared_ptr<DatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("RolesRepository::RolesRepository");
}

RolesRepository::~RolesRepository()
{
    SPDLOG_TRACE("RolesRepository::~RolesRepository");
}

void RolesRepository::add(const Role &entity)
{
    SPDLOG_TRACE("RolesRepository::add");

    // id is autoincremented
    executePrepared("INSERT INTO Roles (name) VALUES (?);",
                    {entity.name});
}

void RolesRepository::update(const Role &entity)
{
    SPDLOG_TRACE("RolesRepository::update");
    executePrepared("UPDATE Roles SET name = ? WHERE id = ?;",
                    {entity.name, entity.id});
}

void RolesRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("RolesRepository::deleteResource | id = {}", id);
    executePrepared("DELETE FROM Roles WHERE id = ?;", {id});
}

std::optional<Role> RolesRepository::getById(const std::string &id) const
{
    SPDLOG_TRACE("RolesRepository::getById | id = {}", id);
    m_dbManager->prepareStatement("SELECT id, name FROM Roles WHERE id = ?;");
    m_dbManager->bind(1, id);
    if (m_dbManager->step())
    {
        auto role = roleFromCurrentRow();
        m_dbManager->finalizeStatement();
        return role;
    }
    m_dbManager->finalizeStatement();
    return std::nullopt;
}

std::vector<Role> RolesRepository::getAll() const
{
    SPDLOG_TRACE("RolesRepository::getAll");
    std::vector<Role> roles;
    m_dbManager->prepareStatement("SELECT id, name FROM Roles;");
    while (m_dbManager->step())
    {
        if (auto role = roleFromCurrentRow())
        {
            roles.push_back(*role);
        }
    }
    m_dbManager->finalizeStatement();
    return roles;
}

void RolesRepository::executePrepared(const std::string &query, const std::vector<std::string> &params) const
{
    m_dbManager->prepareStatement(query);
    for (size_t i = 0; i < params.size(); ++i)
    {
        m_dbManager->bind(static_cast<int>(i + 1), params[i]);
    }
    m_dbManager->step();
    m_dbManager->finalizeStatement();
}

std::optional<Role> RolesRepository::roleFromCurrentRow() const
{
    return Role{
        m_dbManager->columnText(0),
        m_dbManager->columnText(1)};
}
