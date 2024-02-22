#pragma once

#include "Endpoints.hpp"
#include "DatasetCommon.hpp"
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
        const Dataset &dataset = Dataset()) override
    {
        Dataset serverResponse{{Keys::_ERROR, {"Error message"}}};

        if (endpoint == Endpoints::Users::LOGIN && method == Method::POST)
            serverResponse = {{Keys::User::ID, {"0"}}, {Keys::User::ROLE_ID, {"0"}}};
        else if (endpoint == Endpoints::Users::REGISTER && method == Method::POST)
            serverResponse = {};

        emit responseReceived(endpoint, method, serverResponse);
    }
};