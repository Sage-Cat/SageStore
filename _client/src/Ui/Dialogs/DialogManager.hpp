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
    virtual ~DialogManager();

    void init();

    // show dialogs
    void showLoginDialog();
    void showRegistrationDialog();
    void showErrorDialog(const QString &message);

    QMessageBox *messageDialog() const;

protected:
    virtual void initDialogs();
    void setupApiConnections();
    void setupDialogsConnections();

    void showMessage(const QString &title, const QString &message, QMessageBox::Icon type);

private:
    ApiManager &m_apiManager;

protected:
    // dialogs
    LoginDialog *m_loginDialog;
    RegistrationDialog *m_registrationDialog;
    QMessageBox *m_messageDialog;
};