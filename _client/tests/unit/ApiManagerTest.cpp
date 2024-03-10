#include <QtTest>
#include <QSignalSpy>

#include "Network/ApiManager.hpp"
#include "NetworkServiceMock.hpp"

class ApiManagerTest : public QObject
{
    Q_OBJECT

    ApiManager *apiManager;

    NetworkServiceMock *networkServiceMock;

public:
    ApiManagerTest()
    {
        networkServiceMock = new NetworkServiceMock;
        apiManager = new ApiManager(*networkServiceMock);
    }

    ~ApiManagerTest()
    {
        delete apiManager;
    }

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
        QSignalSpy getSuccessSpy(apiManager, &ApiManager::getRoleSuccess);
        apiManager->getRole();
        QCOMPARE(getSuccessSpy.count(), 1);
    }

    void testSuccessfulAddRole()
    {
        QSignalSpy addSuccwssSpy(apiManager, &ApiManager::roleSuccess);
        apiManager->postNewRole("newRole");
        QCOMPARE(addSuccwssSpy.count(), 1);
    }

    void testSuccessfulEditRoles()
    {
        QSignalSpy editSuccessSSpy(apiManager, &ApiManager::roleSuccess);
        apiManager->editRole("0", "role");
        QCOMPARE(editSuccessSSpy.count(), 1);
    }
    void testSuccessfulDeleteRole()
    {
        QSignalSpy deleteSuccessSpy(apiManager, &ApiManager::roleSuccess);
        apiManager->deleteRole("0");
        QCOMPARE(deleteSuccessSpy.count(), 1);
    }
};

QTEST_MAIN(ApiManagerTest)
#include "ApiManagerTest.moc"
