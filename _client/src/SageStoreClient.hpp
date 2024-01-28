#pragma once

#include <QApplication>

#include <memory>

// --- Networking
class NetworkService;
class ConfigManager;
class ApiClient;

// --- UI
class UiManager;

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
     * @brief Gets default font from UiManager and sets it to QApplication
     */
    void applyAppFont();

private slots:
    /**
     * @brief Handle the case when config failed to fetch Url for NetworkService
     */
    void onConfigFetchFailed();

private:
    QApplication &m_app;

    // --- Networking ---
    ConfigManager *m_configManager;

    NetworkService *m_networkService;
    QThread *m_networkServiceThread;

    ApiClient *m_apiManager;

    // --- UI ---
    UiManager *m_uiManager;
};