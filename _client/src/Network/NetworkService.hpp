#pragma once

#include <memory>

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QByteArray>

#include "DataTypes.hpp"
#include "Network/IDataSerializer.hpp"

enum class Method
{
    GET = QNetworkAccessManager::Operation::GetOperation,
    POST = QNetworkAccessManager::Operation::PostOperation,
    PUT = QNetworkAccessManager::Operation::PutOperation,
    DEL = QNetworkAccessManager::Operation::DeleteOperation
};

/**
 * @class NetworkService
 * @brief Manages network operations using Qt's network capabilities.
 */
class NetworkService : public QObject
{
    Q_OBJECT

public:
    typedef struct
    {
        QString scheme;
        QString address;
        int     port;
    } ServerConfig;

    explicit NetworkService(const ServerConfig &serverConfig,
                            std::unique_ptr<IDataSerializer> serializer);

    virtual void sendRequest(QString endpoint,
                             Method method = Method::GET,
                             const Dataset &dataset = Dataset(),
                             const QString &resource_id = "");

signals:
    void responseReceived(const QString &endpoint, Method method, const Dataset &dataset);

private slots:
    void onNetworkReply(const QString &endpoint, Method method, QNetworkReply *reply);

private:
    QNetworkAccessManager *m_manager;
    QUrl m_serverUrl;
    std::unique_ptr<IDataSerializer> m_serializer;
};
