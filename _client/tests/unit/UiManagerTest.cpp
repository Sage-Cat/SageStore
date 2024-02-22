#include <QtTest>
#include <QSignalSpy>
#include <QApplication>

#include "Ui/MainWindow.hpp"
#include "Ui/Dialogs/DialogManager.hpp"

#include "UiManagerWrapper.hpp"

#include "ApiManagerMock.hpp"
#include "NetworkServiceMock.hpp"

class UiManagerTest : public QObject
{
    Q_OBJECT

    QApplication &app;
    NetworkServiceMock *networkServiceMock;
    ApiManagerMock *apiManagerMock;
    UiManagerWrapper *uiManager;

public:
    UiManagerTest(QApplication &app) : app(app) {}

private slots:
    void initTestCase()
    {
        networkServiceMock = new NetworkServiceMock;
        apiManagerMock = new ApiManagerMock(*networkServiceMock);
        uiManager = new UiManagerWrapper(app, *apiManagerMock);
    }

    void cleanupTestCase()
    {
        delete uiManager;
        delete apiManagerMock;
        delete networkServiceMock;
    }

    void testSomeMVVMConnection()
    {
        QVERIFY(true);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    UiManagerTest tc(app);
    return QTest::qExec(&tc, argc, argv);
}

#include "UiManagerTest.moc"
