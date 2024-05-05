#pragma once

#include "Ui/Dialogs/DialogManager.hpp"

#include "mocks/LoginDialogMock.hpp"
#include "mocks/RegistrationDialogMock.hpp"
#include "mocks/QMessageBoxMock.hpp"

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