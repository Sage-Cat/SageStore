#include "ApiManager.hpp"

#include "ApiConstants.hpp"
#include "NetworkService.hpp"
#include "JsonSerializer.hpp"

#include "SpdlogConfig.hpp"

ApiManager::ApiManager(QObject *parent)
    : QObject(parent)
{
    SPDLOG_TRACE("ApiManager::ApiManager");

    setupNetworkService();
    setupHandlers();
}

void ApiManager::setupNetworkService()
{
    m_networkService = new NetworkService(this);
    m_networkService->updateApiUrl(Api::API_URL);
    m_networkService->updateSerializer(std::make_unique<JsonSerializer>());
}

void ApiManager::loginUser(const QString &username, const QString &password)
{
    SPDLOG_TRACE(
        "ApiManager::ApiManager username={}, password={}",
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

void ApiManager::registerUser(const QString &username, const QString &password)
{
    SPDLOG_TRACE("ApiManager::registerUser");

    Dataset dataset;
    dataset["username"] = {username};
    dataset["password"] = {password};

    m_networkService->sendRequest(
        Api::Endpoints::REGISTER,
        QNetworkAccessManager::Operation::PostOperation,
        dataset);
}

void ApiManager::setupHandlers()
{
    SPDLOG_TRACE("ApiManager::setupHandlers");
    m_responseHandlers[Api::Endpoints::LOGIN] = [this](const Dataset &dataset)
    { handleLoginResponse(dataset); };
    m_responseHandlers[Api::Endpoints::REGISTER] = [this](const Dataset &dataset)
    { handleRegistrationResponse(dataset); };
}

void ApiManager::handleResponse(const Dataset &dataset)
{
    const QString endpoint = dataset[Api::Params::ENDPOINT].front();
    SPDLOG_TRACE("ApiManager::handleResponse for endpoint={}", endpoint.toStdString());

    auto handler = m_responseHandlers.find(endpoint);
    if (handler != m_responseHandlers.end())
    {
        handler.value()(dataset);
    }
    else
    {
        SPDLOG_ERROR("ApiManager::handleResponse - couldn't find handler for endpoint");
    }
}

void ApiManager::handleLoginResponse(const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleLoginResponse");
    m_currentUserToken = dataset[Api::Params::TOKEN].front();
    if (!m_currentUserToken.isEmpty())
        emit loginSuccess();
    else
        emit loginFailed("ApiManager::handleLoginResponse got empty user token");
}

void ApiManager::handleRegistrationResponse(const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleLoginResponse");
    const QString error_msg = dataset[Api::Params::ERR].front();
    if (error_msg.isEmpty())
        emit registerSuccess();
    else
        emit registerFailed("Server: " + error_msg);
}
