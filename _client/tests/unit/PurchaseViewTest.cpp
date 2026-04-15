#include <QDate>
#include <QComboBox>
#include <QFrame>
#include <QListView>
#include <QPointer>
#include <QPushButton>
#include <QStyleOptionComboBox>
#include <QTableWidget>
#include <QtTest>

#include "Ui/Models/PurchaseModel.hpp"
#include "Ui/ViewModels/PurchaseViewModel.hpp"
#include "Ui/Views/PurchaseView.hpp"

#include "mocks/ApiManagerMock.hpp"
#include "wrappers/TableUiTestHelpers.hpp"

class PurchaseViewTest : public QObject {
    Q_OBJECT

private slots:
    void init()
    {
        apiManagerMock = new ApiManagerMock();
        purchaseModel = new PurchaseModel(*apiManagerMock);
        viewModel = new PurchaseViewModel(*purchaseModel);
        view = new PurchaseView(*viewModel);
        view->show();
        viewModel->fetchAll();
        QTRY_COMPARE(ordersTable()->rowCount(), 1);
        QTRY_COMPARE(receiptOrdersTable()->rowCount(), 1);
    }

    void cleanup()
    {
        delete view;
        delete viewModel;
        delete purchaseModel;
        delete apiManagerMock;
    }

    void testOrderRecordButtonsFollowSelection()
    {
        ordersTable()->selectRow(0);
        QTRY_COMPARE(recordsTable()->rowCount(), 1);
        QVERIFY(addRecordButton()->isEnabled());
        QVERIFY(!deleteRecordButton()->isEnabled());

        recordsTable()->selectRow(0);

        QVERIFY(deleteRecordButton()->isEnabled());
    }

    void testAddOrderUsesDefaultInlineValues()
    {
        QTest::mouseClick(addOrderButton(), Qt::LeftButton);

        QTRY_COMPARE(ordersTable()->rowCount(), 2);
        const int row = ordersTable()->rowCount() - 1;
        QCOMPARE(ordersTable()->item(row, 1)->text(), QDate::currentDate().toString("yyyy-MM-dd"));
        QCOMPARE(ordersTable()->item(row, 3)->text(), QString("admin"));
        QCOMPARE(ordersTable()->item(row, 5)->text(), QString("Default Supplier"));
        QCOMPARE(ordersTable()->item(row, 6)->data(Qt::UserRole).toString(), QString("Draft"));
    }

    void testOrdersTableProvidesInlineStatusCombo()
    {
        QVERIFY(ordersTable()->isPersistentEditorOpen(ordersTable()->item(0, 6)));
        auto *statusCombo = TableUiTestHelpers::comboCell(ordersTable(), 0, 6);
        QVERIFY(statusCombo != nullptr);
        QVERIFY(statusCombo->view() == nullptr || !statusCombo->view()->isVisible());
        QCOMPARE(statusCombo->findData(QStringLiteral("Received")), -1);
        QCOMPARE(statusCombo->currentData().toString(), QString("Ordered"));
        QTRY_COMPARE(ordersTable()->item(0, 6)->data(Qt::UserRole).toString(), QString("Ordered"));
        QCOMPARE(ordersTable()->item(0, 6)->text(), QString("Ordered"));
    }

    void testOrdersTableInlineStatusComboPreservesUnknownStatus()
    {
        apiManagerMock->setPurchaseOrders(
            {Common::Entities::PurchaseOrder{.id = "1",
                                             .date = "2026-03-13",
                                             .userId = "1",
                                             .supplierId = "1",
                                             .status = "Backordered"}});
        viewModel->fetchAll();

        QTRY_COMPARE(ordersTable()->rowCount(), 1);

        auto *statusCombo = TableUiTestHelpers::comboCell(ordersTable(), 0, 6);
        QVERIFY(statusCombo != nullptr);
        QCOMPARE(statusCombo->currentData().toString(), QString("Backordered"));
        QCOMPARE(statusCombo->currentText(), QString("Backordered"));
    }

    void testUnchangedStatusSelectionDoesNotPersist()
    {
        const int editCountBefore = apiManagerMock->purchaseOrderEditCount();
        TableUiTestHelpers::setComboCellByText(ordersTable(), 0, 6, QStringLiteral("Ordered"));

        QTRY_COMPARE(apiManagerMock->purchaseOrderEditCount(), editCountBefore);
        QCOMPARE(ordersTable()->item(0, 6)->data(Qt::UserRole).toString(), QString("Ordered"));
    }

    void testSelectingOrderDoesNotTriggerBackgroundEditLoop()
    {
        QTest::qWait(25);

        const int editCountBefore = apiManagerMock->purchaseOrderEditCount();
        const int recordRequestsBefore = apiManagerMock->purchaseOrderRecordsRequestCount();

        ordersTable()->selectRow(0);
        QTRY_COMPARE(recordsTable()->rowCount(), 1);

        QTest::qWait(25);
        const int recordRequestsAfterSelection = apiManagerMock->purchaseOrderRecordsRequestCount();

        QCOMPARE(apiManagerMock->purchaseOrderEditCount(), editCountBefore);
        QVERIFY(recordRequestsAfterSelection == recordRequestsBefore ||
                recordRequestsAfterSelection == recordRequestsBefore + 1);

        QTest::qWait(25);
        QCOMPARE(apiManagerMock->purchaseOrderEditCount(), editCountBefore);
        QCOMPARE(apiManagerMock->purchaseOrderRecordsRequestCount(), recordRequestsAfterSelection);
    }

    void testOrderDateEditsInlineFromOrdersTable()
    {
        ordersTable()->selectRow(0);
        TableUiTestHelpers::editTextCell(ordersTable(), 0, 1, "2026-04-01");
        QTRY_COMPARE(ordersTable()->item(0, 1)->text(), QString("2026-04-01"));
    }

    void testOrderLineEditsInlineFromOrdersTable()
    {
        ordersTable()->selectRow(0);
        QTRY_COMPARE(recordsTable()->rowCount(), 1);
        QVERIFY(recordsTable()->isPersistentEditorOpen(recordsTable()->item(0, 3)));
        QVERIFY(recordsTable()->item(0, 3)->flags() & Qt::ItemIsEditable);

        TableUiTestHelpers::editTextCell(recordsTable(), 0, 4, "7");
        TableUiTestHelpers::editTextCell(recordsTable(), 0, 5, "12.25");

        QTRY_COMPARE(recordsTable()->item(0, 4)->text(), QString("7"));
        QCOMPARE(recordsTable()->item(0, 5)->text(), QString("12.25"));
    }

    void testInlineEditorsStayWithinPurchaseCellGeometry()
    {
        ordersTable()->selectRow(0);
        QTRY_COMPARE(recordsTable()->rowCount(), 1);

        auto *editor = TableUiTestHelpers::activateEditor(recordsTable(), 0, 5);
        QVERIFY(editor != nullptr);

        const QRect cellRect = recordsTable()->visualItemRect(recordsTable()->item(0, 5));
        const QRect editorRect = editor->geometry();
        QVERIFY(cellRect.adjusted(-1, -1, 1, 1).contains(editorRect.topLeft()));
        QVERIFY(cellRect.adjusted(-1, -1, 1, 1).contains(editorRect.bottomRight()));
    }

    void testStatusComboEditorStaysWithinPurchaseCellGeometry()
    {
        auto *editor = TableUiTestHelpers::activateEditor(ordersTable(), 0, 6);
        QVERIFY(editor != nullptr);

        const QRect cellRect = ordersTable()->visualItemRect(ordersTable()->item(0, 6));
        const QRect editorRect = editor->geometry();
        QVERIFY(cellRect.adjusted(-1, -1, 1, 1).contains(editorRect.topLeft()));
        QVERIFY(cellRect.adjusted(-1, -1, 1, 1).contains(editorRect.bottomRight()));
    }

    void testInlineStatusComboUsesInsetFramelessChrome()
    {
        auto *statusCombo = TableUiTestHelpers::comboCell(ordersTable(), 0, 6);
        QVERIFY(statusCombo != nullptr);
        QVERIFY(!statusCombo->hasFrame());
        QVERIFY(statusCombo->testAttribute(Qt::WA_StyledBackground));

        const QRect cellRect = ordersTable()->visualItemRect(ordersTable()->item(0, 6));
        const QRect comboRect = statusCombo->geometry();
        QVERIFY(comboRect.left() > cellRect.left());
        QVERIFY(comboRect.right() < cellRect.right());
        QVERIFY(comboRect.top() > cellRect.top());
        QVERIFY(comboRect.bottom() < cellRect.bottom());
    }

    void testInlineStatusComboUsesDedicatedPopupListView()
    {
        auto *statusCombo = TableUiTestHelpers::comboCell(ordersTable(), 0, 6);
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

    void testSupplierComboPopupRendersWithoutDarkOverlay()
    {
        auto *supplierCombo = TableUiTestHelpers::comboCell(ordersTable(), 0, 5);
        QVERIFY(supplierCombo != nullptr);

        QWidget *popupContainer = TableUiTestHelpers::showComboPopup(supplierCombo);
        QVERIFY(popupContainer != nullptr);
        QVERIFY(popupContainer->property("tableComboPopupContainer").toBool());
        QCOMPARE(TableUiTestHelpers::visibleComboPopupContainers().size(), 1);

        const QImage popupImage = TableUiTestHelpers::grabWidgetImage(popupContainer);
        QVERIFY(!popupImage.isNull());
        QVERIFY(popupImage.width() >= supplierCombo->width());
        QVERIFY(popupImage.height() > 8);

        const double darkRatio = TableUiTestHelpers::darkPixelRatio(popupImage);
        QVERIFY2(darkRatio < 0.18,
                 qPrintable(QStringLiteral("Popup image dark ratio too high: %1")
                                .arg(QString::number(darkRatio, 'f', 3))));

        TableUiTestHelpers::hideComboPopup(supplierCombo);
        QTRY_VERIFY(TableUiTestHelpers::visibleComboPopupContainers().isEmpty());
    }

    void testSwitchingBetweenPurchaseCombosLeavesSingleVisiblePopup()
    {
        QTest::mouseClick(addOrderButton(), Qt::LeftButton);
        QTRY_COMPARE(ordersTable()->rowCount(), 2);

        auto *supplierCombo = TableUiTestHelpers::comboCell(ordersTable(), 0, 5);
        auto *statusCombo = TableUiTestHelpers::comboCell(ordersTable(), 1, 6);
        QVERIFY(supplierCombo != nullptr);
        QVERIFY(statusCombo != nullptr);

        QPointer<QWidget> firstPopup = TableUiTestHelpers::showComboPopup(supplierCombo);
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

    void testReceivedOrdersBecomeReadOnlyAfterReceipt()
    {
        view->showSection(PurchaseView::Section::Receipts);
        receiptOrdersTable()->selectRow(0);
        QVERIFY(receiveButton()->isEnabled());

        QTest::mouseClick(receiveButton(), Qt::LeftButton);

        view->showSection(PurchaseView::Section::Orders);
        ordersTable()->selectRow(0);

        QTRY_COMPARE(ordersTable()->item(0, 6)->data(Qt::UserRole).toString(), QString("Received"));
        QVERIFY(!deleteOrderButton()->isEnabled());
        QVERIFY(!addRecordButton()->isEnabled());
        QVERIFY(ordersTable()->cellWidget(0, 6) == nullptr);
        QVERIFY(!(ordersTable()->item(0, 6)->flags() & Qt::ItemIsEditable));

        QTRY_COMPARE(recordsTable()->rowCount(), 1);
        recordsTable()->selectRow(0);
        QVERIFY(!deleteRecordButton()->isEnabled());
        QVERIFY(!(recordsTable()->item(0, 4)->flags() & Qt::ItemIsEditable));
    }

private:
    QTableWidget *ordersTable() const
    {
        auto *table = view->findChild<QTableWidget *>("purchaseOrdersTable");
        Q_ASSERT(table != nullptr);
        return table;
    }

    QTableWidget *recordsTable() const
    {
        auto *table = view->findChild<QTableWidget *>("purchaseOrderRecordsTable");
        Q_ASSERT(table != nullptr);
        return table;
    }

    QTableWidget *receiptOrdersTable() const
    {
        auto *table = view->findChild<QTableWidget *>("purchaseReceiptOrdersTable");
        Q_ASSERT(table != nullptr);
        return table;
    }

    QPushButton *addOrderButton() const
    {
        auto *button = view->findChild<QPushButton *>("purchaseAddOrderButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    QPushButton *deleteOrderButton() const
    {
        auto *button = view->findChild<QPushButton *>("purchaseDeleteOrderButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    QPushButton *addRecordButton() const
    {
        auto *button = view->findChild<QPushButton *>("purchaseAddRecordButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    QPushButton *deleteRecordButton() const
    {
        auto *button = view->findChild<QPushButton *>("purchaseDeleteRecordButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    QPushButton *receiveButton() const
    {
        auto *button = view->findChild<QPushButton *>("purchaseReceiveButton");
        Q_ASSERT(button != nullptr);
        return button;
    }
    ApiManagerMock *apiManagerMock{nullptr};
    PurchaseModel *purchaseModel{nullptr};
    PurchaseViewModel *viewModel{nullptr};
    PurchaseView *view{nullptr};
};

QTEST_MAIN(PurchaseViewTest)
#include "PurchaseViewTest.moc"
