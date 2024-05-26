#include "UsersManagementModel.hpp"

#include "Network/ApiManager.hpp"

#include "common/SpdlogConfig.hpp"

UsersManagementModel::UsersManagementModel(ApiManager &apiManager, QObject *parent)
    : BaseModel(parent), m_apiManager(apiManager)
{
    SPDLOG_TRACE("UsersManagementModel::UsersManagementModel");

    connect(&m_apiManager, &ApiManager::userAdded, this, &UsersManagementModel::onUserAdded);
    connect(&m_apiManager, &ApiManager::userEdited, this, &UsersManagementModel::onUserUpdated);
    connect(&m_apiManager, &ApiManager::userDeleted, this, &UsersManagementModel::onUserDeleted);
    connect(&m_apiManager, &ApiManager::usersList, this, &UsersManagementModel::onUsersList);

    connect(&m_apiManager, &ApiManager::roleCreated, this, &UsersManagementModel::onRoleCreated);
    connect(&m_apiManager, &ApiManager::roleEdited, this, &UsersManagementModel::onRoleEdited);
    connect(&m_apiManager, &ApiManager::roleDeleted, this, &UsersManagementModel::onRoleDeleted);
    connect(&m_apiManager, &ApiManager::rolesList, this, &UsersManagementModel::onRolesList);

    // Fetch data on startup
    SPDLOG_TRACE("UsersManagementModel | On startup fetch data");
    fetchRoles();
    fetchUsers();
}

QVector<Common::Entities::User> UsersManagementModel::users() const { return m_users; }

QVector<Common::Entities::Role> UsersManagementModel::roles() const { return m_roles; }

void UsersManagementModel::fetchUsers() { m_apiManager.getUsers(); }

void UsersManagementModel::addUser(const Common::Entities::User &user)
{
    m_apiManager.addUser(user);
}

void UsersManagementModel::editUser(const Common::Entities::User &user)
{
    m_apiManager.editUser(user);
}

void UsersManagementModel::deleteUser(const QString &id) { m_apiManager.deleteUser(id); }

void UsersManagementModel::fetchRoles() { m_apiManager.getRoleList(); }

void UsersManagementModel::createRole(const Common::Entities::Role &role)
{
    m_apiManager.createRole(role);
}

void UsersManagementModel::editRole(const Common::Entities::Role &role)
{
    m_apiManager.editRole(role);
}

void UsersManagementModel::deleteRole(const QString &id) { m_apiManager.deleteRole(id); }

void UsersManagementModel::onUsersList(const std::vector<Common::Entities::User> &users)
{
    m_users = QVector<Common::Entities::User>(users.begin(), users.end());
    emit usersChanged();
}

void UsersManagementModel::onUserAdded()
{
    fetchUsers();
    userAdded();
}

void UsersManagementModel::onUserUpdated()
{
    fetchUsers();
    emit userEdited();
}

void UsersManagementModel::onUserDeleted()
{
    fetchUsers();
    emit userDeleted();
}

void UsersManagementModel::onRolesList(const std::vector<Common::Entities::Role> &roles)
{
    m_roles = QVector<Common::Entities::Role>(roles.begin(), roles.end());
    emit rolesChanged();
}

void UsersManagementModel::onRoleCreated()
{
    fetchRoles();
    emit roleCreated();
}

void UsersManagementModel::onRoleEdited()
{
    fetchRoles();
    emit roleEdited();
}

void UsersManagementModel::onRoleDeleted()
{
    fetchRoles();
    emit roleDeleted();
}
