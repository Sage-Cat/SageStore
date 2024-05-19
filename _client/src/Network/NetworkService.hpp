#pragma once

#include <memory>

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QUrl>

#include "common/DataTypes.hpp"
#include "common/Network/IDataSerializer.hpp"

enum class Method {
    GET  = QNetworkAccessManager::Operation::GetOperation,
    POST = QNetworkAccessManager::Operation::PostOperation,
    PUT  = QNetworkAccessManager::Operation::PutOperation,
    DEL  = QNetworkAccessManager::Operation::DeleteOperation
};

/**
 * @class NetworkService
 * @brief Manages network operations using Qt's network capabilities.
 */
class NetworkService : public QObject {
    Q_OBJECT

public:
    typedef struct {
        std::string scheme;
        std::string address;
        int port;
    } ServerConfig;

    explicit NetworkService(const ServerConfig &serverConfig,
                            std::unique_ptr<IDataSerializer> serializer);

    virtual void sendRequest(std::string endpoint, Method method = Method::GET,
                             const Dataset &dataset         = Dataset(),
                             const std::string &resource_id = "");

signals:
    void responseReceived(const std::string &endpoint, Method method, const Dataset &dataset);

private slots:
    void onNetworkReply(const std::string &endpoint, Method method, QNetworkReply *reply);

private:
    QNetworkAccessManager *m_manager;
    QUrl m_serverUrl;
    std::unique_ptr<IDataSerializer> m_serializer;
};
