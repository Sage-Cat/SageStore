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

    MainWindow *mainWindow{nullptr};
    ApiManagerMock *apiManagerMock{nullptr};
    DialogManagerMock *dialogManagerMock{nullptr};

private slots:
    void init()
    {
        auto *application = qobject_cast<QApplication *>(QCoreApplication::instance());
        QVERIFY(application != nullptr);

        apiManagerMock    = new ApiManagerMock();
        dialogManagerMock = new DialogManagerMock(*apiManagerMock);
        mainWindow        = new MainWindow(*application, *apiManagerMock, *dialogManagerMock);
    }

    void cleanup()
    {
        delete mainWindow;
        delete dialogManagerMock;
        delete apiManagerMock;

        mainWindow        = nullptr;
        dialogManagerMock = nullptr;
        apiManagerMock    = nullptr;
    }

    void testMainWindowStartsWithoutDataFetch()
    {
        QCOMPARE(apiManagerMock->usersRequestCount(), 0);
        QCOMPARE(apiManagerMock->rolesRequestCount(), 0);
        QCOMPARE(apiManagerMock->productTypesRequestCount(), 0);
        QCOMPARE(apiManagerMock->stocksRequestCount(), 0);
    }

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
        QCOMPARE(apiManagerMock->productTypesRequestCount(), 0);

        QAction *productManagementAction =
            findMenuAction("Inventory",
                           MainMenuActions::NAMES.at(MainMenuActions::Type::PRODUCT_MANAGEMENT));
        QVERIFY(productManagementAction != nullptr);

        productManagementAction->trigger();

        QCOMPARE(tabWidget->count(), 1);
        QCOMPARE(tabWidget->tabText(0),
                 MainMenuActions::NAMES.at(MainMenuActions::Type::PRODUCT_MANAGEMENT));
        QCOMPARE(apiManagerMock->productTypesRequestCount(), 1);

        productManagementAction->trigger();
        QCOMPARE(tabWidget->count(), 1);
        QCOMPARE(apiManagerMock->productTypesRequestCount(), 1);
    }

    void testOpenStockTrackingTabFetchesStockAndCatalogOnce()
    {
        auto *tabWidget = mainWindow->findChild<QTabWidget *>();
        QVERIFY(tabWidget != nullptr);
        const int initialTabCount = tabWidget->count();
        const int initialProductTypeRequests = apiManagerMock->productTypesRequestCount();
        const int initialStockRequests       = apiManagerMock->stocksRequestCount();

        QAction *stockTrackingAction =
            findMenuAction("Inventory",
                           MainMenuActions::NAMES.at(MainMenuActions::Type::STOCK_TRACKING));
        QVERIFY(stockTrackingAction != nullptr);

        stockTrackingAction->trigger();

        QCOMPARE(tabWidget->count(), initialTabCount + 1);
        QCOMPARE(tabWidget->tabText(tabWidget->currentIndex()),
                 MainMenuActions::NAMES.at(MainMenuActions::Type::STOCK_TRACKING));
        QCOMPARE(apiManagerMock->productTypesRequestCount(), initialProductTypeRequests + 1);
        QCOMPARE(apiManagerMock->stocksRequestCount(), initialStockRequests + 1);

        stockTrackingAction->trigger();
        QCOMPARE(tabWidget->count(), initialTabCount + 1);
        QCOMPARE(apiManagerMock->productTypesRequestCount(), initialProductTypeRequests + 1);
        QCOMPARE(apiManagerMock->stocksRequestCount(), initialStockRequests + 1);
    }

    void testReopenStockTrackingTabRefreshesData()
    {
        auto *tabWidget = mainWindow->findChild<QTabWidget *>();
        QVERIFY(tabWidget != nullptr);

        QAction *stockTrackingAction =
            findMenuAction("Inventory",
                           MainMenuActions::NAMES.at(MainMenuActions::Type::STOCK_TRACKING));
        QVERIFY(stockTrackingAction != nullptr);

        stockTrackingAction->trigger();
        QCOMPARE(apiManagerMock->productTypesRequestCount(), 1);
        QCOMPARE(apiManagerMock->stocksRequestCount(), 1);

        tabWidget->removeTab(tabWidget->currentIndex());
        QCOMPARE(tabWidget->count(), 0);

        stockTrackingAction->trigger();
        QCOMPARE(apiManagerMock->productTypesRequestCount(), 2);
        QCOMPARE(apiManagerMock->stocksRequestCount(), 2);
    }

    void testOpenUsersTabFetchesUsersAndRolesOnce()
    {
        auto *tabWidget = mainWindow->findChild<QTabWidget *>();
        QVERIFY(tabWidget != nullptr);
        const int initialTabCount = tabWidget->count();
        QCOMPARE(apiManagerMock->rolesRequestCount(), 0);
        QCOMPARE(apiManagerMock->usersRequestCount(), 0);

        QAction *usersAction =
            findMenuAction("Users", MainMenuActions::NAMES.at(MainMenuActions::Type::USERS));
        QVERIFY(usersAction != nullptr);

        usersAction->trigger();

        QCOMPARE(tabWidget->count(), initialTabCount + 1);
        QCOMPARE(apiManagerMock->rolesRequestCount(), 1);
        QCOMPARE(apiManagerMock->usersRequestCount(), 1);

        usersAction->trigger();
        QCOMPARE(tabWidget->count(), initialTabCount + 1);
        QCOMPARE(apiManagerMock->rolesRequestCount(), 1);
        QCOMPARE(apiManagerMock->usersRequestCount(), 1);
    }

    void testOpenUserRolesTabFetchesRolesOnce()
    {
        auto *tabWidget = mainWindow->findChild<QTabWidget *>();
        QVERIFY(tabWidget != nullptr);
        const int initialTabCount = tabWidget->count();
        QCOMPARE(apiManagerMock->rolesRequestCount(), 0);
        QCOMPARE(apiManagerMock->usersRequestCount(), 0);

        QAction *rolesAction =
            findMenuAction("Users", MainMenuActions::NAMES.at(MainMenuActions::Type::USER_ROLES));
        QVERIFY(rolesAction != nullptr);

        rolesAction->trigger();

        QCOMPARE(tabWidget->count(), initialTabCount + 1);
        QCOMPARE(tabWidget->tabText(tabWidget->currentIndex()),
                 MainMenuActions::NAMES.at(MainMenuActions::Type::USER_ROLES));
        QCOMPARE(apiManagerMock->rolesRequestCount(), 1);
        QCOMPARE(apiManagerMock->usersRequestCount(), 0);

        rolesAction->trigger();
        QCOMPARE(tabWidget->count(), initialTabCount + 1);
        QCOMPARE(apiManagerMock->rolesRequestCount(), 1);
    }

    void testPreviouslyUnsupportedMenuActionsNowOpenTabs()
    {
        auto *tabWidget = mainWindow->findChild<QTabWidget *>();
        QVERIFY(tabWidget != nullptr);
        QCOMPARE(tabWidget->count(), 0);

        const QVector<QPair<QString, MainMenuActions::Type>> actions = {
            {"File", MainMenuActions::Type::SETTINGS},
            {"Purchasing", MainMenuActions::Type::PURCHASE_ORDERS},
            {"Purchasing", MainMenuActions::Type::SUPPLIER_MANAGEMENT},
            {"Purchasing", MainMenuActions::Type::GOODS_RECEIPTS},
            {"Sales", MainMenuActions::Type::SALES_ORDERS},
            {"Sales", MainMenuActions::Type::CUSTOMER_MANAGEMENT},
            {"Sales", MainMenuActions::Type::INVOICING},
            {"Inventory", MainMenuActions::Type::SUPPLIER_PRICELIST_UPLOAD},
            {"Analytics", MainMenuActions::Type::SALES_ANALYTICS},
            {"Analytics", MainMenuActions::Type::INVENTORY_ANALYTICS},
            {"Users", MainMenuActions::Type::USER_ROLES},
            {"Users", MainMenuActions::Type::USER_LOGS},
            {"Management", MainMenuActions::Type::EMPLOYEES},
            {"Management", MainMenuActions::Type::CUSTOMERS},
            {"Management", MainMenuActions::Type::SUPPLIERS}};

        int expectedTabCount = 0;
        for (const auto &entry : actions) {
            const QString &menuTitle           = entry.first;
            const MainMenuActions::Type actionType = entry.second;
            QAction *action = findMenuAction(menuTitle, MainMenuActions::NAMES.at(actionType));
            QVERIFY(action != nullptr);

            action->trigger();
            ++expectedTabCount;

            QCOMPARE(tabWidget->count(), expectedTabCount);
            QCOMPARE(tabWidget->tabText(tabWidget->currentIndex()),
                     MainMenuActions::NAMES.at(actionType));
        }

        QCOMPARE(apiManagerMock->usersRequestCount(), 0);
        QCOMPARE(apiManagerMock->rolesRequestCount(), 1);
    }

private:
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
};

QTEST_MAIN(MainWindowTest)
#include "MainWindowTest.moc"
