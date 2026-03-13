#include <QApplication>
#include <QSignalSpy>
#include <QTabWidget>
#include <QtTest>

#include "Ui/MainWindow.hpp"
#include "Ui/MainMenuActions.hpp"

#include "mocks/ApiManagerMock.hpp"
#include "mocks/DialogManagerMock.hpp"

/* This component test performs testing of MainWindow+DialogManager (NO UI)
 * logic block It's purpose is to test main signal-slot connections between
 * network API and UI elemets
 */
class MainWindowTest : public QObject {
    Q_OBJECT

    MainWindow *mainWindow;

    QApplication &app;
    ApiManagerMock *apiManagerMock;
    DialogManagerMock *dialogManagerMock;

public:
    MainWindowTest(QApplication &app) : app(app)
    {
        apiManagerMock     = new ApiManagerMock();
        dialogManagerMock  = new DialogManagerMock(*apiManagerMock);

        mainWindow = new MainWindow(app, *apiManagerMock, *dialogManagerMock);
    }

    ~MainWindowTest()
    {
        delete mainWindow;
        delete apiManagerMock;
    }

    QAction *findMenuAction(const QString &menuTitle, const QString &actionTitle) const
    {
        for (QAction *menuAction : mainWindow->menuBar()->actions()) {
            if (menuAction->text() != menuTitle || menuAction->menu() == nullptr) {
                continue;
            }

            for (QAction *action : menuAction->menu()->actions()) {
                if (action->text() == actionTitle) {
                    return action;
                }
            }
        }

        return nullptr;
    }

private slots:
    void testShowLoginDialogOnInit()
    {
        QSignalSpy errorMessageSpy(dialogManagerMock, &DialogManagerMock::showLogDialog);
        mainWindow->startUiProcess();
        QCOMPARE(errorMessageSpy.count(), 1);
    }

    void testOpenProductManagementTabFromInventoryMenu()
    {
        auto *tabWidget = mainWindow->findChild<QTabWidget *>();
        QVERIFY(tabWidget != nullptr);
        QCOMPARE(tabWidget->count(), 0);

        QAction *productManagementAction =
            findMenuAction("Inventory",
                           MainMenuActions::NAMES.at(MainMenuActions::Type::PRODUCT_MANAGEMENT));
        QVERIFY(productManagementAction != nullptr);

        productManagementAction->trigger();

        QCOMPARE(tabWidget->count(), 1);
        QCOMPARE(tabWidget->tabText(0),
                 MainMenuActions::NAMES.at(MainMenuActions::Type::PRODUCT_MANAGEMENT));

        productManagementAction->trigger();
        QCOMPARE(tabWidget->count(), 1);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindowTest tc(app);
    return QTest::qExec(&tc, argc, argv);
}

#include "MainWindowTest.moc"
