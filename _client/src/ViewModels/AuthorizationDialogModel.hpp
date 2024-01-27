#pragma once

#include <QObject>
#include <QString>

#include "IDialogModel.hpp"

/**
 * @class AuthorizationDialogModel
 * @brief Manages the logic for user authorization.
 *
 * Handles user login attempts and emits signals based on the outcome.
 */
class AuthorizationDialogModel : public QObject, public IDialogModel
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Authorization DialogModel object.
     * @param parent The parent QObject, nullptr if it has no parent.
     */
    explicit AuthorizationDialogModel(QObject *parent = nullptr);

signals:
    /**
     * @brief Signal to request user authentication.
     * @param username The user's username.
     * @param password The user's password.
     */
    void requestAuthentication(const QString &username, const QString &password);

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
