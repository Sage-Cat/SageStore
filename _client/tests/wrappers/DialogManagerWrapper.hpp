#pragma once

#include "Ui/Dialogs/DialogManager.hpp"

#include "LoginDialogMock.hpp"
#include "RegistrationDialogMock.hpp"
#include "QMessageBoxMock.hpp"

class ApiManager;

class DialogManagerWrapper : public DialogManager
{
public:
    DialogManagerWrapper(ApiManager &apiManager) : DialogManager(apiManager)
    {
    }

    void initDialogs() override
    {
        m_loginDialog = new LoginDialogMock;
        m_registrationDialog = new RegistrationDialogMock;
        m_messageDialog = new QMessageBoxMock;
    }
};