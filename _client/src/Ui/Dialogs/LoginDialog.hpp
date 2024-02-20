#pragma once

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include "Utils.hpp"

#include "BaseDialog.hpp"

/**
 * @class LoginDialog
 * @brief Represents the authorization dialog in the application.
 */
class LoginDialog : public BaseDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(BaseDialog *parent = nullptr);

signals:
    void loginAttempted(const QString &username, const QString &password);
    void registrationRequested();

private:
    void initFields();
    void initButtonsAndLinks();
    void setupLayout();
    void setupConnections();

private slots:
    void onLoginClicked();
    void onRegisterLinkClicked();

private:
    QLineEdit *m_usernameField;
    QLineEdit *m_passwordField;
    QPushButton *m_loginButton;
    QLabel *m_registerLink;
};