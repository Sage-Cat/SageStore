#include <QtTest>
#include <QSignalSpy>

#include "Network/ApiManager.hpp"
#include "NetworkServiceMock.hpp"

class ApiManagerTest : public QObject
{
    Q_OBJECT

    NetworkServiceMock *networkServiceMock;
    ApiManager *apiManager;

private slots:
    void initTestCase()
    {
        networkServiceMock = new NetworkServiceMock;
        apiManager = new ApiManager(*networkServiceMock);
    }

    void cleanupTestCase()
    {
        delete apiManager;
    }

    void testSuccessfulLogin()
    {
        QSignalSpy loginSuccessSpy(apiManager, &ApiManager::loginSuccess);
        apiManager->loginUser("testUser", "testPassword");
        QCOMPARE(loginSuccessSpy.count(), 1);
    }

    void testSuccessfulRegistration()
    {
        QSignalSpy registrationSuccessSpy(apiManager, &ApiManager::registerSuccess);
        apiManager->registerUser("testUser", "testPassword");
        QCOMPARE(registrationSuccessSpy.count(), 1);
    }
};

QTEST_MAIN(ApiManagerTest)
#include "ApiManagerTest.moc"
