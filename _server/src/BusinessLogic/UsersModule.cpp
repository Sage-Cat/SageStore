#include "BusinessLogic/UsersModule.hpp"

#include "ServerException.hpp"

#include "Database/RepositoryManager.hpp"

#include "Database/Entities/User.hpp"
#include "Database/Entities/Role.hpp"

#include "DataCommon.hpp"
#include "SpdlogConfig.hpp"

#define _M "UsersModule"

UsersModule::UsersModule(RepositoryManager &repositoryManager)
{
    SPDLOG_TRACE("UsersModule::UsersModule");

    m_usersRepository = std::move(repositoryManager.getUsersRepository());
    m_rolesRepository = std::move(repositoryManager.getRolesRepository());
}

UsersModule::~UsersModule()
{
    SPDLOG_TRACE("UsersModule::~UsersModule");
}

ResponseData UsersModule::executeTask(const RequestData &requestData)
{
    SPDLOG_TRACE("UsersModule::executeTask");
    ResponseData response;

    if (requestData.submodule == "login" && requestData.method == "POST")
    {
        response = loginUser(requestData.dataset);
    }
    else if (requestData.submodule == "register" && requestData.method == "POST")
    {
        response = registerUser(requestData.dataset);
    }
    else
    {
        SPDLOG_ERROR("UsersModule::executeTask Unrecognized task: " + requestData.submodule + "/" + requestData.method);
        throw ServerException(_M, "Unrecognized task");
    }

    return response;
}

ResponseData UsersModule::loginUser(const Dataset &request)
{
    SPDLOG_TRACE("UsersModule::loginUser");
    ResponseData response;

    std::string clientUsername{}, clientPassword{};
    try
    {
        clientUsername = request.at(Keys::User::USERNAME).front();
        clientPassword = request.at(Keys::User::PASSWORD).front();
    }
    catch (const std::out_of_range &e)
    {
        SPDLOG_ERROR("UsersModule::loginUser | dataset has no needed info: {}", e.what());
        throw ServerException(_M, "Server got no username or password from Client");
    }

    // get user by username from repository
    auto usersVec = m_usersRepository->getByField(Keys::User::USERNAME, clientUsername);

    if (!usersVec.empty())
    {
        User &user = usersVec.front();
        if (user.password == clientPassword)
        {
            SPDLOG_INFO("UsersModule::loginUser | SUCCESS");
            user >> response.dataset;
        }
        else
        {
            SPDLOG_WARN("UsersModule::loginUser | user entered incorrect password");
            throw ServerException(_M, "Wrong password");
        }
    }
    else
    {
        SPDLOG_WARN("UsersModule::loginUser | user does not exist");
        throw ServerException(_M, "User with such username does not exist");
    }

    return response;
}

ResponseData UsersModule::registerUser(const Dataset &request)
{
    SPDLOG_TRACE("UsersModule::registerUser");
    ResponseData response;

    std::string clientUsername{}, clientPassword{};
    try
    {
        clientUsername = request.at(Keys::User::USERNAME).front();
        clientPassword = request.at(Keys::User::PASSWORD).front();
    }
    catch (const std::out_of_range &e)
    {
        SPDLOG_WARN("UsersModule::registerUser username or password does not exit");
        throw ServerException(_M, "Server got no username or password in Dataset");
    }

    // get user by username from repository
    auto usersVec = m_usersRepository->getByField(Keys::User::USERNAME, clientUsername);

    if (usersVec.empty())
    {
        m_usersRepository->add(User("", clientUsername, clientPassword, ""));
        SPDLOG_INFO("UsersModule::registerUser | new user `{}` is registered", clientUsername);
    }
    else
    {
        SPDLOG_WARN("UsersModule::registerUser | user with such username already exists");
        throw ServerException(_M, "User with this username already exists");
    }

    return response;
}
