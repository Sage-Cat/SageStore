#pragma once

#include <QObject>
#include <QMap>
#include <functional>

#include "NetworkService.hpp"

using ResponseHandler = std::function<void(const Dataset &)>;

/**
 * @class ApiManager
 * @brief Manages API calls for user authentication and registration processes.
 *
 * This class encapsulates the logic for interacting with a network service to perform user login and registration.
 * It uses signals to communicate the outcomes of these operations (success or failure) back to the caller.
 */
class ApiManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for ApiManager.
     *
     * @param parent The parent QObject, defaulting to nullptr if not specified.
     */
    ApiManager(QObject *parent = nullptr);

    /**
     * @brief Initiates a login operation for a user with the provided username and password.
     *
     * @param username The username of the user attempting to log in.
     * @param password The password of the user attempting to log in.
     */
    void loginUser(const QString &username, const QString &password);

    /**
     * @brief Initiates a registration operation for a user with the provided username and password.
     *
     * @param username The username for the new user registration.
     * @param password The password for the new user registration.
     */
    void registerUser(const QString &username, const QString &password);

    /**
     * @brief Initilates a get operation for a user
     *
     */
    void getUsersRoles();

    /**
     * @brief Initilates a new role
     * @param new_role The name of role which add
     */
    void setNewRole(const QString &new_role);

private:
    /**
     * @brief Sets up the network service and prepares it for making API calls.
     */
    void
    setupNetworkService();

    /**
     * @brief Configures response handlers for the different API responses.
     */
    void setupHandlers();

signals:
    /**
     * @brief Emitted when an error occurs during API interaction.
     *
     * @param errorMessage The error message describing the failure.
     */
    void errorOccurred(const QString &errorMessage);

    /**
     * @brief Emitted upon successful login.
     */
    void loginSuccess();

    /**
     * @brief Emitted when login fails.
     *
     * @param errorMessage The error message describing why the login failed.
     */
    void loginFailed(const QString &errorMessage);

    /**
     * @brief Emitted upon successful user registration.
     */
    void registerSuccess();

    /**
     * @brief Emitted when user registration fails.
     *
     * @param errorMessage The error message describing why the registration failed.
     */
    void registerFailed(const QString &errorMessage);

private slots:
    /**
     * @brief Hadles response by calling m_responseHandlers
     *
     * @param dataset The data set received in response to an API call.
     */
    void handleResponse(const Dataset &dataset);

    /**
     * @brief Handles errors that occur during network communication.
     *
     * @param errorMessage The error message received from the network service.
     */
    void handleError(const QString &errorMessage);

private:
    /**
     * @brief Specifically handles responses related to login attempts.
     *
     * @param dataset The data set received in response to a login API call.
     */
    void handleLoginResponse(const Dataset &dataset);

    /**
     * @brief Specifically handles responses related to registration attempts.
     *
     * @param dataset The data set received in response to a registration API call.
     */
    void handleRegistrationResponse(const Dataset &dataset);

private:
    NetworkService *m_networkService;                  ///< Pointer to the network service used for API calls.
    QMap<QString, ResponseHandler> m_responseHandlers; ///< Maps API endpoints to their respective response handlers.
    QString m_currentUserToken;                        ///< Stores the current user's authentication token.
};
