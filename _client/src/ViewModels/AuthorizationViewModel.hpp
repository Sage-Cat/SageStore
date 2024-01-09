#pragma once

#include <QObject>
#include <QString>

/**
 * @class AuthorizationViewModel
 * @brief Manages the logic for user authorization.
 *
 * This class handles user login attempts, including interactions with network services
 * and emitting signals based on the outcome of the login process.
 */
class AuthorizationViewModel : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Authorization ViewModel object.
     * @param parent The parent QObject, nullptr if it has no parent.
     */
    explicit AuthorizationViewModel(QObject *parent = nullptr);

public slots:
    /**
     * @brief Attempts to log in with the provided username and password.
     * @param username The user's username.
     * @param password The user's password.
     */
    void attemptLogin(const QString &username, const QString &password);

signals:
    /**
     * @brief Emitted when the login is successful.
     */
    void loginSuccessful();

    /**
     * @brief Emitted when the login fails.
     * @param error A description of the error.
     */
    void loginFailed(const QString &error);
};
