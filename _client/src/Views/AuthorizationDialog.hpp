#pragma once

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include "BaseDialog.hpp"

/**
 * @class AuthorizationDialog
 * @brief Represents the authorization dialog in the application.
 *
 * This class manages the UI elements for user authorization, including
 * fields for username and password, and buttons for login and registration.
 */
class AuthorizationDialog : public BaseDialog
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Authorization Dialog object.
     * @param parent The parent widget of this dialog, nullptr if it has no parent.
     */
    explicit AuthorizationDialog(BaseDialog *parent = nullptr);

signals:
    /**
     * @brief Signal emitted when the user attempts to log in.
     * @param username The user's username.
     * @param password The user's password.
     */
    void loginAttempted(const QString &username, const QString &password);

    /**
     * @brief Signal emitted when the user requests to switch to the registration view.
     */
    void registrationRequested();

public slots:
    /**
     * @brief Slot for case with successful login attempt
     */
    void onLoginSuccess();

    /**
     * @brief Slot for case with failed login attempt
     */
    void onLoginFailure();

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
     * @brief Slot to handle the login button click event.
     */
    void onLoginClicked();

    /**
     * @brief Slot to handle the register link activation event.
     */
    void onRegisterLinkClicked();

private:
    QLineEdit *m_usernameField; ///< Line edit for the username.
    QLineEdit *m_passwordField; ///< Line edit for the password.
    QPushButton *m_loginButton; ///< Button for submitting login credentials.
    QLabel *m_registerLink;     ///< Link to the registration dialog.
};
