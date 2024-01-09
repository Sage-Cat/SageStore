#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

/**
 * @class RegistrationView
 * @brief Represents the registration dialog in the application.
 *
 * This class manages the UI elements for user registration, including
 * input fields for username, password, and confirmation, and a button for registration.
 */
class RegistrationView : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Registration View object
     * @param parent The parent widget of this dialog, nullptr if it has no parent.
     */
    explicit RegistrationView(QWidget *parent = nullptr);

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
