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
class DialogManager final : public QObject
{
    Q_OBJECT

protected:
public:
    DialogManager(ApiManager &apiManager);
    ~DialogManager();

    void showErrorMessageBox(const QString &message);

    // custom dialogs
    void showLoginDialog();
    void showRegistrationDialog();

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