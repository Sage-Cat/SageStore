#include "UsersModule.hpp"

#include "ServerException.hpp"

#include "Database/RepositoryManager.hpp"

#include "common/Entities/Role.hpp"
#include "common/Entities/User.hpp"

#include "common/Keys.hpp"
#include "common/SpdlogConfig.hpp"

#define _M "UsersModule"

UsersModule::UsersModule(RepositoryManager &repositoryManager)
{
    SPDLOG_TRACE("UsersModule::UsersModule");

    m_usersRepository = std::move(repositoryManager.getUserRepository());
    m_rolesRepository = std::move(repositoryManager.getRoleRepository());
}

UsersModule::~UsersModule() { SPDLOG_TRACE("UsersModule::~UsersModule"); }

ResponseData UsersModule::executeTask(const RequestData &requestData)
{
    SPDLOG_TRACE("UsersModule::executeTask");
    ResponseData response;

    SPDLOG_DEBUG("UsersModule::executeTask | Submodule: {}, Method: {}", requestData.submodule,
                 requestData.method);

    if (requestData.submodule == "login" && requestData.method == "POST") {
        response = loginUser(requestData.dataset);
    } else if (requestData.submodule == "users") {
        if (requestData.method == "GET") {
            response = getUsers();
        } else if (requestData.method == "POST") {
            addUser(requestData.dataset);
        } else if (requestData.method == "PUT") {
            editUser(requestData.dataset, requestData.resourceId);
        } else if (requestData.method == "DELETE") {
            deleteUser(requestData.resourceId);
        } else {
            SPDLOG_ERROR("UsersModule::executeTask | Submodule = users | Unrecognized method: {}",
                         requestData.method);
            throw ServerException(_M, "Unrecognized method");
        }
    } else if (requestData.submodule == "roles") {
        if (requestData.method == "GET") {
            response = getRoles();
        } else if (requestData.method == "POST") {
            addRole(requestData.dataset);
        } else if (requestData.method == "PUT") {
            updateRole(requestData.dataset, requestData.resourceId);
        } else if (requestData.method == "DELETE") {
            deleteRole(requestData.resourceId);
        } else {
            SPDLOG_ERROR("UsersModule::executeTask | Submodule = users | Unrecognized method: {}",
                         requestData.method);
            throw ServerException(_M, "Unrecognized method");
        }
    } else {
        SPDLOG_ERROR("UsersModule::executeTask | Unrecognized task: {}/{}", requestData.submodule,
                     requestData.method);
        throw ServerException(_M, "Unrecognized task");
    }

    return response;
}

ResponseData UsersModule::loginUser(const Dataset &request)
{
    SPDLOG_TRACE("UsersModule::loginUser");
    ResponseData response;

    std::string clientUsername{}, clientPassword{};
    try {
        clientUsername = request.at(Common::Entities::User::USERNAME_KEY).front();
        clientPassword = request.at(Common::Entities::User::PASSWORD_KEY).front();
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("UsersModule::loginUser | dataset has no needed info: {}", e.what());
        throw ServerException(_M, "Server got no username or password from Client");
    }

    // get user by username from repository
    auto usersVec =
        m_usersRepository->getByField(Common::Entities::User::USERNAME_KEY, clientUsername);

    if (usersVec.empty()) {
        SPDLOG_WARN("UsersModule::loginUser | user does not exist");
        throw ServerException(_M, "User with such username does not exist");
    } else {
        Common::Entities::User &user = usersVec.front();
        if (user.password == clientPassword) { // password is not being sent to client
            SPDLOG_INFO("UsersModule::loginUser | SUCCESS");
            response.dataset[Common::Entities::User::ID_KEY].push_back(user.id);
            response.dataset[Common::Entities::User::ROLE_ID_KEY].push_back(user.roleId);
        } else {
            SPDLOG_WARN("UsersModule::loginUser | user entered incorrect password");
            throw ServerException(_M, "Wrong password");
        }
    }

    return response;
}

ResponseData UsersModule::getUsers()
{
    SPDLOG_TRACE("UsersModule::getUsers");
    ResponseData response;
    auto usersVec = m_usersRepository->getAll();

    if (usersVec.empty()) {
        SPDLOG_WARN("UsersModule::getUsers | array of users is empty");
    } else {
        for (const auto &user : usersVec) { // password is not being sent to client
            response.dataset[Common::Entities::User::ID_KEY].emplace_back(user.id);
            response.dataset[Common::Entities::User::USERNAME_KEY].emplace_back(user.username);
            response.dataset[Common::Entities::User::PASSWORD_KEY].emplace_back(user.password);
            response.dataset[Common::Entities::User::ROLE_ID_KEY].emplace_back(user.roleId);
        }
    }

    return response;
}

void UsersModule::addUser(const Dataset &request)
{
    SPDLOG_TRACE("UsersModule::addUser");
    std::string username, password, roleId;
    try {
        username = request.at(Common::Entities::User::USERNAME_KEY).front();
        password = request.at(Common::Entities::User::PASSWORD_KEY).front();

        // We could have no Role (it means to use default)
        if (request.contains(Common::Entities::User::ROLE_ID_KEY))
            request.at(Common::Entities::User::ROLE_ID_KEY);
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("UsersModule::addUser | Missing user data: {}", e.what());
        throw ServerException(_M, "Incomplete user data");
    }

    auto usersVec = m_usersRepository->getByField(Common::Entities::User::USERNAME_KEY, username);
    if (!usersVec.empty()) {
        SPDLOG_WARN("UsersModule::addUser | User already exists");
        throw ServerException(_M, "User with this username already exists");
    }

    Common::Entities::User newUser{
        .id = "", .username = username, .password = password, .roleId = roleId};
    m_usersRepository->add(newUser);
    SPDLOG_INFO("UsersModule::addUser | New user `{}` added", username);
}

void UsersModule::editUser(const Dataset &request, const std::string &userId)
{
    SPDLOG_TRACE("UsersModule::editUser");
    if (userId.empty()) {
        throw ServerException(_M, "User ID is empty");
    }

    std::string username, password, roleId;
    try {
        username = request.at(Common::Entities::User::USERNAME_KEY).front();
        password = request.at(Common::Entities::User::PASSWORD_KEY).front();
        roleId   = request.at(Common::Entities::User::ROLE_ID_KEY).front();
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("UsersModule::editUser | Missing user data: {}", e.what());
        throw ServerException(_M, "Incomplete user data");
    }

    Common::Entities::User updatedUser{
        .id = userId, .username = username, .password = password, .roleId = roleId};
    m_usersRepository->update(updatedUser);
    SPDLOG_INFO("UsersModule::editUser | User `{}` updated", username);
}

void UsersModule::deleteUser(const std::string &userId)
{
    SPDLOG_TRACE("UsersModule::deleteUser");
    if (userId.empty()) {
        throw ServerException(_M, "User ID is empty");
    }

    m_usersRepository->deleteResource(userId);
    SPDLOG_INFO("UsersModule::deleteUser | User `{}` deleted", userId);
}

ResponseData UsersModule::getRoles()
{
    SPDLOG_TRACE("UsersModule::getRole");
    ResponseData response;
    auto rolesVec = m_rolesRepository->getAll();

    if (rolesVec.empty()) {
        response.dataset[Common::Entities::Role::ID_KEY]   = {""};
        response.dataset[Common::Entities::Role::NAME_KEY] = {""};
        SPDLOG_WARN("UsersModule::getRoles | array of roles is empty");
    } else {
        for (auto i = rolesVec.cbegin(); i != rolesVec.cend(); i++) {
            response.dataset[Common::Entities::Role::ID_KEY].emplace_back(i->id);
            response.dataset[Common::Entities::Role::NAME_KEY].emplace_back(i->name);
        }
    }

    return response;
}

void UsersModule::addRole(const Dataset &request)
{
    SPDLOG_TRACE("UsersModule::addRoles");
    ResponseData response;
    std::string roleName{};
    try {
        roleName = request.at(Common::Entities::Role::NAME_KEY).front();
    } catch (const std::out_of_range &e) {
        SPDLOG_WARN("UsersModule::addRole username or password does not exit");
        throw ServerException(_M, "Server got no username or password in Dataset");
    }
    auto usersVec = m_rolesRepository->getByField(Common::Entities::Role::NAME_KEY, roleName);
    if (usersVec.empty()) {
        const Common::Entities::Role role{.id = "" /* autoincremented */, .name = roleName};
        m_rolesRepository->add(role);
        SPDLOG_INFO("UsersModule::registerUser | new role `{}` is added", roleName);
    } else {
        throw ServerException(_M, "Server got no username or password in Dataset");
    }
}

void UsersModule::updateRole(const Dataset &request, const std::string &resourseId)
{
    SPDLOG_TRACE("UsersModule::updateRole");
    if (resourseId.empty()) {
        throw ServerException(_M, "resourseId is emtpy");
    }

    std::string roleName{};
    try {
        m_rolesRepository->update(
            Common::Entities::Role{.id = {} /* autoincremented */, .name = roleName});
        roleName = request.at(Common::Entities::Role::NAME_KEY).front();
    } catch (std::out_of_range &e) {
        SPDLOG_WARN("UsersModule::updateRole role name does not exit");
        throw ServerException(_M, "Server got no role name in Dataset");
    }
}

void UsersModule::deleteRole(const std::string &resourseId)
{
    SPDLOG_TRACE("UsersModule::deleteRole");
    if (resourseId.empty()) {
        throw ServerException(_M, "resourseId is emtpy");
    }
    m_rolesRepository->deleteResource(resourseId);
}