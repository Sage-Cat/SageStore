#include "AuthorizationModel.hpp"

#include "SpdlogWrapper.hpp"

AuthorizationModel::AuthorizationModel(QObject *parent)
    : QObject(parent), m_username(""), m_password("")
{
    SPDLOG_TRACE("AuthorizationModel::AuthorizationModel");
}

void AuthorizationModel::setCredentials(const QString &username, const QString &password)
{
    SPDLOG_TRACE("AuthorizationModel::setCredentials | username = {} | password = {}", username.toStdString(), password.toStdString());
    m_username = username;
    m_password = password;
}

void AuthorizationModel::onAuthenticationRequested(const QString &username, const QString &password)
{
    SPDLOG_TRACE("AuthorizationModel::onAuthenticationRequested");
    setCredentials(username, password);
    performAuthentication();
}

void AuthorizationModel::performAuthentication()
{
    SPDLOG_TRACE("AuthorizationModel::performAuthentication");
    // TODO: Implement the logic for authenticating the user.
    // This could involve interacting with a network service.

    // Example:
    // if (authentication is successful) {
    //     emit authenticationSuccessful();
    // } else {
    //     emit authenticationFailed("Invalid credentials");
    // }
}
