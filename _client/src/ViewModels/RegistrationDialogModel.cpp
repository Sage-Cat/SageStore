#include "RegistrationDialogModel.hpp"
#include "SpdlogWrapper.hpp"

RegistrationDialogModel::RegistrationDialogModel(QObject *parent)
    : QObject(parent)
{
    SPDLOG_TRACE("RegistrationDialogModel::RegistrationDialogModel");
}

void RegistrationDialogModel::attemptRegistration(const QString &username, const QString &password, const QString &confirmPassword)
{
    SPDLOG_TRACE("RegistrationDialogModel::attemptRegistration | Username: {} | Password: {} ", username.toStdString(), password.toStdString());

    if (password != confirmPassword)
    {
        emit registrationFailed("Password and confirmation do not match.");
        return;
    }

    emit requestRegistration(username, password);
}
