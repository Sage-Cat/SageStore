#include "NetworkService.hpp"

#include "SpdlogConfig.hpp"

NetworkService::NetworkService(QObject *parent)
    : QObject(parent), m_manager(new QNetworkAccessManager(this))
{
    SPDLOG_TRACE("NetworkService::NetworkService");

    connect(m_manager, &QNetworkAccessManager::finished, this, &NetworkService::onNetworkReply);
}

void NetworkService::sendRequest(const QString &endpoint, QNetworkAccessManager::Operation operation, const Dataset &dataset)
{
    SPDLOG_TRACE("NetworkService::sendRequest");

    if (!m_serializer)
    {
        SPDLOG_ERROR("NetworkService::sendRequest serializer was not set");
        return;
    }

    QUrl fullUrl(m_apiUrl + endpoint); // Append the endpoint to the base URL
    QNetworkRequest request(fullUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    switch (operation)
    {
    case QNetworkAccessManager::GetOperation:
        m_manager->get(request);
        break;
    case QNetworkAccessManager::PostOperation:
        m_manager->post(request, m_serializer->serialize(dataset));
        break;
    case QNetworkAccessManager::PutOperation:     // fall down
    case QNetworkAccessManager::DeleteOperation:  // fall down
    case QNetworkAccessManager::CustomOperation:  // fall down
    case QNetworkAccessManager::UnknownOperation: // fall down
    default:
        SPDLOG_ERROR("NetworkService::sendRequest unexpected switch(operation) occured.");
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
