#pragma once

#include <QObject>
#include <QMap>

#include "DataTypes.hpp"

class NetworkService;

/**
 * @class ApiManager
 * @brief Manages API calls for user authentication and registration processes.
 */
class ApiManager : public QObject
{
    Q_OBJECT

    using ResponseHandler = std::function<void(const Dataset &)>;

public:
    ApiManager(QString serverApiUrl);
    ~ApiManager();

    void loginUser(const QString &username, const QString &password);
    void registerUser(const QString &username, const QString &password);

private:
    void init();
    void setupNetworkService();
    void setupHandlers();

signals:
    void loginSuccess(const QString &id, const QString &roleId);
    void registerSuccess();

    // Error handling
    void errorOccurred(const QString &errorMessage);

private slots:
    void handleResponse(const QString &endpoint, const Dataset &dataset);
    void handleError(const QString &errorMessage);

private: // handlers for server's response
    void handleLoginResponse(const Dataset &dataset);
    void handleRegistrationResponse(const Dataset &dataset);

private:
    NetworkService *m_networkService;
    QMap<QString, ResponseHandler> m_responseHandlers;
};
