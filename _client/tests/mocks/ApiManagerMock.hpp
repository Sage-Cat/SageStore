#pragma once

#include "Network/ApiManager.hpp"

class NetworkService;

class ApiManagerMock : public ApiManager
{
public:
    explicit ApiManagerMock(NetworkService &networkService) : ApiManager(networkService){};

public slots:
    void loginUser(const QString &username, const QString &password) override
    {
        emit loginSuccess(0, 0);
    }

    void registerUser(const QString &username, const QString &password) override
    {
        emit registrationSuccess();
    }

    void getRole() override
    {
        emit getRoleSuccess();
    }

    void postNewRole(const QString &newRole) override
    {
        emit roleSuccess();
    }

    void editRole(const QString &id, const QString &edit_role) override
    {
        emit roleSuccess();
    }

    void deleteRole(const QString &id) override
    {
        emit roleSuccess();
    }
    void emitError(const QString &errorMessage)
    {
        emit errorOccurred(errorMessage);
    }
};

// virtual void getRole();
// virtual void postNewRole(const QString &newRole);
// virtual void editRole(const QString &edit_role, const QString &id);
// virtual void deleteRole(const QString &delete_role, const QString &id);

// void
// void
// void
// void