#pragma once

#include "Network/ApiManager.hpp"

#include "mocks/NetworkServiceMock.hpp"

class ApiManagerMock : public ApiManager {
public:
    ApiManagerMock() : ApiManager(*(new NetworkServiceMock)) {}

public slots:
    void addUser(const User &user) override { emit userAdded(); }
    void editUser(const User &user) override { emit userEdited(); }
    void deleteUser(const QString &id) override { emit userDeleted(); }
    void getUsers() override { emit usersList({}); }
    void createRole(const Role &role) override { emit roleCreated(); }
    void editRole(const Role &role) override { emit roleEdited(); }
    void deleteRole(const QString &id) override { emit roleDeleted(); }
    void getRoleList() override { emit rolesList({}); }
    void emitError(const QString &errorMessage) { emit errorOccurred(errorMessage); }
};
