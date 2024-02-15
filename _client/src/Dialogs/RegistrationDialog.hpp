#pragma once

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include "BaseDialog.hpp"

/**
 * @class RegistrationDialog
 * @brief Represents the registration dialog in the application.
 *
 * This class manages the UI elements for user registration, including
 * input fields for username, password, and confirmation, and a button for registration.
 */
class RegistrationDialog : public BaseDialog
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Registration Dialog object
     * @param parent The parent widget of this dialog, nullptr if it has no parent.
     */
    explicit RegistrationDialog(BaseDialog *parent = nullptr);

signals:
    /**
     * @brief Signal emitted when the user attempts to register.
     * @param username The user's chosen username.
     * @param password The user's chosen password.
     */
    void registrationAttempted(const QString &username, const QString &password);

    /**
     * @brief Signal emitted when user attempts to register not appropriate data.
     */
    void requestErrorMessageBox(const QString &errorMessage);

    /**
     * @brief Signal emitted when the user requests to switch to the login view.
     */
    void loginRequested();

public slots:
    /**
     * @brief Slot for case with successful registration attempt
     */
    void onRegistrationSuccess();

    /**
     * @brief Slot for case with failed registration attempt
     */
    void onRegistrationFailure();

private:
    /**
     * @brief Initializes the input fields.
     */
    void initFields();

    /**
     * @brief Initializes the buttons and links.
     */
    void initButtonsAndLinks();

    /**
     * @brief Sets up the layout of the widget.
     */
    void setupLayout();

    /**
     * @brief Establishes signal-slot connections.
     */
    void setupConnections();

private slots:
    /**
     * @brief Slot to handle the registration button click event.
     */
    void onRegisterClicked();

    /**
     * @brief Slot to handle the login link activation event.
     */
    void onLoginLinkClicked();

private:
    QLineEdit *m_usernameField;        ///< Line edit for the username.
    QLineEdit *m_passwordField;        ///< Line edit for the password.
    QLineEdit *m_confirmPasswordField; ///< Line edit for confirming the password.
    QPushButton *m_registerButton;     ///< Button for submitting registration details.
    QLabel *m_loginLink;               ///< Link to the login dialog.
};