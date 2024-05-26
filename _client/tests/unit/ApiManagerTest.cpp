#include <QSignalSpy>
#include <QtTest>

#include "Network/ApiManager.hpp"
#include "mocks/NetworkServiceMock.hpp"

class ApiManagerTest : public QObject {
    Q_OBJECT

    ApiManager *apiManager;
    NetworkServiceMock *networkServiceMock;

public:
    ApiManagerTest()
    {
        networkServiceMock = new NetworkServiceMock;
        apiManager         = new ApiManager(*networkServiceMock);
    }

    ~ApiManagerTest() { delete apiManager; }

private slots:
    void testSuccessfulLogin()
    {
        QSignalSpy loginSuccessSpy(apiManager, &ApiManager::loginSuccess);
        apiManager->loginUser("testUser", "testPassword");
        QCOMPARE(loginSuccessSpy.count(), 1);
    }

    void testSuccessfulGetRole()
    {
        QSignalSpy getSuccessSpy(apiManager, &ApiManager::rolesList);
        apiManager->getRoleList();
        QCOMPARE(getSuccessSpy.count(), 1);
    }

    void testSuccessfulAddRole()
    {
        QSignalSpy addSuccessSpy(apiManager, &ApiManager::roleCreated);
        apiManager->createRole(Common::Entities::Role{.name = "testName"});
        QCOMPARE(addSuccessSpy.count(), 1);
    }

    void testSuccessfulEditRoles()
    {
        QSignalSpy editSuccessSpy(apiManager, &ApiManager::roleEdited);
        apiManager->editRole(Common::Entities::Role{.id = "0", .name = "testName"});
        QCOMPARE(editSuccessSpy.count(), 1);
    }

    void testSuccessfulDeleteRole()
    {
        QSignalSpy deleteSuccessSpy(apiManager, &ApiManager::roleDeleted);
        apiManager->deleteRole("0");
        QCOMPARE(deleteSuccessSpy.count(), 1);
    }

    void testSuccessfulGetUsers()
    {
        QSignalSpy getUsersSuccessSpy(apiManager, &ApiManager::usersList);
        apiManager->getUsers();
        QCOMPARE(getUsersSuccessSpy.count(), 1);
    }

    void testSuccessfulAddUser()
    {
        QSignalSpy addUserSuccessSpy(apiManager, &ApiManager::userAdded);
        apiManager->addUser(Common::Entities::User{
            .id = "1", .username = "newUser", .password = "newPassword", .roleId = "1"});
        QCOMPARE(addUserSuccessSpy.count(), 1);
    }

    void testSuccessfulUpdateUser()
    {
        QSignalSpy updateUserSuccessSpy(apiManager, &ApiManager::userEdited);
        apiManager->editUser(Common::Entities::User{
            .id = "1", .username = "updatedUser", .password = "updatedPassword", .roleId = "1"});
        QCOMPARE(updateUserSuccessSpy.count(), 1);
    }

    void testSuccessfulDeleteUser()
    {
        QSignalSpy deleteUserSuccessSpy(apiManager, &ApiManager::userDeleted);
        apiManager->deleteUser("1");
        QCOMPARE(deleteUserSuccessSpy.count(), 1);
    }
};

QTEST_MAIN(ApiManagerTest)
#include "ApiManagerTest.moc"
