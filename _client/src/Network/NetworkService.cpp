#include "NetworkService.hpp"
#include "SpdlogConfig.hpp"

NetworkService::NetworkService(const ServerConfig &serverConfig, std::unique_ptr<IDataSerializer> serializer)
    : m_manager(new QNetworkAccessManager(this)), m_serializer(std::move(serializer))
{
    SPDLOG_TRACE("NetworkService::NetworkService");

    m_serverUrl.setScheme(serverConfig.scheme);
    m_serverUrl.setHost(serverConfig.address);
    m_serverUrl.setPort(serverConfig.port);
}

void NetworkService::sendRequest(QString endpoint, Method method, const Dataset &dataset, const QString &resource_id)
{
    QString endpointSuffix; 
    QNetworkReply *reply = nullptr;
    
    SPDLOG_TRACE("NetworkService::sendRequest");

    if (!m_serializer)
    {
        SPDLOG_ERROR("NetworkService::sendRequest serializer was not set");
        return;
    }

    if (resource_id == "")
    {
        endpointSuffix = endpoint;
    }
    else
    {
        endpointSuffix = endpoint + "/" + resource_id;
    }
    m_serverUrl.setPath(endpointSuffix);

    QNetworkRequest request(m_serverUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);

    const auto serializedData = m_serializer->serialize(dataset);
    SPDLOG_DEBUG("NetworkService::onNetworkReply | CLIENT sent data: {}", serializedData.toStdString());
    
    switch (method)
    {
    case Method::GET:
        reply = m_manager->get(request);
        break;
    case Method::POST:
        reply = m_manager->post(request, serializedData);
        break;
    case Method::PUT:
        reply = m_manager->put(request, serializedData);
        break;
    case Method::DEL:
        reply = m_manager->deleteResource(request);
        break;
    default:
        SPDLOG_ERROR("NetworkService::sendRequest unexpected switch(operation) occurred.");
        return;
    }

    if (reply)
    {
        connect(reply, &QNetworkReply::finished, this,
                [this, endpoint = std::move(endpoint), method, reply]
                {
                    this->onNetworkReply(endpoint, method, reply);
                });
    }
}

void NetworkService::onNetworkReply(const QString &endpoint, Method method, QNetworkReply *reply)
{
    SPDLOG_TRACE("NetworkService::onNetworkReply");

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray responseData = reply->readAll();
        SPDLOG_DEBUG("NetworkService::onNetworkReply | CLIENT received data: {}", responseData.toStdString());
        Dataset dataset = m_serializer->deserialize(responseData);
        emit responseReceived(endpoint, method, dataset);
    }
    else
    {
        SPDLOG_ERROR("Network request error: {}", reply->errorString().toStdString());
    }
    reply->deleteLater();
}
