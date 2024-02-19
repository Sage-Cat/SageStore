#include "UsersRepository.hpp"

#include "SpdlogConfig.hpp"

UsersRepository::UsersRepository(std::shared_ptr<DatabaseManager> dbManager)
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

    // id is autoincremented
    executePrepared("INSERT INTO Users (Username, Password, RoleID) VALUES (?, ?, ?);",
                    {entity.username, entity.password, entity.roleId});
}

void UsersRepository::update(const User &entity)
{
    SPDLOG_TRACE("UsersRepository::update");
    executePrepared("UPDATE Users SET Username = ?, Password = ?, RoleID = ? WHERE UserID = ?;",
                    {entity.username, entity.password, entity.roleId, entity.id});
}

void UsersRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("UsersRepository::deleteResource | id = {}", id);
    executePrepared("DELETE FROM Users WHERE UserID = ?;", {id});
}

std::optional<User> UsersRepository::getById(const std::string &id) const
{
    SPDLOG_TRACE("UsersRepository::getById | id = {}", id);

    m_dbManager->prepareStatement("SELECT UserID, Username, Password, RoleID FROM Users WHERE UserID = ?;");
    m_dbManager->bind(1, id);

    if (m_dbManager->step())
    {
        auto user = userFromCurrentRow();
        m_dbManager->finalizeStatement();
        return user;
    }

    m_dbManager->finalizeStatement();
    return std::nullopt;
}

std::vector<User> UsersRepository::getAll() const
{
    SPDLOG_TRACE("UsersRepository::getAll");
    std::vector<User> users;
    m_dbManager->prepareStatement("SELECT UserID, Username, Password, RoleID FROM Users;");
    while (m_dbManager->step())
    {
        if (auto user = userFromCurrentRow())
        {
            users.push_back(*user);
        }
    }
    m_dbManager->finalizeStatement();
    return users;
}

std::optional<User> UsersRepository::getByUsername(const std::string &username) const
{
    SPDLOG_TRACE("UsersRepository::getUserByUsername | username = {}", username);

    m_dbManager->prepareStatement("SELECT UserID, Username, Password, RoleID FROM Users WHERE Username = ?;");
    m_dbManager->bind(1, username);

    if (m_dbManager->step())
    {
        auto user = userFromCurrentRow();
        m_dbManager->finalizeStatement();
        return user;
    }

    m_dbManager->finalizeStatement();
    return std::nullopt;
}

void UsersRepository::executePrepared(const std::string &query, const std::vector<std::string> &params) const
{
    m_dbManager->prepareStatement(query);
    for (size_t i = 0; i < params.size(); ++i)
    {
        m_dbManager->bind(static_cast<int>(i + 1), params[i]);
    }
    m_dbManager->step();
    m_dbManager->finalizeStatement();
}

std::optional<User> UsersRepository::userFromCurrentRow() const
{
    return User{
        m_dbManager->columnText(0),
        m_dbManager->columnText(1),
        m_dbManager->columnText(2),
        m_dbManager->columnText(3)};
}
