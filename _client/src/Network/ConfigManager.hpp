#pragma once

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

#include "IDataSerializer.hpp"

struct Config
{
    QString apiUrl{"https://localhost:8000/api"}, serializationType{"json"};
};

/**
 * @class ConfigManager
 * @brief Manages fetching and storing configuration data from a remote service.
 *
 * ConfigManager is responsible for retrieving configuration data such as API URLs
 * from a remote configuration service and storing this information for use in the application.
 */
class ConfigManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a ConfigManager object.
     * @param parent The parent QObject, typically nullptr.
     */
    explicit ConfigManager(std::unique_ptr<IDataSerializer> serializer, QObject *parent = nullptr);

    /**
     * @brief Initiates fetching of configuration data from the remote service.
     *
     * This function sends a network request to a predefined configuration URL
     * and processes the response asynchronously.
     */
    void fetchConfiguration();

signals:
    /**
     * @brief Signal emitted when configuration data is successfully fetched.
     * @param apiUrl The fetched API URL from the configuration data.
     */
    void configurationFetched(const Config &config);

    /**
     * @brief Signal emitted when configuration data is failed to fetch.
     */
    void configurationFetchFailed();

private slots:
    /**
     * @brief Slot to handle the response from the configuration service.
     * @param reply The network reply object containing the fetched configuration data.
     */
    void onNetworkReply(QNetworkReply *reply);

    /**
     * @brief Slot to handle timeout fetching case
     */
    void onTimeout();

private:
    std::unique_ptr<IDataSerializer> m_serializer; ///< Object for handling serialization.
    QTimer *m_timeoutTimer;                        ///< Timer for handling timeouts
    QNetworkAccessManager *m_manager;              ///< Manager for handling network requests.
    Config m_config;                               ///< Stores the fetched config.
};
