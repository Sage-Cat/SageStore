#include "Database/UsersRepository.hpp"
#include "SpdlogConfig.hpp"

#include "Database/Entities/User.hpp"

namespace
{
    inline constexpr int ID = 0;
    inline constexpr int USERNAME = 1;
    inline constexpr int PASSWORD = 2;
    inline constexpr int ROLE_ID = 3;
}

UsersRepository::UsersRepository(std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("UsersRepository::UsersRepository");
}

UsersRepository::~UsersRepository()
{
    SPDLOG_TRACE("UsersRepository::~UsersRepository");
}

void UsersRepository::add(const User &entity)
{
    SPDLOG_TRACE("UsersRepository::add");
    const std::string query = std::string("INSERT INTO Users (username, password) ") +
                              (entity.roleId.empty() ? "VALUES (?, ?);" : "VALUES (?, ?, ?);");

    const std::vector<std::string> params = entity.roleId.empty()
                                                ? std::vector<std::string>{entity.username, entity.password}
                                                : std::vector<std::string>{entity.username, entity.password, entity.roleId};
    m_dbManager->executeQuery(query, params);
}

void UsersRepository::update(const User &entity)
{
    SPDLOG_TRACE("UsersRepository::update");
    const std::string query = "UPDATE Users SET username = ?, password = ?, roleId = ? WHERE id = ?;";
    const std::vector<std::string> params = {entity.username, entity.password, entity.roleId, entity.id};

    m_dbManager->executeQuery(query, params);
}

void UsersRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("UsersRepository::deleteResource | id = {}", id);
    const std::string query = "DELETE FROM Users WHERE id = ?;";
    const std::vector<std::string> params = {id};

    m_dbManager->executeQuery(query, params);
}

std::vector<User> UsersRepository::getAll() const
{
    SPDLOG_TRACE("UsersRepository::getAll");
    std::vector<User> users;

    const std::string query = "SELECT id, username, password, roleId FROM Users;";
    auto result = m_dbManager->executeQuery(query, {});
    while (result && result->next())
        users.emplace_back(userFromCurrentRow(result));

    return users;
}

User UsersRepository::userFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const
{
    return User(
        queryResult->getString(ID),
        queryResult->getString(USERNAME),
        queryResult->getString(PASSWORD),
        queryResult->getString(ROLE_ID));
}
std::vector<User> UsersRepository::getByField(const std::string &fieldName, const std::string &value) const
{
    SPDLOG_TRACE("UsersRepository::getByField | {} = {}", fieldName, value);
    std::vector<User> users;

    const std::string query = "SELECT id, username, password, roleId FROM Users WHERE " + fieldName + " = ?;";
    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next())
        users.emplace_back(userFromCurrentRow(result));

    return users;
}