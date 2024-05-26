#pragma once

#include <QObject>
#include <QVector>

#include "Ui/DisplayData/Entities.hpp"
#include "Ui/ViewModels/BaseViewModel.hpp"

#include "common/Entities/Role.hpp"
#include "common/Entities/User.hpp"

class UsersManagementModel;

class UsersManagementViewModel : public BaseViewModel {
    Q_OBJECT
public:
    explicit UsersManagementViewModel(UsersManagementModel &model, QObject *parent = nullptr);

    QVector<DisplayData::User> users() const;
    QVector<DisplayData::Role> roles() const;

signals:
    void usersChanged();
    void rolesChanged();
    void errorOccurred(const QString &errorMessage);

public slots:
    void fetchUsers();
    void addUser(const DisplayData::User &user);
    void editUser(const DisplayData::User &user);
    void deleteUser(const QString &id);

    void fetchRoles();
    void createRole(const DisplayData::Role &role);
    void editRole(const DisplayData::Role &role);
    void deleteRole(const QString &id);

private slots:
    void onUsersChanged();
    void onRolesChanged();

private:
    DisplayData::User convertToDisplayUser(const Common::Entities::User &commonUser) const;
    Common::Entities::User convertToCommonUser(const DisplayData::User &displayUser) const;

    QString getRoleNameById(const std::string &roleId) const;

private:
    UsersManagementModel &m_model;
    QVector<DisplayData::User> m_users;
    QVector<DisplayData::Role> m_roles;
};
