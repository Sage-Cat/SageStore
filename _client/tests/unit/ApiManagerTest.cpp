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

    void testSuccessfulRegistration()
    {
        QSignalSpy registrationSuccessSpy(apiManager, &ApiManager::registrationSuccess);
        apiManager->registerUser("testUser", "testPassword");
        QCOMPARE(registrationSuccessSpy.count(), 1);
    }

    void testSuccessfulGetRole()
    {
        QSignalSpy getSuccessSpy(apiManager, &ApiManager::rolesList);
        apiManager->getRoleList();
        QCOMPARE(getSuccessSpy.count(), 1);
    }

    void testSuccessfulAddRole()
    {
        QSignalSpy addSuccwssSpy(apiManager, &ApiManager::roleCreated);
        apiManager->createNewRole("newRole");
        QCOMPARE(addSuccwssSpy.count(), 1);
    }

    void testSuccessfulEditRoles()
    {
        QSignalSpy editSuccessSSpy(apiManager, &ApiManager::roleEdited);
        apiManager->editRole("0", "role");
        QCOMPARE(editSuccessSSpy.count(), 1);
    }

    void testSuccessfulDeleteRole()
    {
        QSignalSpy deleteSuccessSpy(apiManager, &ApiManager::roleDeleted);
        apiManager->deleteRole("0");
        QCOMPARE(deleteSuccessSpy.count(), 1);
    }
};

QTEST_MAIN(ApiManagerTest)
#include "ApiManagerTest.moc"
