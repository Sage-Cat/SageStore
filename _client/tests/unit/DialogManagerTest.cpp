#include <QMessageBox>
#include <QSignalSpy>
#include <QtTest>

#include "wrappers/DialogManagerWrapper.hpp"

#include "mocks/ApiManagerMock.hpp"

class DialogManagerTest : public QObject {
    Q_OBJECT

    DialogManager *dialogManager;

    ApiManagerMock *apiManagerMock;

public:
    DialogManagerTest()
    {
        apiManagerMock = new ApiManagerMock();

        dialogManager = new DialogManagerWrapper(*apiManagerMock);
        dialogManager->init();
    }

    ~DialogManagerTest()
    {
        delete dialogManager;
        delete apiManagerMock;
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
        QSignalSpy registrationAttemptedSpy(apiManagerMock, &ApiManager::userAdded);
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
