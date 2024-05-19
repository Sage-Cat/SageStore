#include <QApplication>
#include <QSignalSpy>
#include <QtTest>

#include "Ui/MainWindow.hpp"

#include "mocks/ApiManagerMock.hpp"
#include "mocks/DialogManagerMock.hpp"
#include "mocks/NetworkServiceMock.hpp"

/* This component test performs testing of MainWindow+DialogManager (NO UI)
 * logic block It's purpose is to test main signal-slot connections between
 * network API and UI elemets
 */
class MainWindowTest : public QObject {
    Q_OBJECT

    MainWindow *mainWindow;

    QApplication &app;
    NetworkServiceMock *networkServiceMock;
    ApiManagerMock *apiManagerMock;
    DialogManagerMock *dialogManagerMock;

public:
    MainWindowTest(QApplication &app) : app(app)
    {
        networkServiceMock = new NetworkServiceMock;
        apiManagerMock     = new ApiManagerMock(*networkServiceMock);
        dialogManagerMock  = new DialogManagerMock(*apiManagerMock);

        mainWindow = new MainWindow(app, *apiManagerMock, *dialogManagerMock);
    }

    ~MainWindowTest()
    {
        delete mainWindow;
        delete apiManagerMock;
        delete networkServiceMock;
    }

private slots:
    void testShowLoginDialogOnInit()
    {
        QSignalSpy errorMessageSpy(dialogManagerMock, &DialogManagerMock::showLogDialog);
        mainWindow->startUiProcess();
        QCOMPARE(errorMessageSpy.count(), 1);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindowTest tc(app);
    return QTest::qExec(&tc, argc, argv);
}

#include "MainWindowTest.moc"
