#include "NetworkService.hpp"

#include "SpdlogWrapper.hpp"

NetworkService::NetworkService(QObject *parent)
    : QObject(parent), m_manager(new QNetworkAccessManager(this))
{
    SPDLOG_TRACE("NetworkService::NetworkService");

    connect(m_manager, &QNetworkAccessManager::finished, this, &NetworkService::onNetworkReply);
}

void NetworkService::sendRequest(const QByteArray &data, QNetworkAccessManager::Operation operation)
{
    SPDLOG_TRACE("NetworkService::sendRequest");
    QNetworkRequest request(m_apiUrl);

    switch (operation)
    {
    case QNetworkAccessManager::GetOperation:
        m_manager->get(request);
        break;
    case QNetworkAccessManager::PostOperation:
        m_manager->post(request, data);
        break;
        // TODO: implement for PUT and DELETE too
    }
}

void NetworkService::updateApiUrl(const QString &apiUrl)
{
    SPDLOG_TRACE("NetworkService::updateApiUrl");
    m_apiUrl = apiUrl;
}

void NetworkService::updateSerializer(std::unique_ptr<IDataSerializer> serializer)
{
    m_serializer = std::move(serializer);
}

void NetworkService::onNetworkReply(QNetworkReply *reply)
{
    SPDLOG_TRACE("NetworkService::onNetworkReply");

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray responseData = reply->readAll();
        Dataset dataset = m_serializer->deserialize(responseData);
        emit responseReceived(dataset);
    }
    reply->deleteLater();
}
