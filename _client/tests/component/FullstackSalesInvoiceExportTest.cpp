#include <memory>

#include <QApplication>
#include <QElapsedTimer>
#include <QFile>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTabWidget>
#include <QTemporaryDir>
#include <QtTest>

#include "Network/ApiManager.hpp"
#include "Network/NetworkService.hpp"
#include "Ui/Models/SalesModel.hpp"
#include "Ui/ViewModels/SalesViewModel.hpp"
#include "Ui/Views/SalesView.hpp"
#include "common/Network/JsonSerializer.hpp"

#include "wrappers/FullstackServerHarness.hpp"

class FullstackSalesInvoiceExportTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase()
    {
        QString errorMessage;
        QVERIFY2(m_server.start(&errorMessage), qPrintable(errorMessage));

        m_networkService = std::make_unique<NetworkService>(
            m_server.serverConfig(), std::make_unique<JsonSerializer>());
        m_apiManager = std::make_unique<ApiManager>(*m_networkService);
        m_salesModel = std::make_unique<SalesModel>(*m_apiManager);
        m_viewModel = std::make_unique<SalesViewModel>(*m_salesModel);
        m_view = std::make_unique<SalesView>(*m_viewModel);

        m_networkService->init();
        m_view->show();
        m_viewModel->fetchAll();

        QVERIFY2(waitForTableRows("salesInvoiceOrdersTable", 1), qPrintable(m_server.logContents()));
    }

    void cleanupTestCase()
    {
        m_view.reset();
        m_viewModel.reset();
        m_salesModel.reset();
        m_apiManager.reset();
        m_networkService.reset();
        m_server.stop();
    }

    void testInvoicePreviewAndExportUseLiveServerData()
    {
        auto *tabs = m_view->findChild<QTabWidget *>("salesTabs");
        QVERIFY(tabs != nullptr);
        tabs->setCurrentIndex(1);

        auto *ordersTable = m_view->findChild<QTableWidget *>("salesInvoiceOrdersTable");
        QVERIFY(ordersTable != nullptr);
        QVERIFY(ordersTable->rowCount() >= 1);

        ordersTable->selectRow(0);
        QVERIFY(ordersTable->selectionModel() != nullptr);
        QVERIFY(ordersTable->selectionModel()->hasSelection());
        QVERIFY2(waitForTableRows("salesInvoiceRecordsTable", 1), qPrintable(m_server.logContents()));
        QVERIFY2(waitForPreviewText("SageStore Invoice"), qPrintable(m_server.logContents()));

        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        const QString exportPath = tempDir.filePath(QStringLiteral("invoice-export.txt"));
        m_view->setInvoiceExportPathProvider(
            [exportPath](QWidget *, const QString &, const QString &, const QString &) {
                return exportPath;
            });

        auto *exportButton = m_view->findChild<QPushButton *>("salesExportInvoiceButton");
        QVERIFY(exportButton != nullptr);
        QVERIFY(exportButton->isEnabled());
        QTest::mouseClick(exportButton, Qt::LeftButton);

        QFile exportedFile(exportPath);
        QVERIFY(exportedFile.exists());
        QVERIFY(exportedFile.open(QIODevice::ReadOnly | QIODevice::Text));

        const QString exportedText = QString::fromUtf8(exportedFile.readAll());
        const auto *preview = m_view->findChild<QPlainTextEdit *>("salesInvoicePreview");
        QVERIFY(preview != nullptr);
        QCOMPARE(exportedText, preview->toPlainText());
        QVERIFY(exportedText.contains(QStringLiteral("SageStore Invoice")));
        QVERIFY(exportedText.contains(QStringLiteral("Order ID")));
        QVERIFY(exportedText.contains(QStringLiteral("Total")));
    }

private:
    bool waitForTableRows(const char *objectName, int minimumRows, int timeoutMs = 5000) const
    {
        QElapsedTimer timer;
        timer.start();

        while (timer.elapsed() < timeoutMs) {
            if (auto *table = m_view->findChild<QTableWidget *>(objectName);
                table != nullptr && table->rowCount() >= minimumRows) {
                return true;
            }

            QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
            QTest::qWait(50);
        }

        return false;
    }

    bool waitForPreviewText(const QString &needle, int timeoutMs = 5000) const
    {
        QElapsedTimer timer;
        timer.start();

        while (timer.elapsed() < timeoutMs) {
            if (auto *preview = m_view->findChild<QPlainTextEdit *>("salesInvoicePreview");
                preview != nullptr && preview->toPlainText().contains(needle)) {
                return true;
            }

            QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
            QTest::qWait(50);
        }

        return false;
    }

    FullstackServerHarness m_server;
    std::unique_ptr<NetworkService> m_networkService;
    std::unique_ptr<ApiManager> m_apiManager;
    std::unique_ptr<SalesModel> m_salesModel;
    std::unique_ptr<SalesViewModel> m_viewModel;
    std::unique_ptr<SalesView> m_view;
};

QTEST_MAIN(FullstackSalesInvoiceExportTest)
#include "FullstackSalesInvoiceExportTest.moc"
