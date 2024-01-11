#pragma once

#include <QObject>
#include <QString>

#include "IModel.hpp"

/**
 * @class AuthorizationModel
 * @brief Manages user authorization data and logic.
 *
 * This class stores user credentials and handles authentication logic.
 * It inherits from QObject for Qt functionalities and IModel for the application's model interface.
 */
class AuthorizationModel : public QObject, public IModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructs an AuthorizationModel with a parent object.
     * @param parent The parent QObject, nullptr if it has no parent.
     */
    explicit AuthorizationModel(QObject *parent = nullptr);

    /**
     * @brief Sets the user credentials.
     * @param username User's username.
     * @param password User's password.
     */
    void setCredentials(const QString &username, const QString &password);

signals:
    /**
     * @brief Emitted when authentication succeeds.
     */
    void authenticationSuccessful();

    /**
     * @brief Emitted when authentication fails.
     * @param error Description of the error.
     */
    void authenticationFailed(const QString &error);

public slots:
    /**
     * @brief Called by signal from UI to perform authentication
     */
    void onAuthenticationRequested(const QString &username, const QString &password);

private:
    /**
     * @brief Performs the actual authentication logic.
     */
    void performAuthentication();

private:
    QString m_username; ///< Stores the username.
    QString m_password; ///< Stores the password.
};
