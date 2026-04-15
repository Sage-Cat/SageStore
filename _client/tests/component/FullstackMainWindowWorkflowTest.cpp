#include <memory>

#include <QAction>
#include <QApplication>
#include <QTableWidget>
#include <QtTest>

#include "Network/ApiManager.hpp"
#include "Network/NetworkService.hpp"
#include "Ui/Dialogs/DialogManager.hpp"
#include "Ui/MainMenuActions.hpp"
#include "Ui/MainWindow.hpp"
#include "common/Network/JsonSerializer.hpp"

#include "wrappers/FullstackServerHarness.hpp"

class FullstackMainWindowWorkflowTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m_app = qobject_cast<QApplication *>(QCoreApplication::instance());
        QVERIFY(m_app != nullptr);

        QString errorMessage;
        QVERIFY2(m_server.start(&errorMessage), qPrintable(errorMessage));

        m_networkService = std::make_unique<NetworkService>(
            m_server.serverConfig(), std::make_unique<JsonSerializer>());
        m_apiManager = std::make_unique<ApiManager>(*m_networkService);
        m_dialogManager = std::make_unique<DialogManager>(*m_apiManager);
        m_networkService->init();
        m_dialogManager->init();

        m_mainWindow =
            std::make_unique<MainWindow>(*m_app, *m_apiManager, *m_dialogManager);
        m_mainWindow->show();
        QTest::qWait(100);
    }

    void cleanupTestCase()
    {
        m_mainWindow.reset();
        m_dialogManager.reset();
        m_apiManager.reset();
        m_networkService.reset();
        m_server.stop();
    }

    void testInventoryAndCatalogWorkspacesLoadFromLiveServer()
    {
        triggerAction(MainMenuActions::Type::PRODUCT_MANAGEMENT);
        QVERIFY2(waitForTableRows("productTypesTable", 1), qPrintable(m_server.logContents()));

        triggerAction(MainMenuActions::Type::STOCK_TRACKING);
        QVERIFY2(waitForTableRows("stocksTable", 1), qPrintable(m_server.logContents()));
    }

    void testManagementPurchaseAndSalesWorkspacesLoadFromLiveServer()
    {
        triggerAction(MainMenuActions::Type::CUSTOMERS);
        QVERIFY2(waitForTableRows("managementContactsTable", 1),
                 qPrintable(m_server.logContents()));
        QVERIFY2(waitForTableRows("managementSuppliersTable", 1),
                 qPrintable(m_server.logContents()));
        QVERIFY2(waitForTableRows("managementEmployeesTable", 1),
                 qPrintable(m_server.logContents()));

        triggerAction(MainMenuActions::Type::PURCHASE_ORDERS);
        QVERIFY2(waitForTableRows("purchaseOrdersTable", 1), qPrintable(m_server.logContents()));
        QVERIFY2(waitForTableRows("purchaseReceiptOrdersTable", 1),
                 qPrintable(m_server.logContents()));

        triggerAction(MainMenuActions::Type::SALES_ORDERS);
        QVERIFY2(waitForTableRows("salesOrdersTable", 1), qPrintable(m_server.logContents()));
        QVERIFY2(waitForTableRows("salesInvoiceOrdersTable", 1),
                 qPrintable(m_server.logContents()));
    }

    void testUsersAndAnalyticsWorkspacesLoadFromLiveServer()
    {
        triggerAction(MainMenuActions::Type::USERS);
        QVERIFY2(waitForTableRows("usersTable", 1), qPrintable(m_server.logContents()));

        triggerAction(MainMenuActions::Type::USER_ROLES);
        QVERIFY2(waitForTableRows("rolesTable", 2), qPrintable(m_server.logContents()));

        triggerAction(MainMenuActions::Type::SALES_ANALYTICS);
        QVERIFY2(waitForTableRows("analyticsSalesTable", 1),
                 qPrintable(m_server.logContents()));
        QVERIFY2(waitForTableRows("analyticsInventoryTable", 1),
                 qPrintable(m_server.logContents()));
    }

private:
    QAction *findMenuAction(MainMenuActions::Type actionType) const
    {
        return m_mainWindow->findChild<QAction *>(MainMenuActions::objectName(actionType));
    }

    void triggerAction(MainMenuActions::Type actionType) const
    {
        QAction *action = findMenuAction(actionType);
        QVERIFY(action != nullptr);
        action->trigger();
        QCoreApplication::processEvents();
    }

    bool waitForTableRows(const char *objectName, int minimumRows, int timeoutMs = 5000) const
    {
        QElapsedTimer timer;
        timer.start();

        while (timer.elapsed() < timeoutMs) {
            if (auto *table = m_mainWindow->findChild<QTableWidget *>(objectName);
                table != nullptr && table->rowCount() >= minimumRows) {
                return true;
            }

            QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
            QTest::qWait(50);
        }

        return false;
    }

    QApplication *m_app{nullptr};
    FullstackServerHarness m_server;
    std::unique_ptr<NetworkService> m_networkService;
    std::unique_ptr<ApiManager> m_apiManager;
    std::unique_ptr<DialogManager> m_dialogManager;
    std::unique_ptr<MainWindow> m_mainWindow;
};

QTEST_MAIN(FullstackMainWindowWorkflowTest)
#include "FullstackMainWindowWorkflowTest.moc"
