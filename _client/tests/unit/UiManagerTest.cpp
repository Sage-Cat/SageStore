#include <QtTest>
#include <QSignalSpy>
#include <QApplication>

#include "Ui/UiManager.hpp"
#include "tests/wrappers/UiManagerWrapper.hpp"

#include "tests/mocks/ApiManagerMock.hpp"
#include "tests/mocks/NetworkServiceMock.hpp"

class UiManagerTest : public QObject
{
    Q_OBJECT

    UiManager *uiManager;

    QApplication &app;
    NetworkServiceMock *networkServiceMock;
    ApiManagerMock *apiManagerMock;

public:
    UiManagerTest(QApplication &app) : app(app)
    {
        networkServiceMock = new NetworkServiceMock;
        apiManagerMock = new ApiManagerMock(*networkServiceMock);

        uiManager = new UiManagerWrapper(app, *apiManagerMock);
        uiManager->init();
    }

    ~UiManagerTest()
    {
        delete uiManager;
        delete apiManagerMock;
        delete networkServiceMock;
    }

private slots:
    void testSomeConnection()
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
