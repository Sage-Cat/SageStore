#pragma once

#include "Network/ApiManager.hpp"

class NetworkService;

class ApiManagerMock : public ApiManager {
public:
    explicit ApiManagerMock(NetworkService &networkService) : ApiManager(networkService){};

public slots:
    void loginUser(const QString &username, const QString &password) override
    {
        emit loginSuccess(0, 0);
    }

    void addUser(const User &user) override { emit userAdded(); }

    void emitError(const QString &errorMessage) { emit errorOccurred(errorMessage); }
};
