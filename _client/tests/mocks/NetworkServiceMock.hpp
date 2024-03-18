#pragma once

#include "Endpoints.hpp"
#include "DataCommon.hpp"
#include "Network/NetworkService.hpp"

class NetworkServiceMock : public NetworkService
{
public:
    NetworkServiceMock()
    {
    }

    void sendRequest(
        QString endpoint,
        Method method = Method::GET,
        const Dataset &dataset = Dataset(),
        const QString &resource_id = "") override
    {
        Dataset serverResponse{{Keys::_ERROR, {"Error message"}}};

        if (endpoint == Endpoints::Users::LOGIN && method == Method::POST)
            serverResponse = {{Keys::User::ID, {"0"}}, {Keys::User::ROLE_ID, {"0"}}};
        else if (endpoint == Endpoints::Users::REGISTER && method == Method::POST)
            serverResponse = {};
        else if (endpoint == Endpoints::Users::ROLES && method == Method::GET)
            serverResponse = {{Keys::Role::ID, {"1", "2", "3"}}, {Keys::Role::NAME, {"adm", "asd", "Asdf"}}};
        else if (endpoint == Endpoints::Users::ROLES && method == Method::POST)
            serverResponse = {{Keys::User::ROLE_ID, {"0"}}};
        else if (endpoint == Endpoints::Users::ROLES && method == Method::PUT)
            serverResponse = {{}};
        else if (endpoint == Endpoints::Users::ROLES && method == Method::DEL)
            serverResponse = {{}};
        emit responseReceived(endpoint, method, serverResponse);
    }
};
