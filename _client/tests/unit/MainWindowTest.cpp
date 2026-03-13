#include <QApplication>
#include <QComboBox>
#include <QSet>
#include <QSignalSpy>
#include <QTabWidget>
#include <QtTest>

#include "Ui/MainWindow.hpp"
#include "Ui/MainMenuActions.hpp"
#include "Ui/Views/SettingsView.hpp"

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

    void testOpenProductManagementTabFromInventoryMenu()
    {
        auto *tabWidget = mainWindow->findChild<QTabWidget *>();
        QVERIFY(tabWidget != nullptr);
        QCOMPARE(tabWidget->count(), 0);
        QCOMPARE(apiManagerMock->productTypesRequestCount(), 0);

        QAction *productManagementAction =
            findMenuAction("Inventory",
                           MainMenuActions::displayName(MainMenuActions::Type::PRODUCT_MANAGEMENT));
        QVERIFY(productManagementAction != nullptr);

        productManagementAction->trigger();

        QCOMPARE(tabWidget->count(), 1);
        QCOMPARE(tabWidget->tabText(0),
                 MainMenuActions::displayName(MainMenuActions::Type::PRODUCT_MANAGEMENT));
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
                           MainMenuActions::displayName(MainMenuActions::Type::STOCK_TRACKING));
        QVERIFY(stockTrackingAction != nullptr);

        stockTrackingAction->trigger();

        QCOMPARE(tabWidget->count(), initialTabCount + 1);
        QCOMPARE(tabWidget->tabText(tabWidget->currentIndex()),
                 MainMenuActions::displayName(MainMenuActions::Type::STOCK_TRACKING));
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
                           MainMenuActions::displayName(MainMenuActions::Type::STOCK_TRACKING));
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
            findMenuAction("Users", MainMenuActions::displayName(MainMenuActions::Type::USERS));
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
            findMenuAction("Users", MainMenuActions::displayName(MainMenuActions::Type::USER_ROLES));
        QVERIFY(rolesAction != nullptr);

        rolesAction->trigger();

        QCOMPARE(tabWidget->count(), initialTabCount + 1);
        QCOMPARE(tabWidget->tabText(tabWidget->currentIndex()),
                 MainMenuActions::displayName(MainMenuActions::Type::USER_ROLES));
        QCOMPARE(apiManagerMock->rolesRequestCount(), 1);
        QCOMPARE(apiManagerMock->usersRequestCount(), 0);

        rolesAction->trigger();
        QCOMPARE(tabWidget->count(), initialTabCount + 1);
        QCOMPARE(apiManagerMock->rolesRequestCount(), 1);
    }

    void testOpenSettingsTabUsesRealSettingsView()
    {
        auto *tabWidget = mainWindow->findChild<QTabWidget *>();
        QVERIFY(tabWidget != nullptr);
        QCOMPARE(tabWidget->count(), 0);

        QAction *settingsAction =
            findMenuAction("File", MainMenuActions::displayName(MainMenuActions::Type::SETTINGS));
        QVERIFY(settingsAction != nullptr);

        settingsAction->trigger();

        QCOMPARE(tabWidget->count(), 1);
        QCOMPARE(tabWidget->tabText(0),
                 MainMenuActions::displayName(MainMenuActions::Type::SETTINGS));
        QVERIFY(tabWidget->widget(0)->findChild<QComboBox *>("settingsLanguageCombo") != nullptr);
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

        auto expectedTabTitle = [](MainMenuActions::Type actionType) {
            switch (actionType) {
            case MainMenuActions::Type::PURCHASE_ORDERS:
            case MainMenuActions::Type::GOODS_RECEIPTS:
                return QStringLiteral("Purchasing");
            case MainMenuActions::Type::SUPPLIER_MANAGEMENT:
            case MainMenuActions::Type::CUSTOMER_MANAGEMENT:
            case MainMenuActions::Type::EMPLOYEES:
            case MainMenuActions::Type::CUSTOMERS:
            case MainMenuActions::Type::SUPPLIERS:
                return QStringLiteral("Management");
            case MainMenuActions::Type::SALES_ORDERS:
            case MainMenuActions::Type::INVOICING:
                return QStringLiteral("Sales");
            case MainMenuActions::Type::SALES_ANALYTICS:
            case MainMenuActions::Type::INVENTORY_ANALYTICS:
                return QStringLiteral("Analytics");
            default:
                return MainMenuActions::displayName(actionType);
            }
        };

        QSet<QString> expectedOpenTabs;
        for (const auto &entry : actions) {
            const QString &menuTitle = entry.first;
            const MainMenuActions::Type actionType = entry.second;
            QAction *action = findMenuAction(menuTitle, MainMenuActions::displayName(actionType));
            QVERIFY(action != nullptr);

            action->trigger();
            expectedOpenTabs.insert(expectedTabTitle(actionType));

            QCOMPARE(tabWidget->count(), expectedOpenTabs.size());
            QCOMPARE(tabWidget->tabText(tabWidget->currentIndex()), expectedTabTitle(actionType));
        }

        QCOMPARE(apiManagerMock->usersRequestCount(), 3);
        QCOMPARE(apiManagerMock->rolesRequestCount(), 1);
        QCOMPARE(apiManagerMock->productTypesRequestCount(), 3);
        QCOMPARE(apiManagerMock->contactsRequestCount(), 2);
        QCOMPARE(apiManagerMock->suppliersRequestCount(), 3);
        QCOMPARE(apiManagerMock->employeesRequestCount(), 3);
        QCOMPARE(apiManagerMock->supplierProductsRequestCount(), 1);
        QCOMPARE(apiManagerMock->purchaseOrdersRequestCount(), 1);
        QCOMPARE(apiManagerMock->salesOrdersRequestCount(), 1);
        QCOMPARE(apiManagerMock->logsRequestCount(), 1);
        QCOMPARE(apiManagerMock->salesAnalyticsRequestCount(), 1);
        QCOMPARE(apiManagerMock->inventoryAnalyticsRequestCount(), 1);
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
