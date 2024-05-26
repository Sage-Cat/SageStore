#include "UsersManagementModel.hpp"

#include "Network/ApiManager.hpp"

UsersManagementModel::UsersManagementModel(ApiManager &apiManager, QObject *parent)
    : BaseModel(parent), m_apiManager(apiManager)
{
    connect(&m_apiManager, &ApiManager::userAdded, this, &UsersManagementModel::onUserAdded);
    connect(&m_apiManager, &ApiManager::userEdited, this, &UsersManagementModel::onUserUpdated);
    connect(&m_apiManager, &ApiManager::userDeleted, this, &UsersManagementModel::onUserDeleted);
    connect(&m_apiManager, &ApiManager::usersList, this, &UsersManagementModel::onUsersList);

    connect(&m_apiManager, &ApiManager::roleCreated, this, &UsersManagementModel::onRoleCreated);
    connect(&m_apiManager, &ApiManager::roleEdited, this, &UsersManagementModel::onRoleEdited);
    connect(&m_apiManager, &ApiManager::roleDeleted, this, &UsersManagementModel::onRoleDeleted);
    connect(&m_apiManager, &ApiManager::rolesList, this, &UsersManagementModel::onRolesList);

    connect(&m_apiManager, &ApiManager::errorOccurred, this, &UsersManagementModel::errorOccurred);
}

QVector<User> UsersManagementModel::users() const { return m_users; }

QVector<Role> UsersManagementModel::roles() const { return m_roles; }

void UsersManagementModel::fetchUsers() { m_apiManager.getUsers(); }

void UsersManagementModel::addUser(const User &user) { m_apiManager.addUser(user); }

void UsersManagementModel::editUser(const User &user) { m_apiManager.editUser(user); }

void UsersManagementModel::deleteUser(const QString &id) { m_apiManager.deleteUser(id); }

void UsersManagementModel::fetchRoles() { m_apiManager.getRoleList(); }

void UsersManagementModel::createRole(const Role &role) { m_apiManager.createRole(role); }

void UsersManagementModel::editRole(const Role &role) { m_apiManager.editRole(role); }

void UsersManagementModel::deleteRole(const QString &id) { m_apiManager.deleteRole(id); }

void UsersManagementModel::onUsersList(const std::vector<User> &users)
{
    m_users = QVector<User>(users.begin(), users.end());
    emit usersChanged();
}

void UsersManagementModel::onUserAdded() { emit userAdded(); }

void UsersManagementModel::onUserUpdated() { emit userEdited(); }

void UsersManagementModel::onUserDeleted() { emit userDeleted(); }

void UsersManagementModel::onRolesList(const std::vector<Role> &roles)
{
    m_roles = QVector<Role>(roles.begin(), roles.end());
    emit rolesChanged();
}

void UsersManagementModel::onRoleCreated() { emit roleCreated(); }

void UsersManagementModel::onRoleEdited() { emit roleEdited(); }

void UsersManagementModel::onRoleDeleted() { emit roleDeleted(); }
