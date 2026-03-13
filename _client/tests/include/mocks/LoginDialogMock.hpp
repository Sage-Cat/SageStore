#pragma once

#include "Ui/Dialogs/LoginDialog.hpp"

class LoginDialogMock : public LoginDialog {
public:
    void setAutoSubmit(bool autoSubmit) { m_autoSubmit = autoSubmit; }

protected:
    void showWithPresetData(const QString &username, const QString &password) override
    {
        LoginDialog::showWithPresetData(username, password);
        if (m_autoSubmit) {
            onLoginClicked();
        }
    }

private:
    bool m_autoSubmit{true};
};
