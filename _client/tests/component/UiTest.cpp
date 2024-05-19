#include <QApplication>
#include <QSignalSpy>
#include <QtTest>

#include "Ui/MainWindow.hpp"
#include "wrappers/DialogManagerWrapper.hpp"

#include "mocks/ApiManagerMock.hpp"
#include "mocks/NetworkServiceMock.hpp"

/* This component test performs testing of MainWindow+DialogManager (NO UI)
 * logic block It's purpose is to test main signal-slot connections between
 * network API and UI elemets
 */
class UiTest : public QObject {
    Q_OBJECT

    QApplication &app;
    MainWindow *mainWindow;
    DialogManagerWrapper *dialogManager;

    NetworkServiceMock *networkServiceMock;
    ApiManagerMock *apiManagerMock;

public:
    UiTest(QApplication &app) : app(app)
    {
        networkServiceMock = new NetworkServiceMock;
        apiManagerMock     = new ApiManagerMock(*networkServiceMock);

        mainWindow = new MainWindow(app, *apiManagerMock, *dialogManager);
    }

    ~UiTest()
    {
        delete mainWindow;
        delete apiManagerMock;
        delete networkServiceMock;
    }

private slots:
    void testSomeConnection() { QVERIFY(true); }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    UiTest tc(app);
    return QTest::qExec(&tc, argc, argv);
}

#include "UiTest.moc"
