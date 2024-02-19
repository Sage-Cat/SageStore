#include "UsersModule.hpp"

#include "ServerException.hpp"
#include "Database/UsersRepository.hpp"
#include "Database/RolesRepository.hpp"

#include "DatasetCommon.hpp"
#include "SpdlogConfig.hpp"

#define _M "UsersModule"

UsersModule::UsersModule(std::shared_ptr<UsersRepository> usersRepository,
                         std::shared_ptr<RolesRepository> rolesRepository)
    : m_usersRepository(std::move(usersRepository)),
      m_rolesRepository(std::move(rolesRepository))
{
    SPDLOG_TRACE("UsersModule::UsersModule");
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
        SPDLOG_ERROR("UsersModule::loginUser username or password does not exit");
        throw ServerException(_M, "Server got no username or password in Dataset");
    }

    // get user by username from repository
    auto userOpt = m_usersRepository->getByUsername(clientUsername);

    if (userOpt.has_value())
    {
        User user = userOpt.value();
        if (user.password == clientPassword)
        {
            user >> response.dataset;
        }
        else
        {
            SPDLOG_ERROR("UsersModule::loginUser | user entered incorrect password");
            throw ServerException(_M, "Wrong password");
        }
    }
    else
    {
        SPDLOG_ERROR("UsersModule::loginUser | user does not exist");
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
        SPDLOG_ERROR("UsersModule::registerUser username or password does not exit");
        throw ServerException(_M, "Server got no username or password in Dataset");
    }

    // get user by username from repository
    auto userOpt = m_usersRepository->getByUsername(clientUsername);

    if (!userOpt.has_value())
    {
        m_usersRepository->add(User("", clientUsername, clientPassword, ""));
    }
    else
    {
        SPDLOG_WARN("UsersModule::registerUser such user aready is in database");
        throw ServerException(_M, "User with this name already exists");
    }

    return response;
}
