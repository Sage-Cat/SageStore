#include <QtTest>
#include <QSignalSpy>
#include <QMessageBox>

#include "DialogManagerWrapper.hpp"

#include "ApiManagerMock.hpp"
#include "NetworkServiceMock.hpp"

class DialogManagerTest : public QObject
{
    Q_OBJECT

    DialogManager *dialogManager;

    NetworkServiceMock *networkServiceMock;
    ApiManagerMock *apiManagerMock;

public:
    DialogManagerTest()
    {
        networkServiceMock = new NetworkServiceMock;
        apiManagerMock = new ApiManagerMock(*networkServiceMock);

        dialogManager = new DialogManagerWrapper(*apiManagerMock);
        dialogManager->init();
    }

    ~DialogManagerTest()
    {
        delete dialogManager;
        delete apiManagerMock;
        delete networkServiceMock;
    }

private slots:
    void testLoginRoutine()
    {
        QSignalSpy loginAttemptedSpy(apiManagerMock, &ApiManager::loginSuccess);
        dialogManager->showLoginDialog();
        QCOMPARE(loginAttemptedSpy.count(), 1);
    }

    void testRegistrationRoutine()
    {
        QSignalSpy registrationAttemptedSpy(apiManagerMock, &ApiManager::registrationSuccess);
        dialogManager->showRegistrationDialog();
        QCOMPARE(registrationAttemptedSpy.count(), 1);
    }

    void testErrorMessageBoxConnection()
    {
        QSignalSpy errorMessageSpy(dialogManager->messageDialog(), &QMessageBox::buttonClicked);
        apiManagerMock->emitError("Error message");
        QCOMPARE(errorMessageSpy.count(), 1);
    }
};

QTEST_MAIN(DialogManagerTest)
#include "DialogManagerTest.moc"
