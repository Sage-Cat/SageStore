#pragma once

#include <memory>

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QByteArray>

#include "DataTypes.hpp"
#include "IDataSerializer.hpp"

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
    explicit NetworkService(QObject *parent = nullptr);
    virtual void sendRequest(
        QString endpoint,
        Method method = Method::GET,
        const Dataset &dataset = Dataset(),
        const QString &resource_id = "");
    void setApiUrl(const QString &apiUrl);
    void setSerializer(std::unique_ptr<IDataSerializer> serializer);

signals:
    void responseReceived(const QString &endpoint, Method method, const Dataset &dataset);

private slots:
    void onNetworkReply(const QString &endpoint, Method method, QNetworkReply *reply);

private:
    std::unique_ptr<IDataSerializer> m_serializer;
    QNetworkAccessManager *m_manager;
    QString m_apiUrl;
};
