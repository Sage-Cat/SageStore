#include "RoleRepository.hpp"
#include "common/SpdlogConfig.hpp"

inline constexpr int ID   = 0;
inline constexpr int NAME = 1;

RoleRepository::RoleRepository(std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("RoleRepository::RoleRepository");
}

RoleRepository::~RoleRepository() { SPDLOG_TRACE("RoleRepository::~RoleRepository"); }

void RoleRepository::add(const Common::Entities::Role &entity)
{
    SPDLOG_TRACE("RoleRepository::add");
    const std::string query =
        "INSERT INTO " + std::string(Common::Entities::Role::TABLE_NAME) + " (name) VALUES (?);";
    m_dbManager->executeQuery(query, {entity.name});
}

void RoleRepository::update(const Common::Entities::Role &entity)
{
    SPDLOG_TRACE("RoleRepository::update");
    const std::string query =
        "UPDATE " + std::string(Common::Entities::Role::TABLE_NAME) + " SET name = ? WHERE id = ?;";
    m_dbManager->executeQuery(query, {entity.name, entity.id});
}

void RoleRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("RoleRepository::deleteResource | id = {}", id);
    const std::string query =
        "DELETE FROM " + std::string(Common::Entities::Role::TABLE_NAME) + " WHERE id = ?;";
    m_dbManager->executeQuery(query, {id});
}

std::vector<Common::Entities::Role> RoleRepository::getByField(const std::string &fieldName,
                                                               const std::string &value) const
{
    SPDLOG_TRACE("RoleRepository::getByField | {} = {}", fieldName, value);
    std::vector<Common::Entities::Role> roles;

    const std::string query = "SELECT id, name FROM " +
                              std::string(Common::Entities::Role::TABLE_NAME) + " WHERE " +
                              fieldName + " = ?;";
    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next())
        roles.emplace_back(roleFromCurrentRow(result));

    return roles;
}

std::vector<Common::Entities::Role> RoleRepository::getAll() const
{
    SPDLOG_TRACE("RoleRepository::getAll");
    std::vector<Common::Entities::Role> roles;

    const std::string query =
        "SELECT id, name FROM " + std::string(Common::Entities::Role::TABLE_NAME) + ";";
    auto result = m_dbManager->executeQuery(query, {});
    while (result && result->next())
        roles.emplace_back(roleFromCurrentRow(result));

    return roles;
}

Common::Entities::Role
RoleRepository::roleFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const
{
    return Common::Entities::Role{.id   = queryResult->getString(ID),
                                  .name = queryResult->getString(NAME)};
}
