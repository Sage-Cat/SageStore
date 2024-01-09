#pragma once

#include <QObject>
#include <QString>

#include "IModel.hpp"

/**
 * @class RegistrationModel
 * @brief Manages user registration data and logic.
 *
 * This class stores user registration details and handles registration logic.
 * It inherits from QObject for Qt functionalities and IModel for the application's model interface.
 */
class RegistrationModel : public QObject, public IModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a RegistrationModel with a parent object.
     * @param parent The parent QObject, nullptr if it has no parent.
     */
    explicit RegistrationModel(QObject *parent = nullptr);

    /**
     * @brief Sets the user registration details.
     * @param username User's chosen username.
     * @param password User's chosen password.
     */
    void setRegistrationDetails(const QString &username, const QString &password);

signals:
    /**
     * @brief Emitted when registration succeeds.
     */
    void registrationSuccessful();

    /**
     * @brief Emitted when registration fails.
     * @param error Description of the error.
     */
    void registrationFailed(const QString &error);

private:
    /**
     * @brief Performs the actual registration logic.
     */
    void performRegistration();

private:
    QString m_username; ///< Stores the chosen username.
    QString m_password; ///< Stores the chosen password.
};
