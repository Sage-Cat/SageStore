#pragma once

#include "Ui/Dialogs/RegistrationDialog.hpp"

#include "common/Entities/User.hpp"

class RegistrationDialogMock : public RegistrationDialog {
    void showWithPresetData(const QString &username, const QString &password) override
    {
        emit registrationAttempted(Common::Entities::User{
            .username = username.toStdString(), .password = password.toStdString(), .roleId = "0"});
    }
};