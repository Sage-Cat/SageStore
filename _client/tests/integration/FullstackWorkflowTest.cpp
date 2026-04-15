#include <functional>
#include <memory>

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QElapsedTimer>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QtTest>

#include "Network/ApiManager.hpp"
#include "Network/NetworkService.hpp"

#include "Ui/Dialogs/DialogManager.hpp"
#include "Ui/MainMenuActions.hpp"
#include "Ui/MainWindow.hpp"

#include "common/Network/JsonSerializer.hpp"
#include "wrappers/TableUiTestHelpers.hpp"
#include "wrappers/FullstackServerHarness.hpp"

class FullstackWorkflowTest : public QObject {
    Q_OBJECT

private slots:
    void init()
    {
        QString errorMessage;
        QVERIFY2(m_server.start(&errorMessage), qPrintable(errorMessage));
        startClient();
    }

    void cleanup()
    {
        m_mainWindow.reset();
        m_dialogManager.reset();
        m_apiManager.reset();
        m_networkService.reset();
        m_server.stop();
    }

    void testInventoryProductTypeCreatePersistsThroughRealServer()
    {
        triggerMenuAction(MainMenuActions::Type::PRODUCT_MANAGEMENT);

        auto *table = waitForChild<QTableWidget>("productTypesTable");
        auto *addButton = waitForChild<QPushButton>("productTypesAddButton");
        QTRY_COMPARE(table->rowCount(), 1);
        QVERIFY(addButton != nullptr);
        QTest::mouseClick(addButton, Qt::LeftButton);
        QTRY_COMPARE(table->rowCount(), 2);
        const QString createdId = table->item(1, 0)->text();
        QVERIFY(!createdId.isEmpty());

        int row = waitForRowByText(table, 0, createdId);
        QVERIFY(row >= 0);
        TableUiTestHelpers::editTextCell(table, row, 1, QStringLiteral("PT-FS-001"));
        row = waitForRowByText(table, 0, createdId);
        QVERIFY(row >= 0);
        TableUiTestHelpers::editTextCell(table, row, 2, QStringLiteral("Brake pad set"));
        row = waitForRowByText(table, 0, createdId);
        QVERIFY(row >= 0);
        TableUiTestHelpers::editTextCell(table, row, 3, QStringLiteral("482000000001"));
        row = waitForRowByText(table, 0, createdId);
        QVERIFY(row >= 0);
        TableUiTestHelpers::editTextCell(table, row, 4, QStringLiteral("set"));
        row = waitForRowByText(table, 0, createdId);
        QVERIFY(row >= 0);
        TableUiTestHelpers::editTextCell(table, row, 5, QStringLiteral("45.50"));
        row = waitForRowByText(table, 0, createdId);
        QVERIFY(row >= 0);
        TableUiTestHelpers::editTextCell(table, row, 7,
                                         QStringLiteral("Front axle brake pad set"));
        row = waitForRowByText(table, 0, createdId);
        QVERIFY(row >= 0);
        QTRY_COMPARE(table->item(row, 1)->text(), QString("PT-FS-001"));
        QCOMPARE(table->item(row, 2)->text(), QString("Brake pad set"));
        QCOMPARE(table->item(row, 3)->text(), QString("482000000001"));
        QCOMPARE(table->item(row, 4)->text(), QString("set"));
        QCOMPARE(table->item(row, 5)->text(), QString("45.50"));
        QCOMPARE(table->item(row, 7)->text(), QString("Front axle brake pad set"));
    }

    void testManagementContactCreatePersistsThroughRealServer()
    {
        triggerMenuAction(MainMenuActions::Type::CUSTOMERS);

        auto *contactsTable = waitForChild<QTableWidget>("managementContactsTable");
        auto *addButton = waitForChild<QPushButton>("managementContactsAddButton");
        QTRY_COMPARE(contactsTable->rowCount(), 1);
        QVERIFY(addButton != nullptr);
        QTest::mouseClick(addButton, Qt::LeftButton);
        QTRY_COMPARE(contactsTable->rowCount(), 2);
        TableUiTestHelpers::editTextCell(contactsTable, 1, 1,
                                         QStringLiteral("Fleet Customer"));
        TableUiTestHelpers::setComboCellByText(contactsTable, 1, 2,
                                               QStringLiteral("Customer"));
        TableUiTestHelpers::editTextCell(contactsTable, 1, 3,
                                         QStringLiteral("fleet@example.com"));
        TableUiTestHelpers::editTextCell(contactsTable, 1, 4,
                                         QStringLiteral("+380000000001"));
        QVERIFY(findRowByText(contactsTable, 1, "Fleet Customer") >= 0);
    }

    void testPurchaseReceiptPostingUpdatesOrderAndStockViews()
    {
        triggerMenuAction(MainMenuActions::Type::GOODS_RECEIPTS);

        auto *receiptOrdersTable = waitForChild<QTableWidget>("purchaseReceiptOrdersTable");
        auto *employeeBox = waitForChild<QComboBox>("purchaseReceiptEmployeeBox");
        auto *receiveButton = waitForChild<QPushButton>("purchaseReceiveButton");

        QTRY_COMPARE(receiptOrdersTable->rowCount(), 1);
        QTRY_VERIFY(employeeBox->count() >= 1);

        employeeBox->setCurrentIndex(0);
        receiptOrdersTable->selectRow(0);
        QTest::mouseClick(receiveButton, Qt::LeftButton);

        QTRY_COMPARE(receiptOrdersTable->rowCount(), 0);

        triggerMenuAction(MainMenuActions::Type::PURCHASE_ORDERS);
        auto *ordersTable = waitForChild<QTableWidget>("purchaseOrdersTable");
        QTRY_COMPARE(ordersTable->rowCount(), 1);
        QTRY_COMPARE(ordersTable->item(0, 6)->data(Qt::UserRole).toString(),
                     QString("Received"));

        triggerMenuAction(MainMenuActions::Type::STOCK_TRACKING);
        auto *stocksTable = waitForChild<QTableWidget>("stocksTable");
        QTRY_COMPARE(stocksTable->rowCount(), 1);
        QTRY_COMPARE(stocksTable->item(0, 3)->text(), QString("1"));
    }

    void testSalesInvoicePreviewLoadsThroughRealServer()
    {
        triggerMenuAction(MainMenuActions::Type::INVOICING);

        auto *invoiceOrdersTable = waitForChild<QTableWidget>("salesInvoiceOrdersTable");
        auto *invoicePreview = waitForChild<QPlainTextEdit>("salesInvoicePreview");

        QTRY_COMPARE(invoiceOrdersTable->rowCount(), 1);
        invoiceOrdersTable->selectRow(0);

        QTRY_VERIFY(invoicePreview->toPlainText().contains("SageStore Invoice"));
        QVERIFY(invoicePreview->toPlainText().contains("Order ID: 1"));
        QVERIFY(invoicePreview->toPlainText().contains("Total: 10.99"));
    }

private:
    void startClient()
    {
        auto *application = qobject_cast<QApplication *>(QCoreApplication::instance());
        QVERIFY(application != nullptr);

        m_networkService =
            std::make_unique<NetworkService>(m_server.serverConfig(),
                                             std::make_unique<JsonSerializer>());
        m_apiManager = std::make_unique<ApiManager>(*m_networkService);
        m_networkService->init();

        m_dialogManager = std::make_unique<DialogManager>(*m_apiManager);
        m_dialogManager->init();

        m_mainWindow = std::make_unique<MainWindow>(*application, *m_apiManager, *m_dialogManager);
        m_mainWindow->show();
    }

    QAction *findMenuAction(MainMenuActions::Type actionType) const
    {
        return m_mainWindow->findChild<QAction *>(MainMenuActions::objectName(actionType));
    }

    void triggerMenuAction(MainMenuActions::Type actionType)
    {
        QAction *action = findMenuAction(actionType);
        QVERIFY(action != nullptr);
        action->trigger();
    }

    template <typename T>
    T *waitForChild(const char *objectName) const
    {
        QElapsedTimer timer;
        timer.start();

        while (timer.elapsed() < 5000) {
            if (auto *child = m_mainWindow->findChild<T *>(objectName); child != nullptr) {
                return child;
            }

            QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
            QTest::qWait(50);
        }

        return nullptr;
    }

    int findRowByText(QTableWidget *table, int column, const QString &text) const
    {
        for (int row = 0; row < table->rowCount(); ++row) {
            auto *item = table->item(row, column);
            if (item != nullptr && item->text() == text) {
                return row;
            }
        }

        return -1;
    }

    int waitForRowByText(QTableWidget *table, int column, const QString &text,
                         int timeoutMs = 5000) const
    {
        QElapsedTimer timer;
        timer.start();

        while (timer.elapsed() < timeoutMs) {
            const int row = findRowByText(table, column, text);
            if (row >= 0) {
                return row;
            }

            QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
            QTest::qWait(50);
        }

        return -1;
    }

    FullstackServerHarness m_server;
    std::unique_ptr<NetworkService> m_networkService;
    std::unique_ptr<ApiManager> m_apiManager;
    std::unique_ptr<DialogManager> m_dialogManager;
    std::unique_ptr<MainWindow> m_mainWindow;
};

QTEST_MAIN(FullstackWorkflowTest)
#include "FullstackWorkflowTest.moc"
