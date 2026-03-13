#include <QApplication>
#include <QSignalSpy>
#include <QtTest>

#include "Ui/MainWindow.hpp"
#include "Ui/StartupController.hpp"

#include "mocks/ApiManagerMock.hpp"
#include "mocks/DialogManagerMock.hpp"

class StartupControllerTest : public QObject {
    Q_OBJECT

    MainWindow *mainWindow{nullptr};
    ApiManagerMock *apiManagerMock{nullptr};
    DialogManagerMock *dialogManagerMock{nullptr};
    StartupController *startupController{nullptr};
    int mainWindowFactoryCallCount{0};

private slots:
    void init()
    {
        auto *application = qobject_cast<QApplication *>(QCoreApplication::instance());
        QVERIFY(application != nullptr);

        apiManagerMock    = new ApiManagerMock();
        dialogManagerMock = new DialogManagerMock(*apiManagerMock);
        startupController = new StartupController(*dialogManagerMock, [this, application]() {
            ++mainWindowFactoryCallCount;
            mainWindow = new MainWindow(*application, *apiManagerMock, *dialogManagerMock);
            return mainWindow;
        });
    }

    void cleanup()
    {
        delete startupController;
        delete mainWindow;
        delete dialogManagerMock;
        delete apiManagerMock;

        startupController = nullptr;
        mainWindow = nullptr;
        dialogManagerMock = nullptr;
        apiManagerMock = nullptr;
        mainWindowFactoryCallCount = 0;
    }

    void testStartShowsLoginWithoutCreatingMainWindow()
    {
        QSignalSpy showLoginSpy(dialogManagerMock, &DialogManagerMock::showLogDialog);

        startupController->start();

        QCOMPARE(showLoginSpy.count(), 1);
        QCOMPARE(mainWindowFactoryCallCount, 0);
        QVERIFY(startupController->mainWindow() == nullptr);
    }

    void testLoginSuccessCreatesAndShowsMainWindow()
    {
        startupController->start();
        QVERIFY(startupController->mainWindow() == nullptr);

        dialogManagerMock->emitLoginSucceededForTest();
        QCoreApplication::processEvents();

        QCOMPARE(mainWindowFactoryCallCount, 1);
        QVERIFY(mainWindow != nullptr);
        QVERIFY(startupController->mainWindow() == mainWindow);
        QVERIFY(mainWindow->isVisible());
    }

    void testRepeatedLoginSuccessReusesExistingMainWindow()
    {
        startupController->start();

        dialogManagerMock->emitLoginSucceededForTest();
        QCoreApplication::processEvents();
        QVERIFY(mainWindow != nullptr);

        MainWindow *firstMainWindow = mainWindow;
        dialogManagerMock->emitLoginSucceededForTest();
        QCoreApplication::processEvents();

        QCOMPARE(mainWindowFactoryCallCount, 1);
        QVERIFY(startupController->mainWindow() == firstMainWindow);
    }
};

QTEST_MAIN(StartupControllerTest)
#include "StartupControllerTest.moc"
