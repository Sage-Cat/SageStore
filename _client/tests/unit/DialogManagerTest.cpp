#include <QMessageBox>
#include <QSignalSpy>
#include <QtTest>

#include "wrappers/DialogManagerWrapper.hpp"

#include "mocks/ApiManagerMock.hpp"

class DialogManagerTest : public QObject {
    Q_OBJECT

    DialogManagerWrapper *dialogManager{nullptr};
    ApiManagerMock *apiManagerMock{nullptr};

private slots:
    void init()
    {
        apiManagerMock = new ApiManagerMock();
        dialogManager  = new DialogManagerWrapper(*apiManagerMock);
        dialogManager->init();
    }

    void cleanup()
    {
        delete dialogManager;
        delete apiManagerMock;
        dialogManager = nullptr;
        apiManagerMock = nullptr;
    }

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

    void testLoginDialogRejectEmitsCancellationBeforeAuthentication()
    {
        dialogManager->setLoginAutoSubmit(false);

        QSignalSpy cancelledSpy(dialogManager, &DialogManager::loginCancelled);
        dialogManager->showLoginDialog();
        dialogManager->rejectLoginDialog();

        QCOMPARE(cancelledSpy.count(), 1);
    }
};

QTEST_MAIN(DialogManagerTest)
#include "DialogManagerTest.moc"
