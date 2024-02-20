#include "NetworkService.hpp"
#include "SpdlogConfig.hpp"

NetworkService::NetworkService(QObject *parent)
    : QObject(parent), m_manager(new QNetworkAccessManager(this))
{
    SPDLOG_TRACE("NetworkService::NetworkService");
}

void NetworkService::sendRequest(QString endpoint, QNetworkAccessManager::Operation operation, const Dataset &dataset)
{
    SPDLOG_TRACE("NetworkService::sendRequest");

    if (!m_serializer)
    {
        SPDLOG_ERROR("NetworkService::sendRequest serializer was not set");
        return;
    }

    QUrl fullUrl(m_apiUrl + endpoint);
    QNetworkRequest request(fullUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);

    QNetworkReply *reply = nullptr;
    const auto serializedData = m_serializer->serialize(dataset);
    SPDLOG_DEBUG("NetworkService::onNetworkReply | CLIENT sent data: {}", serializedData.toStdString());
    switch (operation)
    {
    case QNetworkAccessManager::GetOperation:
        reply = m_manager->get(request);
        break;
    case QNetworkAccessManager::PostOperation:
        reply = m_manager->post(request, serializedData);
        break;
    default:
        SPDLOG_ERROR("NetworkService::sendRequest unexpected switch(operation) occurred.");
        return;
    }

    if (reply)
    {
        connect(reply, &QNetworkReply::finished, this, [this, endpoint = std::move(endpoint), reply]()
                { this->onNetworkReply(std::move(endpoint), reply); });
    }
}

void NetworkService::onNetworkReply(QString endpoint, QNetworkReply *reply)
{
    SPDLOG_TRACE("NetworkService::onNetworkReply");

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray responseData = reply->readAll();
        SPDLOG_DEBUG("NetworkService::onNetworkReply | CLIENT received data: {}", responseData.toStdString());
        Dataset dataset = m_serializer->deserialize(responseData);
        emit responseReceived(endpoint, dataset);
    }
    else
    {
        SPDLOG_ERROR("Network request error: {}", reply->errorString().toStdString());
    }
    reply->deleteLater();
}

void NetworkService::setApiUrl(const QString &apiUrl)
{
    SPDLOG_TRACE("NetworkService::setApiUrl");
    m_apiUrl = apiUrl;
}

void NetworkService::setSerializer(std::unique_ptr<IDataSerializer> serializer)
{
    m_serializer = std::move(serializer);
}
