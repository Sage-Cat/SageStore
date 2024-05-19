#pragma once

#include "Ui/Dialogs/RegistrationDialog.hpp"

class RegistrationDialogMock : public RegistrationDialog {
    void showWithPresetData(const QString &username, const QString &password) override
    {
        emit registrationAttempted(username, password);
    }
};