#include "NetworkService.hpp"
#include "SpdlogConfig.hpp"

NetworkService::NetworkService(QObject *parent)
    : QObject(parent), m_manager(new QNetworkAccessManager(this))
{
    SPDLOG_TRACE("NetworkService::NetworkService");
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
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);

    QNetworkReply *reply = nullptr;
    switch (operation)
    {
    case QNetworkAccessManager::GetOperation:
        reply = m_manager->get(request);
        break;
    case QNetworkAccessManager::PostOperation:
        reply = m_manager->post(request, m_serializer->serialize(dataset));
        break;
    default:
        SPDLOG_ERROR("NetworkService::sendRequest unexpected switch(operation) occurred.");
        return;
    }

    if (reply)
    {
        connect(reply, &QNetworkReply::finished, this, [this, reply]()
                {
                    if (reply->error() == QNetworkReply::NoError)
                    {
                        QByteArray responseData = reply->readAll();
                        Dataset dataset = m_serializer->deserialize(responseData);
                        emit responseReceived(dataset);
                    }
                    else
                    {
                        SPDLOG_ERROR("Network request error: {}", reply->errorString().toStdString());
                    }
                    reply->deleteLater(); // Ensure reply is deleted after processing
                });
    }
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
    else
    {
        // Handle error
        SPDLOG_ERROR("Network request error: {}", reply->errorString().toStdString());
    }
    reply->deleteLater();
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
