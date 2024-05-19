#pragma once

#include "Ui/Dialogs/LoginDialog.hpp"

class LoginDialogMock : public LoginDialog {
    void showWithPresetData(const QString &username, const QString &password) override
    {
        onLoginClicked();
    }
};