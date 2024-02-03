#pragma once

#include <memory>

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QByteArray>

class Dataset;
class IDataSerializer;

/**
 * @class NetworkService
 * @brief Manages network operations using Qt's network capabilities.
 *
 * This class handles all network operations and data serialization
 * using a provided IDataSerializer implementation.
 */
class NetworkService : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a NetworkService object.
     * @param parent Parent QObject, typically nullptr.
     */
    explicit NetworkService(QObject *parent = nullptr);

    /**
     * @brief Sends a network request to a specific endpoint.
     * @param endpoint The specific API endpoint for the request.
     * @param operation Type of network operation (GET, POST, etc.).
     * @param dataset Data to be sent in the request.
     */
    void sendRequest(
        const QString &endpoint,
        QNetworkAccessManager::Operation operation = QNetworkAccessManager::GetOperation,
        const Dataset &dataset = Dataset());

    /**
     * @brief Updates the API base URL used for network requests.
     * @param apiUrl The new API base URL.
     */
    void updateApiUrl(const QString &apiUrl);

    /**
     * @brief Updates serializer
     * @param unique unique ptr to IDataSerializer object
     */
    void updateSerializer(std::unique_ptr<IDataSerializer> serializer);

signals:
    /**
     * @brief Signal emitted with deserialized response data.
     * @param dataset deserialized response data as a QString.
     */
    void responseReceived(const Dataset &dataset);

private slots:
    /**
     * @brief Slot to handle network responses.
     * @param reply Network reply object from the request.
     */
    void onNetworkReply(QNetworkReply *reply);

private:
    std::unique_ptr<IDataSerializer> m_serializer; ///< Object for handling serialization.
    QNetworkAccessManager *m_manager;              ///< Manager for network requests.
    QString m_apiUrl;
};
