#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QtTest>

#include "Ui/Models/StocksModel.hpp"
#include "Ui/ViewModels/StocksViewModel.hpp"
#include "Ui/Views/StocksView.hpp"

#include "mocks/ApiManagerMock.hpp"
#include "wrappers/TableUiTestHelpers.hpp"

class StocksViewTest : public QObject {
    Q_OBJECT

private slots:
    void init()
    {
        apiManagerMock = new ApiManagerMock();
        stocksModel    = new StocksModel(*apiManagerMock);
        viewModel      = new StocksViewModel(*stocksModel);
        view           = new StocksView(*viewModel);
        view->show();
        viewModel->fetchProductTypes();
        viewModel->fetchStocks();
        QTRY_COMPARE(table()->rowCount(), 1);
    }

    void cleanup()
    {
        delete view;
        delete viewModel;
        delete stocksModel;
        delete apiManagerMock;
        view           = nullptr;
        viewModel      = nullptr;
        stocksModel    = nullptr;
        apiManagerMock = nullptr;
    }

    void testFilterStocks()
    {
        auto *filterField = view->findChild<QLineEdit *>("stocksFilterField");
        QVERIFY(filterField != nullptr);

        filterField->setText("Oil");
        QTRY_COMPARE(table()->rowCount(), 1);

        filterField->setText("999");
        QTRY_COMPARE(table()->rowCount(), 0);

        filterField->clear();
        QTRY_COMPARE(table()->rowCount(), 1);
    }

    void testAddStockFromView()
    {
        QTest::mouseClick(addButton(), Qt::LeftButton);

        QTRY_COMPARE(table()->rowCount(), 2);
        const int row = findRowByProductLabel("PT-002 | Brake fluid");
        QVERIFY(row >= 0);
        QCOMPARE(table()->item(row, 3)->text(), QString("0"));
        QCOMPARE(table()->item(row, 4)->text(), QString("1"));
    }

    void testEditStockFromView()
    {
        table()->selectRow(0);

        TableUiTestHelpers::editTextCell(table(), 0, 3, "19");
        TableUiTestHelpers::editTextCell(table(), 0, 4, "3");

        QCOMPARE(table()->item(0, 3)->text(), QString("19"));
        QCOMPARE(table()->item(0, 4)->text(), QString("3"));
    }

    void testUnchangedQuantityEditDoesNotPersist()
    {
        const int editCountBefore = apiManagerMock->stockEditCount();
        TableUiTestHelpers::editTextCell(table(), 0, 3, "10");

        QTRY_COMPARE(apiManagerMock->stockEditCount(), editCountBefore);
        QCOMPARE(table()->item(0, 3)->text(), QString("10"));
    }

    void testDeleteStockFromView()
    {
        table()->selectRow(0);

        QTest::mouseClick(deleteButton(), Qt::LeftButton);

        QTRY_COMPARE(table()->rowCount(), 0);
    }

private:
    QTableWidget *table() const
    {
        auto *stocksTable = view->findChild<QTableWidget *>("stocksTable");
        Q_ASSERT(stocksTable != nullptr);
        return stocksTable;
    }

    QPushButton *addButton() const
    {
        auto *button = view->findChild<QPushButton *>("stocksAddButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    QPushButton *deleteButton() const
    {
        auto *button = view->findChild<QPushButton *>("stocksDeleteButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    int findRowByProductLabel(const QString &label) const
    {
        for (int row = 0; row < table()->rowCount(); ++row) {
            auto *item = table()->item(row, 2);
            if (item != nullptr && item->text() == label) {
                return row;
            }
        }

        return -1;
    }
    ApiManagerMock *apiManagerMock{nullptr};
    StocksModel *stocksModel{nullptr};
    StocksViewModel *viewModel{nullptr};
    StocksView *view{nullptr};
};

QTEST_MAIN(StocksViewTest)
#include "StocksViewTest.moc"
