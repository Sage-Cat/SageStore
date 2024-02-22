#include "ApiManager.hpp"

#include "Endpoints.hpp"

#include "Network/NetworkService.hpp"

#include "DatasetCommon.hpp"
#include "SpdlogConfig.hpp"

ApiManager::ApiManager(NetworkService &networkService)
    : m_networkService(networkService)
{
    SPDLOG_TRACE("ApiManager::ApiManager");

    // setup network service
    connect(&m_networkService, &NetworkService::responseReceived,
            this, &ApiManager::handleResponse);

    // setup handlers for responses
    setupHandlers();
}

ApiManager::~ApiManager()
{
    SPDLOG_TRACE("ApiManager::~ApiManager");
}

void ApiManager::loginUser(const QString &username, const QString &password)
{
    SPDLOG_TRACE(
        "ApiManager::loginUser username={}, password={}",
        username.toStdString(),
        password.toStdString());

    Dataset dataset;
    dataset[Keys::User::USERNAME] = {username};
    dataset[Keys::User::PASSWORD] = {password};
    m_networkService.sendRequest(
        Endpoints::Users::LOGIN,
        Method::POST,
        dataset);
}

void ApiManager::registerUser(const QString &username, const QString &password)
{
    SPDLOG_TRACE("ApiManager::registerUser");

    Dataset dataset;
    dataset["username"] = {username};
    dataset["password"] = {password};

    m_networkService.sendRequest(
        Endpoints::Users::REGISTER,
        Method::POST,
        dataset);
}

void ApiManager::setupHandlers()
{
    SPDLOG_TRACE("ApiManager::setupHandlers");
    m_responseHandlers[Endpoints::Users::LOGIN] = [this](Method method, const Dataset &dataset)
    { handleLoginResponse(method, dataset); };
    m_responseHandlers[Endpoints::Users::REGISTER] = [this](Method method, const Dataset &dataset)
    { handleRegistrationResponse(method, dataset); };
}

void ApiManager::handleResponse(const QString &endpoint, Method method, const Dataset &dataset)
{
    SPDLOG_DEBUG("ApiManager::handleResponse for endpoint={}", endpoint.toStdString());

    auto handler = m_responseHandlers.find(endpoint);
    if (handler != m_responseHandlers.end())
    {
        QString errorMsg{};
        if (dataset.contains(Keys::_ERROR))
            errorMsg = dataset[Keys::_ERROR].front();

        if (errorMsg.isEmpty())
            handler.value()(method, dataset);
        else
            handleError(errorMsg);
    }
    else
    {
        SPDLOG_ERROR("ApiManager::handleResponse | Can't find handler for endpoint={}", endpoint.toStdString());
    }
}

void ApiManager::handleError(const QString &errorMessage)
{
    SPDLOG_TRACE("ApiManager::handleError | {}", errorMessage.toStdString());
    emit errorOccurred(errorMessage);
}

void ApiManager::handleLoginResponse(Method, const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleLoginResponse");

    if (!dataset[Keys::User::ID].isEmpty() && !dataset[Keys::User::ROLE_ID].isEmpty())
    {
        const auto id = dataset[Keys::User::ID].front();
        const auto roleId = dataset[Keys::User::ROLE_ID].front();

        if (!id.isEmpty() && !roleId.isEmpty())
            emit loginSuccess(id, roleId);
        else
            handleError("ApiManager::handleLoginResponse | empty id or roleId");
    }
    else
    {
        handleError("ApiManager::handleLoginResponse | empty lists");
    }
}

void ApiManager::handleRegistrationResponse(Method, const Dataset &)
{
    SPDLOG_TRACE("ApiManager::handleRegisterResponse");
    emit registerSuccess();
}
