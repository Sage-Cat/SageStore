#include "RegistrationViewModel.hpp"
#include "SpdlogWrapper.hpp"

RegistrationViewModel::RegistrationViewModel(QObject *parent)
    : QObject(parent)
{
    SPDLOG_TRACE("RegistrationViewModel::RegistrationViewModel");
}

void RegistrationViewModel::attemptRegistration(const QString &username, const QString &password, const QString &confirmPassword)
{
    SPDLOG_TRACE("RegistrationViewModel::attemptRegistration | Username: {} | Password: {} ", username.toStdString(), password.toStdString());

    if (password != confirmPassword)
    {
        emit registrationFailed("Password and confirmation do not match.");
        return;
    }

    emit requestRegistration(username, password);
}
