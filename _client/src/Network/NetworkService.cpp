#include "NetworkService.hpp"

#include "common/SpdlogConfig.hpp"

std::string to_string(Method method)
{
    switch (method) {
    case Method::GET:
        return "GET";
    case Method::POST:
        return "POST";
    case Method::PUT:
        return "PUT";
    case Method::DEL:
        return "DEL";
    default:
        return "UNKNOWN";
    }
}

NetworkService::NetworkService(const ServerConfig &serverConfig,
                               std::unique_ptr<IDataSerializer> serializer)
    : m_serializer(std::move(serializer))
{
    SPDLOG_TRACE("NetworkService::NetworkService");

    m_serverUrl.setScheme(QString::fromStdString(serverConfig.scheme));
    m_serverUrl.setHost(QString::fromStdString(serverConfig.address));
    m_serverUrl.setPort(serverConfig.port);
}

void NetworkService::init()
{
    SPDLOG_TRACE("NetworkService::init");
    m_manager = new QNetworkAccessManager(this);

    SPDLOG_TRACE("NetworkService | emit successful connetction to server");
    emit connected();
}

void NetworkService::sendRequest(std::string endpoint, Method method, const Dataset &dataset,
                                 const std::string &resource_id)
{
    SPDLOG_TRACE("NetworkService::sendRequest | endpoint = {} | method = {} | recource_id = {}",
                 endpoint, to_string(method), resource_id);

    QNetworkReply *reply = nullptr;

    if (!m_serializer) {
        SPDLOG_ERROR("NetworkService::sendRequest serializer was not set");
        return;
    }

    const auto endpointSuffix = endpoint + (resource_id == "" ? "" : +"/" + resource_id);
    m_serverUrl.setPath(QString::fromStdString(endpointSuffix));

    QNetworkRequest request(m_serverUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::ManualRedirectPolicy);

    const auto serializedData = QByteArray::fromStdString(m_serializer->serialize(dataset));
    SPDLOG_DEBUG("NetworkService::onNetworkReply | CLIENT sent data: {}",
                 serializedData.toStdString());

    switch (method) {
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

    if (reply) {
        connect(reply, &QNetworkReply::finished, this,
                [this, endpoint = std::move(endpoint), method, reply] {
                    this->onNetworkReply(endpoint, method, reply);
                });
    }
}

void NetworkService::onNetworkReply(const std::string &endpoint, Method method,
                                    QNetworkReply *reply)
{
    SPDLOG_TRACE("NetworkService::onNetworkReply");

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        SPDLOG_DEBUG("NetworkService::onNetworkReply | CLIENT received data: {}",
                     responseData.toStdString());
        Dataset dataset = m_serializer->deserialize(responseData.toStdString());
        emit responseReceived(endpoint, method, dataset);
    } else {
        SPDLOG_ERROR("Network request error: {}", reply->errorString().toStdString());
    }
    reply->deleteLater();
}
