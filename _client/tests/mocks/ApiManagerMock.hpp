#pragma once

#include "Network/ApiManager.hpp"

class NetworkService;

class ApiManagerMock : public ApiManager
{
public:
    explicit ApiManagerMock(NetworkService &networkService) : ApiManager(networkService){};

public slots:
    void loginUser(const QString &username, const QString &password)
    {
        emit loginSuccess(0, 0);
    }

    void registerUser(const QString &username, const QString &password)
    {
        emit registerSuccess();
    }
};
