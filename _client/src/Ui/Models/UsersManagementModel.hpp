#pragma once

#include "BaseModel.hpp"

#include "common/Entities/Role.hpp"
#include "common/Entities/User.hpp"

#include <QObject>
#include <QVector>

class ApiManager;

class UsersManagementModel : public BaseModel {
    Q_OBJECT
public:
    explicit UsersManagementModel(ApiManager &apiManager, QObject *parent = nullptr);

    QVector<User> users() const;
    QVector<Role> roles() const;

signals:
    void usersChanged();
    void userAdded();
    void userEdited();
    void userDeleted();

    void rolesChanged();
    void roleCreated();
    void roleEdited();
    void roleDeleted();

    void errorOccurred(const QString &errorMessage);

public slots:
    // Users
    void fetchUsers();
    void addUser(const User &user);
    void editUser(const User &user);
    void deleteUser(const QString &id);

    // Roles
    void fetchRoles();
    void createRole(const Role &role);
    void editRole(const Role &role);
    void deleteRole(const QString &id);

private slots: // -- ApiManager signal handlers ---
    void onUserAdded();
    void onUserUpdated();
    void onUserDeleted();
    void onUsersList(const std::vector<User> &users);

    void onRoleCreated();
    void onRoleEdited();
    void onRoleDeleted();
    void onRolesList(const std::vector<Role> &roles);

private:
    ApiManager &m_apiManager;

    QVector<User> m_users;
    QVector<Role> m_roles;
};
