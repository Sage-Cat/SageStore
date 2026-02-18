#include "Ui/ViewModels/UsersManagementViewModel.hpp"
#include "Ui/Models/UsersManagementModel.hpp"

#include "common/SpdlogConfig.hpp"

UsersManagementViewModel::UsersManagementViewModel(UsersManagementModel &model, QObject *parent)
    : BaseViewModel(model, parent), m_model(model)
{
    SPDLOG_TRACE("UsersManagementViewModel::UsersManagementViewModel");

    connect(&m_model, &UsersManagementModel::usersChanged, this,
            &UsersManagementViewModel::onUsersChanged);
    connect(&m_model, &UsersManagementModel::rolesChanged, this,
            &UsersManagementViewModel::onRolesChanged);
    connect(&m_model, &UsersManagementModel::errorOccurred, this,
            &UsersManagementViewModel::errorOccurred);
}

QVector<DisplayData::User> UsersManagementViewModel::users() const { return m_users; }
QVector<DisplayData::Role> UsersManagementViewModel::roles() const { return m_roles; }

void UsersManagementViewModel::fetchUsers() { m_model.fetchUsers(); }

void UsersManagementViewModel::addUser(const DisplayData::User &user)
{
    m_model.addUser(convertToCommonUser(user));
}

void UsersManagementViewModel::editUser(const DisplayData::User &user)
{
    m_model.editUser(convertToCommonUser(user));
}

void UsersManagementViewModel::deleteUser(const QString &id) { m_model.deleteUser(id); }

void UsersManagementViewModel::fetchRoles() { m_model.fetchRoles(); }

void UsersManagementViewModel::createRole(const DisplayData::Role &role)
{
    m_model.createRole(role);
}

void UsersManagementViewModel::editRole(const DisplayData::Role &role) { m_model.editRole(role); }

void UsersManagementViewModel::deleteRole(const QString &id) { m_model.deleteRole(id); }

void UsersManagementViewModel::onUsersChanged()
{
    m_users.clear();

    for (const auto &user : m_model.users())
        m_users.emplace_back(convertToDisplayUser(user));

    emit usersChanged();
}

void UsersManagementViewModel::onRolesChanged()
{
    m_roles = m_model.roles();
    emit rolesChanged();
}

DisplayData::User
UsersManagementViewModel::convertToDisplayUser(const Common::Entities::User &commonUser) const
{
    DisplayData::User displayUser;
    displayUser.id       = QString::fromStdString(commonUser.id);
    displayUser.username = QString::fromStdString(commonUser.username);
    displayUser.password = {};
    displayUser.roleId   = QString::fromStdString(commonUser.roleId);
    displayUser.roleName = getRoleNameById(commonUser.roleId);

    return displayUser;
}

Common::Entities::User
UsersManagementViewModel::convertToCommonUser(const DisplayData::User &displayUser) const
{
    Common::Entities::User commonUser;
    commonUser.id       = displayUser.id.toStdString();
    commonUser.username = displayUser.username.toStdString();
    commonUser.password = displayUser.password.toStdString();
    commonUser.roleId   = displayUser.roleId.toStdString();

    return commonUser;
}

QString UsersManagementViewModel::getRoleNameById(const std::string &roleId) const
{
    for (const auto &role : m_roles) {
        if (role.id == roleId) {
            return QString::fromStdString(role.name);
        }
    }
    return QString();
}
