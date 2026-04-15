#include <QAction>
#include <QApplication>
#include <QElapsedTimer>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTest>
#include <QTimer>
#include <QWidget>

#include <functional>
#include <memory>
#include <utility>

#include "Network/ApiManager.hpp"
#include "Network/NetworkService.hpp"
#include "Ui/Dialogs/DialogManager.hpp"
#include "Ui/Dialogs/LoginDialog.hpp"
#include "Ui/MainMenuActions.hpp"
#include "Ui/MainWindow.hpp"
#include "Ui/StartupController.hpp"

#include "common/Network/JsonSerializer.hpp"
#include "wrappers/TableUiTestHelpers.hpp"
#include "wrappers/FullstackServerHarness.hpp"

class FullstackInventoryWorkflowTest : public QObject {
    Q_OBJECT

private slots:
    void init()
    {
        auto *application = qobject_cast<QApplication *>(QCoreApplication::instance());
        QVERIFY(application != nullptr);
        m_application = application;

        QString errorMessage;
        QVERIFY2(m_server.start(&errorMessage), qPrintable(errorMessage));
        m_networkService =
            std::make_unique<NetworkService>(m_server.serverConfig(),
                                             std::make_unique<JsonSerializer>());
        m_apiManager    = std::make_unique<ApiManager>(*m_networkService);
        m_dialogManager = std::make_unique<DialogManager>(*m_apiManager);
        m_dialogManager->init();
        m_networkService->init();

        connect(m_dialogManager.get(), &DialogManager::loginSucceeded, this, [this]() {
            QTimer::singleShot(0, m_dialogManager->messageDialog(), &QMessageBox::accept);
        });

        m_startupController = std::make_unique<StartupController>(
            *m_dialogManager, [this]() {
                m_mainWindow =
                    std::make_unique<MainWindow>(*m_application, *m_apiManager, *m_dialogManager);
                return m_mainWindow.get();
            });
    }

    void cleanup()
    {
        m_startupController.reset();
        m_mainWindow.reset();
        m_dialogManager.reset();
        m_apiManager.reset();
        m_networkService.reset();
        m_server.stop();
        m_application = nullptr;
    }

    void testLoginAndInventoryCrudAgainstRealServer()
    {
        m_startupController->start();

        auto *loginDialog = waitForLoginDialog();
        QVERIFY(loginDialog != nullptr);

        auto *usernameField = loginDialog->findChild<QLineEdit *>("loginUsernameField");
        auto *passwordField = loginDialog->findChild<QLineEdit *>("loginPasswordField");
        auto *loginButton   = loginDialog->findChild<QPushButton *>("loginSubmitButton");
        QVERIFY(usernameField != nullptr);
        QVERIFY(passwordField != nullptr);
        QVERIFY(loginButton != nullptr);
        QCOMPARE(usernameField->text(), QStringLiteral("admin"));
        QCOMPARE(passwordField->text(), QStringLiteral("admin123"));

        QSignalSpy loginSucceededSpy(m_dialogManager.get(), &DialogManager::loginSucceeded);
        QTest::mouseClick(loginButton, Qt::LeftButton);

        QTRY_COMPARE_WITH_TIMEOUT(loginSucceededSpy.count(), 1, 5000);
        QTRY_VERIFY_WITH_TIMEOUT(m_mainWindow != nullptr && m_mainWindow->isVisible(), 5000);

        QAction *productManagementAction =
            findMenuAction(MainMenuActions::Type::PRODUCT_MANAGEMENT);
        QVERIFY(productManagementAction != nullptr);
        productManagementAction->trigger();

        auto *addButton = waitForMainWindowChild<QPushButton *>("productTypesAddButton");
        auto *table     = waitForMainWindowChild<QTableWidget *>("productTypesTable");
        QVERIFY(addButton != nullptr);
        QVERIFY(table != nullptr);
        QTRY_VERIFY_WITH_TIMEOUT(table->rowCount() >= 1, 5000);

        const int initialRowCount = table->rowCount();
        const QString newCode = QStringLiteral("ERP-FULLSTACK-001");
        const QString newName = QStringLiteral("Brake Pad Set");

        QTest::mouseClick(addButton, Qt::LeftButton);

        QTRY_COMPARE_WITH_TIMEOUT(table->rowCount(), initialRowCount + 1, 5000);
        const int createdRow = initialRowCount;
        const QString createdId = table->item(createdRow, 0)->text();
        QVERIFY(!createdId.isEmpty());

        int persistedRow = waitForRowByValue(table, 0, createdId, 5000);
        QVERIFY(persistedRow >= 0);
        TableUiTestHelpers::editTextCell(table, persistedRow, 1, newCode);
        persistedRow = waitForRowByValue(table, 0, createdId, 5000);
        QVERIFY(persistedRow >= 0);
        TableUiTestHelpers::editTextCell(table, persistedRow, 2, newName);
        persistedRow = waitForRowByValue(table, 0, createdId, 5000);
        QVERIFY(persistedRow >= 0);
        TableUiTestHelpers::editTextCell(table, persistedRow, 3, QStringLiteral("482000000001"));
        persistedRow = waitForRowByValue(table, 0, createdId, 5000);
        QVERIFY(persistedRow >= 0);
        TableUiTestHelpers::editTextCell(table, persistedRow, 4, QStringLiteral("set"));
        persistedRow = waitForRowByValue(table, 0, createdId, 5000);
        QVERIFY(persistedRow >= 0);
        TableUiTestHelpers::editTextCell(table, persistedRow, 5, QStringLiteral("99.95"));

        persistedRow = waitForRowByValue(table, 0, createdId, 5000);
        QVERIFY(persistedRow >= 0);
        QCOMPARE(table->item(persistedRow, 1)->text(), newCode);
        QCOMPARE(table->item(persistedRow, 2)->text(), newName);
        QCOMPARE(table->item(persistedRow, 3)->text(), QString("482000000001"));
        QCOMPARE(table->item(persistedRow, 4)->text(), QString("set"));
        QCOMPARE(table->item(persistedRow, 5)->text(), QString("99.95"));

        table->selectRow(persistedRow);
        auto *deleteButton = waitForMainWindowChild<QPushButton *>("productTypesDeleteButton");
        QVERIFY(deleteButton != nullptr);
        QTest::mouseClick(deleteButton, Qt::LeftButton);

        QTRY_COMPARE_WITH_TIMEOUT(table->rowCount(), initialRowCount, 5000);
        QCOMPARE(findRowByValue(table, 1, newCode), -1);
    }

private:
    LoginDialog *waitForLoginDialog(int timeoutMs = 5000) const
    {
        QElapsedTimer timer;
        timer.start();

        while (timer.elapsed() < timeoutMs) {
            for (QWidget *widget : QApplication::topLevelWidgets()) {
                auto *dialog = qobject_cast<LoginDialog *>(widget);
                if (dialog != nullptr && dialog->objectName() == QStringLiteral("loginDialog") &&
                    dialog->isVisible()) {
                    return dialog;
                }
            }

            QTest::qWait(50);
        }

        return nullptr;
    }

    template <typename WidgetType>
    WidgetType waitForMainWindowChild(const char *objectName, int timeoutMs = 5000) const
    {
        QElapsedTimer timer;
        timer.start();

        while (timer.elapsed() < timeoutMs) {
            if (m_mainWindow != nullptr) {
                if (WidgetType widget = m_mainWindow->findChild<WidgetType>(objectName)) {
                    return widget;
                }
            }

            QTest::qWait(50);
        }

        return nullptr;
    }

    QAction *findMenuAction(MainMenuActions::Type actionType) const
    {
        return m_mainWindow != nullptr
                   ? m_mainWindow->findChild<QAction *>(MainMenuActions::objectName(actionType))
                   : nullptr;
    }

    static int findRowByValue(QTableWidget *table, int column, const QString &value)
    {
        if (table == nullptr) {
            return -1;
        }

        for (int row = 0; row < table->rowCount(); ++row) {
            auto *item = table->item(row, column);
            if (item != nullptr && item->text() == value) {
                return row;
            }
        }

        return -1;
    }

    static int waitForRowByValue(QTableWidget *table, int column, const QString &value,
                                 int timeoutMs)
    {
        QElapsedTimer timer;
        timer.start();

        while (timer.elapsed() < timeoutMs) {
            const int row = findRowByValue(table, column, value);
            if (row >= 0) {
                return row;
            }

            QTest::qWait(50);
        }

        return -1;
    }

    QApplication *m_application{nullptr};
    FullstackServerHarness m_server;
    std::unique_ptr<NetworkService> m_networkService;
    std::unique_ptr<ApiManager> m_apiManager;
    std::unique_ptr<DialogManager> m_dialogManager;
    std::unique_ptr<StartupController> m_startupController;
    std::unique_ptr<MainWindow> m_mainWindow;
};

QTEST_MAIN(FullstackInventoryWorkflowTest)
#include "FullstackInventoryWorkflowTest.moc"
