#pragma once

#include <QObject>
#include <QString>

#include "IDialogModel.hpp"

/**
 * @class RegistrationDialogModel
 * @brief Manages the logic for user registration.
 *
 * Handles user registration attempts and emits signals based on the outcome.
 */
class RegistrationDialogModel : public QObject, public IDialogModel
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Registration DialogModel object.
     * @param parent The parent QObject, nullptr if it has no parent.
     */
    explicit RegistrationDialogModel(QObject *parent = nullptr);

signals:
    /**
     * @brief Signal to request user registration.
     * @param username The user's chosen username.
     * @param password The user's chosen password.
     */
    void requestRegistration(const QString &username, const QString &password);

    /**
     * @brief Emitted when the registration is successful.
     */
    void registrationSuccessful();

    /**
     * @brief Emitted when the registration fails.
     * @param error A description of the error.
     */
    void registrationFailed(const QString &error);

public slots:
    /**
     * @brief Attempts to register a new user with the provided details.
     * @param username The user's chosen username.
     * @param password The user's chosen password.
     * @param confirmPassword Password confirmation.
     */
    void attemptRegistration(const QString &username, const QString &password, const QString &confirmPassword);
};
