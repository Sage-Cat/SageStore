#pragma once

#include <QObject>
#include <QMap>
#include <QVector>

#include "DataTypes.hpp"

class NetworkService;
enum class Method;

struct Role
{
    QString id;
    QString name;
    Role() = default;
    Role(QString id, QString name) : id{id},
                                     name{name}
    {
    }
};

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

public slots:
    // for UI
    virtual void loginUser(const QString &username, const QString &password);
    virtual void registerUser(const QString &username, const QString &password);
    virtual void getRoleList();
    virtual void createNewRole(const QString &roleName);
    virtual void editRole(const QString &id, const QString &roleName);
    virtual void deleteRole(const QString &id);
protected slots:
    // for NetworkService
    virtual void
    handleResponse(const QString &endpoint, Method method, const Dataset &dataset);

signals:
    void loginSuccess(const QString &id, const QString &roleId);
    void registrationSuccess();
    void rolesList(const QVector<Role> &roleList);
    void newRoleCreated();
    void roleEdited();
    void roleDeleted();
    // Error handling
    void errorOccurred(const QString &errorMessage);

private:
    void init();
    void setupHandlers();

    // handlers
    void handleError(const QString &errorMessage);
    void handleLoginResponse(Method method, const Dataset &dataset);
    void handleRegistrationResponse(Method method, const Dataset &dataset);
    void handleRoles(Method method, const Dataset &dataset);
    void handleRoleList(const Dataset &dataset);

private:
    NetworkService &m_networkService;
    QMap<QString, ResponseHandler> m_responseHandlers;
};
