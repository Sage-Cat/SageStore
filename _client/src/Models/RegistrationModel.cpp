#include "RegistrationModel.hpp"

RegistrationModel::RegistrationModel(QObject *parent)
    : QObject(parent), m_username(""), m_password("")
{
}

void RegistrationModel::setRegistrationDetails(const QString &username, const QString &password)
{
    m_username = username;
    m_password = password;
}

void RegistrationModel::performRegistration()
{
    // TODO: Implement the logic for registering the user.
    // This could involve interacting with a network service.

    // Example:
    // if (registration is successful) {
    //     emit registrationSuccessful();
    // } else {
    //     emit registrationFailed("Registration error");
    // }
}
