#pragma once

#include <QMessageBox>
#include <QObject>
#include <QString>

#include "LoginDialog.hpp"
#include "Network/ApiManager.hpp"
#include "RegistrationDialog.hpp"

/**
 * @class DialogManager
 * This class manages all dialogs in client: initializes them
 * and control their behavior
 */
class DialogManager : public QObject {
    Q_OBJECT

public:
    DialogManager(ApiManager &apiManager, QObject *parent = nullptr);
    virtual ~DialogManager();

    void init();

    QMessageBox *messageDialog() const;

public slots:
    // show dialogs
    virtual void showLoginDialog();
    virtual void showRegistrationDialog();
    virtual void showErrorDialog(const QString &message);

protected:
    virtual void setupDialogs();
    void setupApiConnections();

    void showMessage(const QString &title, const QString &message, QMessageBox::Icon type);

private:
    ApiManager &m_apiManager;

private slots:
    void onLoginSuccess();
    void onUserAdded();

protected:
    // dialogs
    LoginDialog *m_loginDialog;
    RegistrationDialog *m_registrationDialog;
    QMessageBox *m_messageDialog;
};