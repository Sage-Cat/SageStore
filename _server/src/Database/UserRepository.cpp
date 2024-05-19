#include "UserRepository.hpp"

#include "common/Entities/User.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
inline constexpr int ID       = 0;
inline constexpr int USERNAME = 1;
inline constexpr int PASSWORD = 2;
inline constexpr int ROLE_ID  = 3;
} // namespace

UserRepository::UserRepository(std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("UserRepository::UserRepository");
}

UserRepository::~UserRepository() { SPDLOG_TRACE("UserRepository::~UserRepository"); }

void UserRepository::add(const User &entity)
{
    SPDLOG_TRACE("UserRepository::add");
    const std::string query = "INSERT INTO " + std::string(User::TABLE_NAME) +
                              " (username, password) " +
                              (entity.roleId.empty() ? "VALUES (?, ?);" : "VALUES (?, ?, ?);");

    const std::vector<std::string> params =
        entity.roleId.empty()
            ? std::vector<std::string>{entity.username, entity.password}
            : std::vector<std::string>{entity.username, entity.password, entity.roleId};
    m_dbManager->executeQuery(query, params);
}

void UserRepository::update(const User &entity)
{
    SPDLOG_TRACE("UserRepository::update");
    const std::string query = "UPDATE " + std::string(User::TABLE_NAME) +
                              " SET username = ?, password = ?, roleId = ? WHERE id = ?;";
    const std::vector<std::string> params = {entity.username, entity.password, entity.roleId,
                                             entity.id};

    m_dbManager->executeQuery(query, params);
}

void UserRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("UserRepository::deleteResource | id = {}", id);
    const std::string query = "DELETE FROM " + std::string(User::TABLE_NAME) + " WHERE id = ?;";
    const std::vector<std::string> params = {id};

    m_dbManager->executeQuery(query, params);
}

std::vector<User> UserRepository::getByField(const std::string &fieldName,
                                             const std::string &value) const
{
    SPDLOG_TRACE("UserRepository::getByField | {} = {}", fieldName, value);
    std::vector<User> users;

    const std::string query = "SELECT id, username, password, roleId FROM " +
                              std::string(User::TABLE_NAME) + " WHERE " + fieldName + " = ?;";
    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next())
        users.emplace_back(userFromCurrentRow(result));

    return users;
}

std::vector<User> UserRepository::getAll() const
{
    SPDLOG_TRACE("UserRepository::getAll");
    std::vector<User> users;

    const std::string query =
        "SELECT id, username, password, roleId FROM  " + std::string(User::TABLE_NAME) + " ;";
    auto result = m_dbManager->executeQuery(query, {});
    while (result && result->next())
        users.emplace_back(userFromCurrentRow(result));

    return users;
}

User UserRepository::userFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const
{
    return User{.id       = queryResult->getString(ID),
                .username = queryResult->getString(USERNAME),
                .password = queryResult->getString(PASSWORD),
                .roleId   = queryResult->getString(ROLE_ID)};
}
