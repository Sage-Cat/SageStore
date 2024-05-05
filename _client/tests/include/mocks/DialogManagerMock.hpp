#pragma once

#include "Ui/Dialogs/DialogManager.hpp"

class DialogManagerMock : public DialogManager
{
    Q_OBJECT

public:
    explicit DialogManagerMock(ApiManager &apiManager, QObject *parent = nullptr) : DialogManager(apiManager, parent) {}

    // show dialogs
    void showLoginDialog() override { emit showLogDialog(); }
    void showRegistrationDialog() override { emit showRegDialog(); }
    void showErrorDialog(const QString &message) override { emit showErrorDialog(); }

signals:
    void showLogDialog();
    void showRegDialog();
    void showErrorDialog();

protected:
    void initDialogs() override {}
};