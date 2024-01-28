#include "ApiClient.hpp"

#include "NetworkService.hpp"
#include "ApiConstants.hpp"

#include "SpdlogConfig.hpp"

ApiClient::ApiClient(NetworkService *networkService, QObject *parent)
    : QObject(parent), m_networkService(networkService)
{
    SPDLOG_TRACE("ApiClient::ApiClient");
    setupHandlers();
}

void ApiClient::loginUser(const QString &username, const QString &password)
{
    SPDLOG_TRACE(
        "ApiClient::ApiClient username={}, password={}",
        username.toStdString(),
        password.toStdString());

    Dataset dataset;
    dataset["username"] = {username};
    dataset["password"] = {password};
    m_networkService->sendRequest(
        Api::Endpoints::LOGIN,
        QNetworkAccessManager::Operation::PostOperation,
        dataset);
}

void ApiClient::registerUser(const QString &username, const QString &password)
{
    SPDLOG_TRACE("ApiClient::registerUser");

    Dataset dataset;
    dataset["username"] = {username};
    dataset["password"] = {password};

    m_networkService->sendRequest(
        Api::Endpoints::REGISTER,
        QNetworkAccessManager::Operation::PostOperation,
        dataset);
}

void ApiClient::setupHandlers()
{
    SPDLOG_TRACE("ApiClient::setupHandlers");
    m_responseHandlers[Api::Endpoints::LOGIN] = [this](const Dataset &dataset)
    { handleLoginResponse(dataset); };
    m_responseHandlers[Api::Endpoints::REGISTER] = [this](const Dataset &dataset)
    { handleRegistrationResponse(dataset); };
}

void ApiClient::handleResponse(const Dataset &dataset)
{
    const QString endpoint = dataset[Api::Params::ENDPOINT].front();
    SPDLOG_TRACE("ApiClient::handleResponse for endpoint={}", endpoint.toStdString());

    auto handler = m_responseHandlers.find(endpoint);
    if (handler != m_responseHandlers.end())
    {
        handler.value()(dataset);
    }
    else
    {
        SPDLOG_ERROR("ApiClient::handleResponse - couldn't find handler for endpoint");
    }
}

void ApiClient::handleLoginResponse(const Dataset &dataset)
{
    SPDLOG_TRACE("ApiClient::handleLoginResponse");
    m_currentUserToken = dataset[Api::Params::TOKEN].front();
    if (!m_currentUserToken.isEmpty())
        emit loginSuccess();
    else
        emit loginFailed("ApiClient::handleLoginResponse got empty user token");
}

void ApiClient::handleRegistrationResponse(const Dataset &dataset)
{
    SPDLOG_TRACE("ApiClient::handleLoginResponse");
    const QString error_msg = dataset[Api::Params::ERROR].front();
    if (error_msg.isEmpty())
        emit registerSuccess();
    else
        emit registerFailed("Server: " + error_msg);
}
