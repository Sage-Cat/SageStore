#pragma once

#include <QObject>
#include <QMap>

#include "DataTypes.hpp"

class NetworkService;
enum class Method;

/**
 * @class ApiManager
 * @brief Manages API calls for user authentication and registration processes.
 */
class ApiManager : public QObject
{
    Q_OBJECT

    using ResponseHandler = std::function<void(Method method, const Dataset &)>;

public:
    ApiManager(NetworkService &networkService);
    ~ApiManager();

public slots: // for UI
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

private slots: // for NetworkService
    void handleResponse(const QString &endpoint, Method method, const Dataset &dataset);

private:
    void handleError(const QString &errorMessage);

    // specific handlers
    void handleLoginResponse(Method method, const Dataset &dataset);
    void handleRegistrationResponse(Method method, const Dataset &dataset);

private:
    NetworkService &m_networkService;
    QMap<QString, ResponseHandler> m_responseHandlers;
};
