#include "ApiManager.hpp"

#include "ApiConstants.hpp"
#include "NetworkService.hpp"
#include "JsonSerializer.hpp"

#include "DatasetCommon.hpp"
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
    connect(m_networkService, &NetworkService::responseReceived, this, &ApiManager::handleResponse);
}

void ApiManager::loginUser(const QString &username, const QString &password)
{
    SPDLOG_TRACE(
        "ApiManager::loginUser username={}, password={}",
        username.toStdString(),
        password.toStdString());

    Dataset dataset;
    dataset["username"] = {username};
    dataset["password"] = {password};
    m_networkService->sendRequest(
        Api::Endpoints::Users::LOGIN,
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
        Api::Endpoints::Users::REGISTER,
        QNetworkAccessManager::Operation::PostOperation,
        dataset);
}

void ApiManager::setupHandlers()
{
    SPDLOG_TRACE("ApiManager::setupHandlers");
    m_responseHandlers[Api::Endpoints::Users::LOGIN] = [this](const Dataset &dataset)
    { handleLoginResponse(dataset); };
    m_responseHandlers[Api::Endpoints::Users::REGISTER] = [this](const Dataset &dataset)
    { handleRegistrationResponse(dataset); };
}

void ApiManager::handleResponse(const QString &endpoint, const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleResponse for endpoint={}", endpoint.toStdString());

    auto handler = m_responseHandlers.find(endpoint);
    if (handler != m_responseHandlers.end())
    {
        if (!dataset.contains(Keys::_ERROR))
            handler.value()(dataset);
        else
            handleError(dataset[Keys::_ERROR].front());
    }
    else
    {
        SPDLOG_ERROR("ApiManager::handleResponse | Can't find handler for endpoint={}", endpoint.toStdString());
    }
}

void ApiManager::handleError(const QString &errorMessage)
{
    SPDLOG_TRACE("ApiManager::handleError - {}", errorMessage.toStdString());
    emit errorOccurred(errorMessage);
}

void ApiManager::handleLoginResponse(const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleLoginResponse");
    const auto id = dataset[Keys::User::ID].front();
    const auto roleId = dataset[Keys::User::ROLE_ID].front();
    if (!id.isEmpty() && !roleId.isEmpty())
        emit loginSuccess(id, roleId);
    else
        emit loginFailed("ApiManager::handleLoginResponse got empty id or roleId from server");
}

void ApiManager::handleRegistrationResponse(const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleLoginResponse");
    const auto error = dataset[Keys::_ERROR].front();
    if (error.isEmpty())
        emit registerSuccess();
    else
        emit registerFailed(error);
}
