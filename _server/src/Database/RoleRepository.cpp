#include "RoleRepository.hpp"
#include "common/SpdlogConfig.hpp"

#include "common/Entities/Role.hpp"

inline constexpr int ID   = 0;
inline constexpr int NAME = 1;

RoleRepository::RoleRepository(std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("RoleRepository::RoleRepository");
}

RoleRepository::~RoleRepository() { SPDLOG_TRACE("RoleRepository::~RoleRepository"); }

void RoleRepository::add(const Role &entity)
{
    SPDLOG_TRACE("RoleRepository::add");
    const std::string query =
        "INSERT INTO " + std::string(Role::TABLE_NAME) + " (name) VALUES (?);";
    m_dbManager->executeQuery(query, {entity.name});
}

void RoleRepository::update(const Role &entity)
{
    SPDLOG_TRACE("RoleRepository::update");
    const std::string query =
        "UPDATE " + std::string(Role::TABLE_NAME) + " SET name = ? WHERE id = ?;";
    m_dbManager->executeQuery(query, {entity.name, entity.id});
}

void RoleRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("RoleRepository::deleteResource | id = {}", id);
    const std::string query = "DELETE FROM " + std::string(Role::TABLE_NAME) + " WHERE id = ?;";
    m_dbManager->executeQuery(query, {id});
}

std::vector<Role> RoleRepository::getByField(const std::string &fieldName,
                                             const std::string &value) const
{
    SPDLOG_TRACE("RoleRepository::getByField | {} = {}", fieldName, value);
    std::vector<Role> roles;

    const std::string query =
        "SELECT id, name FROM " + std::string(Role::TABLE_NAME) + " WHERE " + fieldName + " = ?;";
    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next())
        roles.emplace_back(roleFromCurrentRow(result));

    return roles;
}

std::vector<Role> RoleRepository::getAll() const
{
    SPDLOG_TRACE("RoleRepository::getAll");
    std::vector<Role> roles;

    const std::string query = "SELECT id, name FROM " + std::string(Role::TABLE_NAME) + ";";
    auto result             = m_dbManager->executeQuery(query, {});
    while (result && result->next())
        roles.emplace_back(roleFromCurrentRow(result));

    return roles;
}

Role RoleRepository::roleFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const
{
    return Role{.id = queryResult->getString(ID), .name = queryResult->getString(NAME)};
}
