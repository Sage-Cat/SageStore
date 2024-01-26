#pragma once

#include <QApplication>

#include <memory>

// --- Networking
class NetworkService;
class ConfigManager;

// --- UI
class UiManager;

// Models
class AuthorizationModel;
class RegistrationModel;

/**
 * @class SageStoreClient
 * @brief Main class for the Sage Store Client.
 *
 * This class is responsible for initializing and running the Sage Store Client.
 */
class SageStoreClient : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Default constructor for SageStoreClient.
     *
     * Initializes the SageStoreClient object with a reference to a QApplication object
     *
     * @param app A reference to the QApplication object responsible for the application lifecycle.
     */
    SageStoreClient(QApplication &app);

    /**
     * @brief Deleted copy constructor.
     */
    SageStoreClient(const SageStoreClient &) = delete;

    /**
     * @brief Deleted copy assignment operator.
     */
    SageStoreClient &operator=(const SageStoreClient &) = delete;

    /**
     * @brief Destructor for SageStoreClient.
     *
     * Cleans up any resources used by the SageStoreClient object.
     */
    ~SageStoreClient();

private:
    /**
     * @brief Initialize the SageStore client.
     *
     * This function performs all necessary initialization tasks for the client.
     */
    void init();

    /**
     * @brief Initialize UI models.
     *
     * Sets up various models required by the application (according MVVM).
     */
    void initModels();

    /**
     * @brief Setup networkservice - get URL using ConfigManager and setup NetworkService
     */
    void setupNetworkService();

    /**
     * @brief Setup model - viewModel connections according to MVVM
     */
    void setupMVMConnections();

    /**
     * @brief Gets default font from UiManager and sets it to QApplication
     */
    void applyAppFont();

private slots:
    /**
     * @brief Handle the case when config failed to fetch Url for NetworkService
     */
    void onConfigFetchFailed();

private:
    /**
     * @brief Reference to the QApplication object.
     *
     * Holds a reference to the QApplication object to manage application-wide settings.
     */
    QApplication &m_app;

    ConfigManager *m_configManager;
    NetworkService *m_networkService;
    QThread *m_networkServiceThread;

    /**
     * @brief UiManager object initialized on stack.
     *
     * This member is responsible for managing the user interface of the client.
     */
    UiManager *m_uiManager;

    // Models
    AuthorizationModel *m_authorizationModel;
    RegistrationModel *m_registrationModel;
};