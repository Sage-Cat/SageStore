#pragma once

#include "Utils.hpp"
#include <QCryptographicHash>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "BaseDialog.hpp"

/**
 * @class LoginDialog
 * @brief Represents the authorization dialog in the application.
 */
class LoginDialog : public BaseDialog {
    Q_OBJECT

public:
    explicit LoginDialog(BaseDialog *parent = nullptr);
    ~LoginDialog();

    virtual void showWithPresetData(const QString &username, const QString &password);

signals:
    void loginAttempted(const QString &username, const QString &password);
    void registrationRequested();

private:
    void initFields();
    void initButtonsAndLinks();
    void setupLayout();
    void setupConnections();

protected slots:
    void onLoginClicked();
    void onRegisterLinkClicked();

private:
    QLineEdit *m_usernameField;
    QLineEdit *m_passwordField;
    QPushButton *m_loginButton;
    QLabel *m_registerLink;

    QVBoxLayout *labelLayout;
    QVBoxLayout *lineLayout;
    QHBoxLayout *inputLayout;
};
