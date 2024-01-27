#include "RegistrationModel.hpp"

#include "SpdlogConfig.hpp"

RegistrationModel::RegistrationModel(QObject *parent)
    : QObject(parent), m_username(""), m_password("")
{
    SPDLOG_TRACE("RegistrationModel::RegistrationModel");
}

void RegistrationModel::setRegistrationDetails(const QString &username, const QString &password)
{
    SPDLOG_TRACE("RegistrationModel::setRegistrationDetails | username = {} | password = {}", username.toStdString(), password.toStdString());
    m_username = username;
    m_password = password;
}

void RegistrationModel::onRegistrationRequested(const QString &username, const QString &password)
{
    SPDLOG_TRACE("RegistrationModel::onRegistrationRequested");
    setRegistrationDetails(username, password);
    performRegistration();
}

void RegistrationModel::performRegistration()
{
    SPDLOG_TRACE("RegistrationModel::performRegistration");
    // TODO: Implement the logic for registering the user.
    // This could involve interacting with a network service.

    // Example:
    // if (registration is successful) {
    //     emit registrationSuccessful();
    // } else {
    //     emit registrationFailed("Registration error");
    // }
}
