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
    
    QString getUsernameById(const std::string &userId) const;

    QVector<Common::Entities::User> users() const;
    QVector<Common::Entities::Role> roles() const;

signals:
    void usersChanged();
    void userAdded();
    void userEdited();
    void userDeleted();

    void rolesChanged();
    void roleCreated();
    void roleEdited();
    void roleDeleted();

public slots:
    // Users
    void fetchUsers();
    void addUser(const Common::Entities::User &user);
    void editUser(const Common::Entities::User &user);
    void deleteUser(const QString &id);

    // Roles
    void fetchRoles();
    void createRole(const Common::Entities::Role &role);
    void editRole(const Common::Entities::Role &role);
    void deleteRole(const QString &id);

private slots: // -- ApiManager signal handlers ---
    void onUserAdded();
    void onUserUpdated();
    void onUserDeleted();
    void onUsersList(const std::vector<Common::Entities::User> &users);

    void onRoleCreated();
    void onRoleEdited();
    void onRoleDeleted();
    void onRolesList(const std::vector<Common::Entities::Role> &roles);

private:
    ApiManager &m_apiManager;

    QVector<Common::Entities::User> m_users;
    QVector<Common::Entities::Role> m_roles;
};
