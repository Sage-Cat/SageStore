#include "Network/ApiManager.hpp"

#include "common/Endpoints.hpp"

#include "Network/NetworkService.hpp"

#include "common/Keys.hpp"
#include "common/SpdlogConfig.hpp"

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
    dataset[Keys::User::USERNAME] = {username.toStdString()};
    dataset[Keys::User::PASSWORD] = {password.toStdString()};
    m_networkService.sendRequest(
        Endpoints::Users::LOGIN,
        Method::POST,
        dataset);
}

void ApiManager::registerUser(const QString &username, const QString &password)
{
    SPDLOG_TRACE("ApiManager::registerUser");

    Dataset dataset;
    dataset["username"] = {username.toStdString()};
    dataset["password"] = {password.toStdString()};

    m_networkService.sendRequest(
        Endpoints::Users::REGISTER,
        Method::POST,
        dataset);
}

void ApiManager::getRoleList()
{
    SPDLOG_TRACE("ApiManager::getRole");

    m_networkService.sendRequest(
        Endpoints::Users::ROLES,
        Method::GET);
}

void ApiManager::createNewRole(const QString &roleName)
{
    SPDLOG_TRACE("ApiManager::postRole");
    Dataset dataset;
    dataset[Keys::Role::NAME] = {roleName.toStdString()};
    dataset[Keys::Role::ID] = {};

    m_networkService.sendRequest(
        Endpoints::Users::ROLES,
        Method::POST,
        dataset);
}

void ApiManager::editRole(const QString &id, const QString &roleName)
{
    SPDLOG_TRACE("ApiManager::editRole");
    Dataset dataset;
    dataset[Keys::Role::NAME] = {roleName.toStdString()};

    m_networkService.sendRequest(
        Endpoints::Users::ROLES,
        Method::PUT,
        dataset,
        id.toStdString());
}

void ApiManager::deleteRole(const QString &id)
{
    SPDLOG_TRACE("ApiManager::deleteRole");

    m_networkService.sendRequest(
        Endpoints::Users::ROLES,
        Method::DEL,
        {},
        id.toStdString());
}

void ApiManager::setupHandlers()
{
    SPDLOG_TRACE("ApiManager::setupHandlers");
    m_responseHandlers[Endpoints::Users::LOGIN] = [this](Method method, const Dataset &dataset)
    { handleLoginResponse(method, dataset); };
    m_responseHandlers[Endpoints::Users::REGISTER] = [this](Method method, const Dataset &dataset)
    { handleRegistrationResponse(method, dataset); };
    m_responseHandlers[Endpoints::Users::ROLES] = [this](Method method, const Dataset &dataset)
    { handleRoles(method, dataset); };
}

void ApiManager::handleResponse(const std::string &endpoint, Method method, const Dataset &dataset)
{
    SPDLOG_DEBUG("ApiManager::handleResponse for endpoint={}", endpoint);
    auto handler = m_responseHandlers.find(endpoint);
    if (handler != m_responseHandlers.end())
    {
        std::string errorMsg{};
        if (dataset.contains(Keys::_ERROR))
        {
            for (const auto &errorData : dataset.at(Keys::_ERROR))
            {
                errorMsg += errorData + "\n";
            }
        }

        if (errorMsg.empty())
        {
            handler->second(method, dataset);
        }
        else
        {
            handleError(errorMsg);
        }
    }
    else
    {
        SPDLOG_ERROR("ApiManager::handleResponse | Can't find handler for endpoint={}", endpoint);
    }
}

void ApiManager::handleError(const std::string &errorMessage)
{
    SPDLOG_TRACE("ApiManager::handleError | {}", errorMessage);
    emit errorOccurred(QString::fromStdString(errorMessage));
}

void ApiManager::handleRoles(Method method, const Dataset &dataset)
{
    switch (method)
    {
    case Method::GET:
        handleRolesList(dataset);
        break;
    case Method::POST:
        emit roleCreated();
        break;
    case Method::PUT:
        emit roleEdited();
        break;
    case Method::DEL:
        emit roleDeleted();
        break;
    default:
        break;
    }
}

void ApiManager::handleLoginResponse(Method, const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleLoginResponse");

    try
    {
        const auto &id_list = dataset.at(Keys::User::ID);
        const auto &role_id_list = dataset.at(Keys::User::ROLE_ID);

        if (id_list.empty() || role_id_list.empty())
        {
            handleError("ApiManager::handleLoginResponse | empty lists");
            return;
        }

        const std::string &id = id_list.front();
        const std::string &roleId = role_id_list.front();

        if (id.empty() || roleId.empty())
        {
            handleError("ApiManager::handleLoginResponse | empty id or roleId");
        }
        else
        {
            emit loginSuccess(QString::fromStdString(id), QString::fromStdString(roleId));
        }
    }
    catch (const std::out_of_range &e)
    {
        SPDLOG_ERROR("Missing required keys in dataset: {}", e.what());
        handleError("ApiManager::handleLoginResponse | missing key in dataset");
    }
}

void ApiManager::handleRegistrationResponse(Method, const Dataset &)
{
    SPDLOG_TRACE("ApiManager::handleRegisterResponse");
    emit registrationSuccess();
}

void ApiManager::handleRolesList(const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleRolesList");

    try
    {
        if (!dataset.contains(Keys::Role::ID) || !dataset.contains(Keys::Role::NAME))
        {
            handleError("dataset doesn't contain ID or NAME lists");
            return;
        }

        const auto &idList = dataset.at(Keys::Role::ID);
        const auto &nameList = dataset.at(Keys::Role::NAME);

        if (idList.size() != nameList.size())
        {
            handleError("ID and Name lists have different sizes.");
            return;
        }

        std::list<Role> roles;
        auto idIt = idList.begin();
        auto nameIt = nameList.begin();

        while (idIt != idList.end() && nameIt != nameList.end())
        {
            Role role{
                .id = *idIt,
                .name = *nameIt};
            roles.push_back(role);

            ++idIt;
            ++nameIt;
        }

        emit rolesList(roles);
    }
    catch (const std::out_of_range &e)
    {
        SPDLOG_ERROR("Key missing in dataset: {}", e.what());
        handleError("Key missing in dataset");
    }
}
