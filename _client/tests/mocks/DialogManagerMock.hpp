#pragma once

#include "Ui/Dialogs/DialogManager.hpp"

class ApiManager;

class DialogManagerMock : public DialogManager
{
public:
    DialogManagerMock(ApiManager &apiManager) : DialogManager(apiManager)
    {
    }

    void showErrorMessageBox(const QString &message) override
    {
    }

    void showLoginDialog() override
    {
    }

    void showRegistrationDialog() override
    {
    }
};