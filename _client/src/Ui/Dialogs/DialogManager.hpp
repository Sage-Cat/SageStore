#pragma once

#include <QString>
#include <QObject>
#include <QMessageBox>

#include "LoginDialog.hpp"
#include "RegistrationDialog.hpp"
#include "Network/ApiManager.hpp"

/**
 * @class DialogManager
 * This class manages all dialogs in client: initializes them
 * and control their behavior
 */
class DialogManager : public QObject
{
    Q_OBJECT

public:
    DialogManager(ApiManager &apiManager);
    ~DialogManager();

    virtual void showErrorMessageBox(const QString &message);

    // custom dialogs
    virtual void showLoginDialog();
    virtual void showRegistrationDialog();

private:
    void initDialogs();
    void setupApiConnections();
    void setupDialogsConnections();

private:
    ApiManager &m_apiManager;

    // dialogs
    LoginDialog *m_loginDialog;
    RegistrationDialog *m_registrationDialog;
};