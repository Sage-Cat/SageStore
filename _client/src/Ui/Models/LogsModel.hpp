#pragma once

#include <QVector>

#include "Ui/Models/BaseModel.hpp"

#include "common/Entities/Log.hpp"
#include "common/Entities/User.hpp"

class ApiManager;

class LogsModel : public BaseModel {
    Q_OBJECT

public:
    explicit LogsModel(ApiManager &apiManager, QObject *parent = nullptr);

    QVector<Common::Entities::Log> logs() const;
    QVector<Common::Entities::User> users() const;

signals:
    void logsChanged();
    void usersChanged();

public slots:
    void fetchAll();

private slots:
    void onLogsList(const std::vector<Common::Entities::Log> &logs);
    void onUsersList(const std::vector<Common::Entities::User> &users);

private:
    ApiManager &m_apiManager;
    QVector<Common::Entities::Log> m_logs;
    QVector<Common::Entities::User> m_users;
};
