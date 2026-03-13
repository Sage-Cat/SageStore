#include "LogRepository.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
inline constexpr int ID        = 0;
inline constexpr int USER_ID   = 1;
inline constexpr int ACTION    = 2;
inline constexpr int TIMESTAMP = 3;
} // namespace

LogRepository::LogRepository(std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("LogRepository::LogRepository");
}

LogRepository::~LogRepository() { SPDLOG_TRACE("LogRepository::~LogRepository"); }

void LogRepository::add(const Common::Entities::Log &entity)
{
    SPDLOG_TRACE("LogRepository::add");

    const std::string query = "INSERT INTO " + std::string(Common::Entities::Log::TABLE_NAME) +
                              " (userId, action, timestamp) VALUES (NULLIF(?, ''), ?, ?);";
    m_dbManager->executeQuery(query, {entity.userId, entity.action, entity.timestamp});
}

void LogRepository::update(const Common::Entities::Log &entity)
{
    SPDLOG_TRACE("LogRepository::update");

    const std::string query = "UPDATE " + std::string(Common::Entities::Log::TABLE_NAME) +
                              " SET userId = NULLIF(?, ''), action = ?, timestamp = ? WHERE id = ?;";
    m_dbManager->executeQuery(query, {entity.userId, entity.action, entity.timestamp, entity.id});
}

void LogRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("LogRepository::deleteResource | id = {}", id);

    const std::string query =
        "DELETE FROM " + std::string(Common::Entities::Log::TABLE_NAME) + " WHERE id = ?;";
    m_dbManager->executeQuery(query, {id});
}

std::vector<Common::Entities::Log>
LogRepository::getByField(const std::string &fieldName, const std::string &value) const
{
    SPDLOG_TRACE("LogRepository::getByField | {} = {}", fieldName, value);

    std::vector<Common::Entities::Log> logs;
    const std::string query =
        "SELECT id, userId, action, timestamp FROM " +
        std::string(Common::Entities::Log::TABLE_NAME) + " WHERE " + fieldName + " = ?;";

    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next()) {
        logs.emplace_back(logFromCurrentRow(result));
    }

    return logs;
}

std::vector<Common::Entities::Log> LogRepository::getAll() const
{
    SPDLOG_TRACE("LogRepository::getAll");

    std::vector<Common::Entities::Log> logs;
    const std::string query = "SELECT id, userId, action, timestamp FROM " +
                              std::string(Common::Entities::Log::TABLE_NAME) + ";";

    auto result = m_dbManager->executeQuery(query, {});
    while (result && result->next()) {
        logs.emplace_back(logFromCurrentRow(result));
    }

    return logs;
}

Common::Entities::Log
LogRepository::logFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const
{
    return Common::Entities::Log{.id = queryResult->getString(ID),
                                 .userId = queryResult->getString(USER_ID),
                                 .action = queryResult->getString(ACTION),
                                 .timestamp = queryResult->getString(TIMESTAMP)};
}
