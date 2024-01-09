#include "RegistrationViewModel.hpp"

RegistrationViewModel::RegistrationViewModel(QObject *parent)
    : QObject(parent)
{
}

void RegistrationViewModel::attemptRegistration(const QString &username, const QString &password, const QString &confirmPassword)
{
    if (password != confirmPassword)
    {
        emit registrationFailed("Password and confirmation do not match.");
        return;
    }

    emit requestRegistration(username, password);
}
