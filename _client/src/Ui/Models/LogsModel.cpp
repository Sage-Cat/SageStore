#include "Ui/Models/LogsModel.hpp"

#include "Network/ApiManager.hpp"

LogsModel::LogsModel(ApiManager &apiManager, QObject *parent)
    : BaseModel(parent), m_apiManager(apiManager)
{
    connect(&m_apiManager, &ApiManager::errorOccurred, this, &LogsModel::errorOccurred);
    connect(&m_apiManager, &ApiManager::logsList, this, &LogsModel::onLogsList);
    connect(&m_apiManager, &ApiManager::usersList, this, &LogsModel::onUsersList);
}

QVector<Common::Entities::Log> LogsModel::logs() const { return m_logs; }

QVector<Common::Entities::User> LogsModel::users() const { return m_users; }

void LogsModel::fetchAll()
{
    m_apiManager.getUsers();
    m_apiManager.getLogs();
}

void LogsModel::onLogsList(const std::vector<Common::Entities::Log> &logs)
{
    m_logs = QVector<Common::Entities::Log>(logs.begin(), logs.end());
    emit logsChanged();
}

void LogsModel::onUsersList(const std::vector<Common::Entities::User> &users)
{
    m_users = QVector<Common::Entities::User>(users.begin(), users.end());
    emit usersChanged();
}
