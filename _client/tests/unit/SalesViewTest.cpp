#include <QDate>
#include <QComboBox>
#include <QFile>
#include <QFrame>
#include <QListView>
#include <QPlainTextEdit>
#include <QPointer>
#include <QPushButton>
#include <QStyleOptionComboBox>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QtTest>

#include "Ui/Models/SalesModel.hpp"
#include "Ui/ViewModels/SalesViewModel.hpp"
#include "Ui/Views/SalesView.hpp"

#include "mocks/ApiManagerMock.hpp"
#include "wrappers/TableUiTestHelpers.hpp"

class SalesViewTest : public QObject {
    Q_OBJECT

private slots:
    void init()
    {
        apiManagerMock = new ApiManagerMock();
        apiManagerMock->setSalesOrders(
            {Common::Entities::SaleOrder{.id = "1",
                                         .date = "2026-03-13",
                                         .userId = "1",
                                         .contactId = "1",
                                         .employeeId = "1",
                                         .status = "Pending"}});
        salesModel = new SalesModel(*apiManagerMock);
        viewModel  = new SalesViewModel(*salesModel);
        view       = new SalesView(*viewModel);
        view->show();
        viewModel->fetchAll();
        QTRY_COMPARE(ordersTable()->rowCount(), 1);
    }

    void cleanup()
    {
        delete view;
        delete viewModel;
        delete salesModel;
        delete apiManagerMock;
    }

    void testEditOrderPreservesUnknownStatus()
    {
        ordersTable()->selectRow(0);
        QVERIFY(ordersTable()->selectionModel()->hasSelection());

        QVERIFY(ordersTable()->isPersistentEditorOpen(ordersTable()->item(0, 8)));
        auto *statusCombo = TableUiTestHelpers::comboCell(ordersTable(), 0, 8);
        QVERIFY(statusCombo->view() == nullptr || !statusCombo->view()->isVisible());
        QCOMPARE(statusCombo->currentData().toString(), QString("Pending"));
        QCOMPARE(statusCombo->currentText(), QString("Pending"));

        QTRY_COMPARE(ordersTable()->item(0, 8)->data(Qt::UserRole).toString(), QString("Pending"));
        QCOMPARE(ordersTable()->item(0, 8)->text(), QString("Pending"));
    }

    void testOrdersTableInlineStatusComboPreservesUnknownStatus()
    {
        auto *statusCombo = TableUiTestHelpers::comboCell(ordersTable(), 0, 8);
        QVERIFY(statusCombo != nullptr);
        QCOMPARE(statusCombo->currentData().toString(), QString("Pending"));

        TableUiTestHelpers::setComboCellByText(ordersTable(), 0, 8, QStringLiteral("Confirmed"));
        QTRY_COMPARE(ordersTable()->item(0, 8)->data(Qt::UserRole).toString(),
                     QString("Confirmed"));
        QCOMPARE(ordersTable()->item(0, 8)->text(), QString("Confirmed"));
    }

    void testUnchangedStatusSelectionDoesNotPersist()
    {
        const int editCountBefore = apiManagerMock->salesOrderEditCount();
        TableUiTestHelpers::setComboCellByText(ordersTable(), 0, 8, QStringLiteral("Pending"));

        QTRY_COMPARE(apiManagerMock->salesOrderEditCount(), editCountBefore);
        QCOMPARE(ordersTable()->item(0, 8)->data(Qt::UserRole).toString(), QString("Pending"));
    }

    void testSelectingOrderDoesNotTriggerBackgroundEditLoop()
    {
        QTest::qWait(25);

        const int editCountBefore = apiManagerMock->salesOrderEditCount();
        const int recordRequestsBefore = apiManagerMock->salesOrderRecordsRequestCount();

        ordersTable()->selectRow(0);
        QTRY_COMPARE(orderRecordsTable()->rowCount(), 1);

        QTest::qWait(25);
        const int recordRequestsAfterSelection = apiManagerMock->salesOrderRecordsRequestCount();

        QCOMPARE(apiManagerMock->salesOrderEditCount(), editCountBefore);
        QVERIFY(recordRequestsAfterSelection == recordRequestsBefore ||
                recordRequestsAfterSelection == recordRequestsBefore + 1);

        QTest::qWait(25);
        QCOMPARE(apiManagerMock->salesOrderEditCount(), editCountBefore);
        QCOMPARE(apiManagerMock->salesOrderRecordsRequestCount(), recordRequestsAfterSelection);
    }

    void testSelectingInvoiceDoesNotTriggerBackgroundEditLoop()
    {
        QTest::qWait(25);

        const int editCountBefore = apiManagerMock->salesOrderEditCount();
        const int recordRequestsBefore = apiManagerMock->salesOrderRecordsRequestCount();

        invoiceOrdersTable()->selectRow(0);
        QTRY_VERIFY(invoicePreview()->toPlainText().contains("SageStore Invoice"));

        QTest::qWait(25);
        const int recordRequestsAfterSelection = apiManagerMock->salesOrderRecordsRequestCount();

        QCOMPARE(apiManagerMock->salesOrderEditCount(), editCountBefore);
        QVERIFY(recordRequestsAfterSelection == recordRequestsBefore ||
                recordRequestsAfterSelection == recordRequestsBefore + 1);

        QTest::qWait(25);
        QCOMPARE(apiManagerMock->salesOrderEditCount(), editCountBefore);
        QCOMPARE(apiManagerMock->salesOrderRecordsRequestCount(), recordRequestsAfterSelection);
    }

    void testAddOrderUsesDefaultInlineValues()
    {
        QTest::mouseClick(addOrderButton(), Qt::LeftButton);

        QTRY_COMPARE(ordersTable()->rowCount(), 2);
        const int row = ordersTable()->rowCount() - 1;
        QCOMPARE(ordersTable()->item(row, 1)->text(), QDate::currentDate().toString("yyyy-MM-dd"));
        QCOMPARE(ordersTable()->item(row, 3)->text(), QString("admin"));
        QCOMPARE(ordersTable()->item(row, 5)->text(), QString("Default Contact"));
        QCOMPARE(ordersTable()->item(row, 7)->text(), QString("John Doe"));
        QCOMPARE(ordersTable()->item(row, 8)->data(Qt::UserRole).toString(), QString("Draft"));
    }

    void testOrderRecordButtonsFollowSelection()
    {
        ordersTable()->selectRow(0);
        QTRY_COMPARE(orderRecordsTable()->rowCount(), 1);
        QVERIFY(addRecordButton()->isEnabled());
        QVERIFY(!deleteRecordButton()->isEnabled());

        orderRecordsTable()->selectRow(0);

        QVERIFY(deleteRecordButton()->isEnabled());
    }

    void testOrderDateEditsInlineFromOrdersTable()
    {
        ordersTable()->selectRow(0);
        TableUiTestHelpers::editTextCell(ordersTable(), 0, 1, "2026-04-02");
        QTRY_COMPARE(ordersTable()->item(0, 1)->text(), QString("2026-04-02"));
    }

    void testInlineEditorsStayWithinSalesCellGeometry()
    {
        auto *editor = TableUiTestHelpers::activateEditor(ordersTable(), 0, 1);
        QVERIFY(editor != nullptr);

        const QRect cellRect = ordersTable()->visualItemRect(ordersTable()->item(0, 1));
        const QRect editorRect = editor->geometry();
        QVERIFY(cellRect.adjusted(-1, -1, 1, 1).contains(editorRect.topLeft()));
        QVERIFY(cellRect.adjusted(-1, -1, 1, 1).contains(editorRect.bottomRight()));
    }

    void testStatusComboEditorStaysWithinSalesCellGeometry()
    {
        auto *editor = TableUiTestHelpers::activateEditor(ordersTable(), 0, 8);
        QVERIFY(editor != nullptr);

        const QRect cellRect = ordersTable()->visualItemRect(ordersTable()->item(0, 8));
        const QRect editorRect = editor->geometry();
        QVERIFY(cellRect.adjusted(-1, -1, 1, 1).contains(editorRect.topLeft()));
        QVERIFY(cellRect.adjusted(-1, -1, 1, 1).contains(editorRect.bottomRight()));
    }

    void testInlineStatusComboUsesInsetFramelessChrome()
    {
        auto *statusCombo = TableUiTestHelpers::comboCell(ordersTable(), 0, 8);
        QVERIFY(statusCombo != nullptr);
        QVERIFY(!statusCombo->hasFrame());
        QVERIFY(statusCombo->testAttribute(Qt::WA_StyledBackground));

        const QRect cellRect = ordersTable()->visualItemRect(ordersTable()->item(0, 8));
        const QRect comboRect = statusCombo->geometry();
        QVERIFY(comboRect.left() > cellRect.left());
        QVERIFY(comboRect.right() < cellRect.right());
        QVERIFY(comboRect.top() > cellRect.top());
        QVERIFY(comboRect.bottom() < cellRect.bottom());
    }

    void testInlineStatusComboUsesDedicatedPopupListView()
    {
        auto *statusCombo = TableUiTestHelpers::comboCell(ordersTable(), 0, 8);
        QVERIFY(statusCombo != nullptr);

        QStyleOptionComboBox option;
        option.initFrom(statusCombo);
        option.editable = statusCombo->isEditable();
        QCOMPARE(statusCombo->style()->styleHint(QStyle::SH_ComboBox_Popup, &option, statusCombo),
                 0);

        auto *popupView = qobject_cast<QListView *>(statusCombo->view());
        QVERIFY(popupView != nullptr);
        QVERIFY(popupView->property("tableComboPopup").toBool());
        QCOMPARE(popupView->horizontalScrollBarPolicy(), Qt::ScrollBarAlwaysOff);

        QWidget *popupContainer = TableUiTestHelpers::showComboPopup(statusCombo);
        QVERIFY(popupContainer != nullptr);
        QVERIFY(popupContainer->property("tableComboPopupContainer").toBool());
        auto *popupFrame = qobject_cast<QFrame *>(popupContainer);
        QVERIFY(popupFrame != nullptr);
        QCOMPARE(popupFrame->frameShape(), QFrame::NoFrame);

        int visibleScrollerCount = 0;
        for (auto *child : popupContainer->findChildren<QWidget *>()) {
            if (QString::fromLatin1(child->metaObject()->className()) ==
                    QStringLiteral("QComboBoxPrivateScroller") &&
                child->isVisible()) {
                ++visibleScrollerCount;
            }
        }
        QCOMPARE(visibleScrollerCount, 0);
        statusCombo->hidePopup();
    }

    void testCustomerComboPopupRendersWithoutDarkOverlay()
    {
        auto *customerCombo = TableUiTestHelpers::comboCell(ordersTable(), 0, 5);
        QVERIFY(customerCombo != nullptr);

        QWidget *popupContainer = TableUiTestHelpers::showComboPopup(customerCombo);
        QVERIFY(popupContainer != nullptr);
        QVERIFY(popupContainer->property("tableComboPopupContainer").toBool());
        QCOMPARE(TableUiTestHelpers::visibleComboPopupContainers().size(), 1);

        const QImage popupImage = TableUiTestHelpers::grabWidgetImage(popupContainer);
        QVERIFY(!popupImage.isNull());
        QVERIFY(popupImage.width() >= customerCombo->width());
        QVERIFY(popupImage.height() > 8);

        const double darkRatio = TableUiTestHelpers::darkPixelRatio(popupImage);
        QVERIFY2(darkRatio < 0.18,
                 qPrintable(QStringLiteral("Popup image dark ratio too high: %1")
                                .arg(QString::number(darkRatio, 'f', 3))));

        TableUiTestHelpers::hideComboPopup(customerCombo);
        QTRY_VERIFY(TableUiTestHelpers::visibleComboPopupContainers().isEmpty());
    }

    void testSwitchingBetweenSalesCombosLeavesSingleVisiblePopup()
    {
        QTest::mouseClick(addOrderButton(), Qt::LeftButton);
        QTRY_COMPARE(ordersTable()->rowCount(), 2);

        auto *customerCombo = TableUiTestHelpers::comboCell(ordersTable(), 0, 5);
        auto *statusCombo = TableUiTestHelpers::comboCell(ordersTable(), 1, 8);
        QVERIFY(customerCombo != nullptr);
        QVERIFY(statusCombo != nullptr);

        QPointer<QWidget> firstPopup = TableUiTestHelpers::showComboPopup(customerCombo);
        QVERIFY(firstPopup != nullptr);
        QCOMPARE(TableUiTestHelpers::visibleComboPopupContainers().size(), 1);

        QPointer<QWidget> secondPopup = TableUiTestHelpers::showComboPopup(statusCombo);
        QVERIFY(secondPopup != nullptr);
        QTRY_COMPARE(TableUiTestHelpers::visibleComboPopupContainers().size(), 1);
        QVERIFY(secondPopup->isVisible());
        QVERIFY(firstPopup == secondPopup || firstPopup == nullptr || !firstPopup->isVisible());

        const double darkRatio =
            TableUiTestHelpers::darkPixelRatio(TableUiTestHelpers::grabWidgetImage(secondPopup));
        QVERIFY2(darkRatio < 0.18,
                 qPrintable(QStringLiteral("Popup image dark ratio too high after switch: %1")
                                .arg(QString::number(darkRatio, 'f', 3))));

        TableUiTestHelpers::hideComboPopup(statusCombo);
        QTRY_VERIFY(TableUiTestHelpers::visibleComboPopupContainers().isEmpty());
    }

    void testInvoicePreviewUsesFormattedLabels()
    {
        invoiceOrdersTable()->selectRow(0);

        QTRY_VERIFY(invoicePreview()->toPlainText().contains("SageStore Invoice"));
        QVERIFY(invoicePreview()->toPlainText().contains("Order ID: 1"));
        QVERIFY(invoicePreview()->toPlainText().contains("Total: 31.50"));
    }

    void testExportInvoiceWritesPreviewToFile()
    {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());

        const QString exportPath = tempDir.filePath("invoice-test.txt");
        view->setInvoiceExportPathProvider(
            [exportPath](QWidget *, const QString &, const QString &, const QString &) {
                return exportPath;
            });

        invoiceOrdersTable()->selectRow(0);
        QTRY_VERIFY(invoicePreview()->toPlainText().contains("SageStore Invoice"));

        QTest::mouseClick(exportButton(), Qt::LeftButton);

        QFile exportedFile(exportPath);
        QVERIFY(exportedFile.open(QIODevice::ReadOnly | QIODevice::Text));
        QCOMPARE(QString::fromUtf8(exportedFile.readAll()), invoicePreview()->toPlainText());
    }

private:
    QTableWidget *ordersTable() const
    {
        auto *table = view->findChild<QTableWidget *>("salesOrdersTable");
        Q_ASSERT(table != nullptr);
        return table;
    }

    QTableWidget *orderRecordsTable() const
    {
        auto *table = view->findChild<QTableWidget *>("salesOrderRecordsTable");
        Q_ASSERT(table != nullptr);
        return table;
    }

    QTableWidget *invoiceOrdersTable() const
    {
        auto *table = view->findChild<QTableWidget *>("salesInvoiceOrdersTable");
        Q_ASSERT(table != nullptr);
        return table;
    }

    QPlainTextEdit *invoicePreview() const
    {
        auto *preview = view->findChild<QPlainTextEdit *>("salesInvoicePreview");
        Q_ASSERT(preview != nullptr);
        return preview;
    }

    QPushButton *addOrderButton() const
    {
        auto *button = view->findChild<QPushButton *>("salesAddOrderButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    QPushButton *addRecordButton() const
    {
        auto *button = view->findChild<QPushButton *>("salesAddRecordButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    QPushButton *deleteRecordButton() const
    {
        auto *button = view->findChild<QPushButton *>("salesDeleteRecordButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    QPushButton *exportButton() const
    {
        auto *button = view->findChild<QPushButton *>("salesExportInvoiceButton");
        Q_ASSERT(button != nullptr);
        return button;
    }
    ApiManagerMock *apiManagerMock{nullptr};
    SalesModel *salesModel{nullptr};
    SalesViewModel *viewModel{nullptr};
    SalesView *view{nullptr};
};

QTEST_MAIN(SalesViewTest)
#include "SalesViewTest.moc"
