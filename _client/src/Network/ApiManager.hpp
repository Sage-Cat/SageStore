#pragma once

#include <map>
#include <list>

#include <QObject>

#include "common/DataTypes.hpp"

#include "common/Entities/Role.hpp"

class NetworkService;
enum class Method;

/**
 * @class ApiManager
 * @brief Manages API calls for user authentication and registration processes.
 *
 * @attention For external communication (with UI) QString is used, but for internal (NetworkService) std::string
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
    virtual void handleResponse(const std::string &endpoint, Method method, const Dataset &dataset);

signals:
    void loginSuccess(const QString &id, const QString &roleId);
    void registrationSuccess();
    void rolesList(const std::list<Role> &roleList);
    void roleCreated();
    void roleEdited();
    void roleDeleted();

    // Error handling
    void errorOccurred(const QString &errorMessage);

private:
    void init();
    void setupHandlers();

    // handlers
    void handleError(const std::string &errorMessage);
    void handleLoginResponse(Method method, const Dataset &dataset);
    void handleRegistrationResponse(Method method, const Dataset &dataset);
    void handleRoles(Method method, const Dataset &dataset);
    void handleRolesList(const Dataset &dataset);

private:
    NetworkService &m_networkService;
    std::map<std::string, ResponseHandler> m_responseHandlers;
};
