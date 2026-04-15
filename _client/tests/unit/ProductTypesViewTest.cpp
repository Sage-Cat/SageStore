#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QtTest>

#include "Ui/Models/ProductTypesModel.hpp"
#include "Ui/ViewModels/ProductTypesViewModel.hpp"
#include "Ui/Views/ProductTypesView.hpp"

#include "mocks/ApiManagerMock.hpp"
#include "wrappers/TableUiTestHelpers.hpp"

class ProductTypesViewTest : public QObject {
    Q_OBJECT

public:
    ProductTypesViewTest() = default;

private slots:
    void init()
    {
        apiManagerMock   = new ApiManagerMock();
        productTypesModel = new ProductTypesModel(*apiManagerMock);
        viewModel         = new ProductTypesViewModel(*productTypesModel);
        view              = new ProductTypesView(*viewModel);
        view->show();
        viewModel->fetchProductTypes();
        QTRY_COMPARE(table()->rowCount(), 2);
    }

    void cleanup()
    {
        delete view;
        delete viewModel;
        delete productTypesModel;
        delete apiManagerMock;
    }

    void testFilterProductTypes()
    {
        auto *filterField = view->findChild<QLineEdit *>("productTypesFilterField");
        QVERIFY(filterField != nullptr);

        filterField->setText("brake");
        QTRY_COMPARE(table()->rowCount(), 1);
        QCOMPARE(table()->item(0, 2)->text(), QString("Brake fluid"));

        filterField->clear();
        QTRY_COMPARE(table()->rowCount(), 2);
    }

    void testAddProductTypeFromView()
    {
        QTest::mouseClick(addButton(), Qt::LeftButton);

        QTRY_COMPARE(table()->rowCount(), 3);
        const int row = table()->rowCount() - 1;
        QVERIFY(table()->item(row, 1)->text().startsWith("PT-"));
        QVERIFY(table()->item(row, 2)->text().startsWith("New product "));
        QCOMPARE(table()->item(row, 4)->text(), QString("pcs"));
        QCOMPARE(table()->item(row, 5)->text(), QString("0.00"));
        QCOMPARE(table()->item(row, 6)->text(), QString("No"));
    }

    void testEditProductTypeFromView()
    {
        const int row = findRowByCode("PT-001");
        QVERIFY(row >= 0);
        table()->selectRow(row);

        TableUiTestHelpers::editTextCell(table(), row, 2, "Engine Oil Pro");
        TableUiTestHelpers::editTextCell(table(), row, 5, "11.99");

        const int editedRow = findRowByCode("PT-001");
        QVERIFY(editedRow >= 0);
        QCOMPARE(table()->item(editedRow, 2)->text(), QString("Engine Oil Pro"));
        QCOMPARE(table()->item(editedRow, 5)->text(), QString("11.99"));
    }

    void testDoubleClickRowOpensEditor()
    {
        const int row = findRowByCode("PT-001");
        QVERIFY(row >= 0);

        TableUiTestHelpers::editTextCell(table(), row, 2, "Engine Oil Inline");

        const int editedRow = findRowByCode("PT-001");
        QVERIFY(editedRow >= 0);
        QCOMPARE(table()->item(editedRow, 2)->text(), QString("Engine Oil Inline"));
    }

    void testDeleteProductTypeFromView()
    {
        const int row = findRowByCode("PT-002");
        QVERIFY(row >= 0);
        table()->selectRow(row);

        QTest::mouseClick(deleteButton(), Qt::LeftButton);

        QTRY_COMPARE(table()->rowCount(), 1);
        QCOMPARE(findRowByCode("PT-002"), -1);
    }

private:
    QTableWidget *table() const
    {
        auto *productTypesTable = view->findChild<QTableWidget *>("productTypesTable");
        Q_ASSERT(productTypesTable != nullptr);
        return productTypesTable;
    }

    QPushButton *addButton() const
    {
        auto *button = view->findChild<QPushButton *>("productTypesAddButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    QPushButton *deleteButton() const
    {
        auto *button = view->findChild<QPushButton *>("productTypesDeleteButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    int findRowByCode(const QString &code) const
    {
        for (int row = 0; row < table()->rowCount(); ++row) {
            auto *codeItem = table()->item(row, 1);
            if (codeItem != nullptr && codeItem->text() == code) {
                return row;
            }
        }

        return -1;
    }
    ApiManagerMock *apiManagerMock{nullptr};
    ProductTypesModel *productTypesModel{nullptr};
    ProductTypesViewModel *viewModel{nullptr};
    ProductTypesView *view{nullptr};
};

QTEST_MAIN(ProductTypesViewTest)
#include "ProductTypesViewTest.moc"
