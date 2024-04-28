#include "Database/RolesRepository.hpp"
#include "SpdlogConfig.hpp"

#include "Database/Entities/Role.hpp"

inline constexpr int ID = 0;
inline constexpr int NAME = 1;

RolesRepository::RolesRepository(std::shared_ptr<IDatabaseManager> dbManager)
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
    const std::string query = "INSERT INTO Roles (name) VALUES (?);";
    m_dbManager->executeQuery(query, {entity.name});
}

void RolesRepository::update(const Role &entity)
{
    SPDLOG_TRACE("RolesRepository::update");
    const std::string query = "UPDATE Roles SET name = ? WHERE id = ?;";
    m_dbManager->executeQuery(query, {entity.name, entity.id});
}

void RolesRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("RolesRepository::deleteResource | id = {}", id);
    const std::string query = "DELETE FROM Roles WHERE id = ?;";
    m_dbManager->executeQuery(query, {id});
}

std::vector<Role> RolesRepository::getByField(const std::string &fieldName, const std::string &value) const
{
    SPDLOG_TRACE("RolesRepository::getByField | {} = {}", fieldName, value);
    std::vector<Role> roles;

    const std::string query = "SELECT id, name FROM Roles WHERE " + fieldName + " = ?;";
    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next())
        roles.emplace_back(roleFromCurrentRow(result));

    return roles;
}

std::vector<Role> RolesRepository::getAll() const
{
    SPDLOG_TRACE("RolesRepository::getAll");
    std::vector<Role> roles;

    const std::string query = "SELECT id, name FROM Roles;";
    auto result = m_dbManager->executeQuery(query, {});
    while (result && result->next())
        roles.emplace_back(roleFromCurrentRow(result));

    return roles;
}

Role RolesRepository::roleFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const
{
    return Role(
        queryResult->getString(ID),
        queryResult->getString(NAME));
}
