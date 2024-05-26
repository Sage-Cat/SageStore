#pragma once

#include "Network/NetworkService.hpp"
#include "common/Endpoints.hpp"
#include "common/Keys.hpp"

class NetworkServiceMock : public NetworkService {
public:
    NetworkServiceMock() : NetworkService(NetworkService::ServerConfig(), nullptr) {}

    void sendRequest(std::string endpoint, Method method = Method::GET,
                     const Dataset &dataset         = Dataset(),
                     const std::string &resource_id = "") override
    {
        Dataset serverResponse{{Keys::_ERROR, {"Error message"}}};

        if (endpoint == Endpoints::Users::LOGIN && method == Method::POST) {
            serverResponse = {{Common::Entities::User::ID_KEY, {"0"}},
                              {Common::Entities::User::ROLE_ID_KEY, {"0"}}};
        } else if (endpoint == Endpoints::Users::USERS && method == Method::GET) {
            serverResponse = {{Common::Entities::User::ID_KEY, {"1", "2"}},
                              {Common::Entities::User::USERNAME_KEY, {"user1", "user2"}},
                              {Common::Entities::User::PASSWORD_KEY, {"password1", "password2"}},
                              {Common::Entities::User::ROLE_ID_KEY, {"1", "2"}}};
        } else if (endpoint == Endpoints::Users::USERS && method == Method::POST) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Users::USERS && method == Method::PUT) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Users::USERS && method == Method::DEL) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Users::ROLES && method == Method::GET) {
            serverResponse = {{Common::Entities::Role::ID_KEY, {"1", "2", "3"}},
                              {Common::Entities::Role::NAME_KEY, {"adm", "asd", "Asdf"}}};
        } else if (endpoint == Endpoints::Users::ROLES && method == Method::POST) {
            serverResponse = {{Common::Entities::Role::ID_KEY, {"0"}}};
        } else if (endpoint == Endpoints::Users::ROLES && method == Method::PUT) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Users::ROLES && method == Method::DEL) {
            serverResponse = {};
        }
        emit responseReceived(endpoint, method, serverResponse);
    }
};
