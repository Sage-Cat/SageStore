#pragma once

#include <QObject>
#include <QMap>
#include <functional>

#include "NetworkService.hpp"

using ResponseHandler = std::function<void(const Dataset &)>;

class ApiClient : public QObject
{
    Q_OBJECT

public:
    ApiClient(NetworkService *networkService, QObject *parent = nullptr);

    void loginUser(const QString &username, const QString &password);
    void registerUser(const QString &username, const QString &password);

private:
    void setupHandlers();

signals:
    void loginSuccess();
    void loginFailed(const QString &errorMessage);

    void registerSuccess();
    void registerFailed(const QString &errorMessage);

private slots:
    void handleResponse(const Dataset &dataset);

    void handleLoginResponse(const Dataset &dataset);
    void handleRegistrationResponse(const Dataset &dataset);

private:
    NetworkService *m_networkService;
    QMap<QString, ResponseHandler> m_responseHandlers;
    QString m_currentUserToken;
};
