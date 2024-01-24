#include "ConfigManager.hpp"

#include "SpdlogWrapper.hpp"

const char *SERVER_CONFIG_URL{"http://localhost:8000/config"};
namespace CONFIG_KEYS
{
    const char *API_URL{"apiUrl"};
    const char *SERIALIZATION_TYPE{"serializationType"};
};

ConfigManager::ConfigManager(std::unique_ptr<IDataSerializer> serializer, QObject *parent)
    : QObject(parent), m_serializer(std::move(serializer)), m_manager(new QNetworkAccessManager(this))
{
    SPDLOG_TRACE("ConfigManager::ConfigManager");

    // Setup the timeout timer
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setInterval(5000); // Set timeout interval (e.g., 5000 ms)
    connect(m_timeoutTimer, &QTimer::timeout, this, &ConfigManager::onTimeout);

    connect(m_manager, &QNetworkAccessManager::finished, this, &ConfigManager::onNetworkReply);
}

void ConfigManager::fetchConfiguration()
{
    SPDLOG_TRACE("ConfigManager::fetchConfiguration");

    QUrl url(SERVER_CONFIG_URL);
    QNetworkRequest request(url);
    m_manager->get(request);
}

void ConfigManager::onNetworkReply(QNetworkReply *reply)
{
    SPDLOG_TRACE("ConfigManager::onNetworkReply");

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        SPDLOG_TRACE("Network Reply: {}", response.toStdString());

        // Deserialize the response into a Dataset
        const Dataset configDataset = m_serializer->deserialize(response);

        if (!configDataset.isEmpty() && !configDataset[CONFIG_KEYS::API_URL].isEmpty())
        {
            m_config.apiUrl = configDataset[CONFIG_KEYS::API_URL].first();
            m_config.serializationType = configDataset[CONFIG_KEYS::SERIALIZATION_TYPE].first();
            emit configurationFetched(m_config);
            SPDLOG_TRACE("Fetched API URL: {}", m_config.apiUrl.toStdString());
            SPDLOG_TRACE("Fetched serialization type: {}", m_config.serializationType.toStdString());
        }
        else
        {
            SPDLOG_ERROR("Invalid or missing data in response");
            emit configurationFetchFailed();
        }
    }
    else
    {
        SPDLOG_ERROR("Network Error: {}", reply->errorString().toStdString());
        emit configurationFetchFailed();
    }

    reply->deleteLater();
}

void ConfigManager::onTimeout()
{
    SPDLOG_WARN("ConfigManager::fetchConfiguration - Timeout occurred");
    emit configurationFetchFailed();
    m_timeoutTimer->stop();
}
