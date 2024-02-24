#include "UsersRepository.hpp"
#include "SpdlogConfig.hpp"

inline constexpr int ID = 0;
inline constexpr int USERNAME = 1;
inline constexpr int PASSWORD = 2;
inline constexpr int ROLE_ID = 3;

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

std::optional<User> UsersRepository::getById(const std::string &id) const
{
    SPDLOG_TRACE("UsersRepository::getById | id = {}", id);
    const std::string query = "SELECT id, username, password, roleId FROM Users WHERE id = ?;";
    auto result = m_dbManager->executeQuery(query, {id});

    if (result && result->next())
        return userFromCurrentRow(result);

    return std::nullopt;
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

std::optional<User> UsersRepository::getByUsername(const std::string &username) const
{
    SPDLOG_TRACE("UsersRepository::getUserByUsername | username = {}", username);
    const std::string query = "SELECT id, username, password, roleId FROM Users WHERE username = ?;";
    auto result = m_dbManager->executeQuery(query, {username});

    if (result && result->next())
        return userFromCurrentRow(result);

    return std::nullopt;
}

User UsersRepository::userFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const
{
    return User(
        queryResult->getString(ID),
        queryResult->getString(USERNAME),
        queryResult->getString(PASSWORD),
        queryResult->getString(ROLE_ID));
}
