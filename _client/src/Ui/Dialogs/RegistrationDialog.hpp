#pragma once

#include "Utils.hpp"
#include <QCryptographicHash>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "BaseDialog.hpp"

/**
 * @class RegistrationDialog
 * @brief Represents the registration dialog in the application.
 */
class RegistrationDialog : public BaseDialog {
    Q_OBJECT

public:
    explicit RegistrationDialog(BaseDialog *parent = nullptr);
    virtual ~RegistrationDialog();

    virtual void showWithPresetData(const QString &username, const QString &password);

    const QString getUsername();
    const QString getPassword();

signals:
    void registrationAttempted(const QString &username, const QString &password);
    void requestErrorMessageBox(const QString &errorMessage);
    void loginRequested();

public slots:
    void onRegistrationSuccess();
    void onRegistrationFailure();

private:
    void initFields();
    void initButtonsAndLinks();
    void setupLayout();
    void setupConnections();

protected slots:
    void onRegisterClicked();
    void onLoginLinkClicked();

private:
    QLineEdit *m_usernameField;
    QLineEdit *m_passwordField;
    QLineEdit *m_confirmPasswordField;
    QPushButton *m_registerButton;
    QLabel *m_loginLink;

    QVBoxLayout *labelLayout;
    QVBoxLayout *lineLayout;
    QHBoxLayout *inputLayout;
};
