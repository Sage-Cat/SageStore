#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QtTest>

#include "Ui/Models/SupplierCatalogModel.hpp"
#include "Ui/ViewModels/SupplierCatalogViewModel.hpp"
#include "Ui/Views/SupplierCatalogView.hpp"

#include "mocks/ApiManagerMock.hpp"
#include "wrappers/TableUiTestHelpers.hpp"

class SupplierCatalogViewTest : public QObject {
    Q_OBJECT

private slots:
    void init()
    {
        apiManagerMock = new ApiManagerMock();
        supplierCatalogModel = new SupplierCatalogModel(*apiManagerMock);
        viewModel = new SupplierCatalogViewModel(*supplierCatalogModel);
        view = new SupplierCatalogView(*viewModel);
        view->show();
        viewModel->fetchAll();
        QTRY_COMPARE(table()->rowCount(), 1);
    }

    void cleanup()
    {
        delete view;
        delete viewModel;
        delete supplierCatalogModel;
        delete apiManagerMock;
    }

    void testAddMappingFromView()
    {
        QTest::mouseClick(addButton(), Qt::LeftButton);

        QTRY_COMPARE(table()->rowCount(), 2);
        const int row = table()->rowCount() - 1;
        QVERIFY(table()->item(row, 5)->text().startsWith("CODE-"));
    }

    void testEditMappingFromView()
    {
        TableUiTestHelpers::editTextCell(table(), 0, 5, "SUP-EDIT");

        QVERIFY(findRowByCode("SUP-EDIT") >= 0);
    }

    void testDeleteMappingFromView()
    {
        table()->selectRow(0);

        QTest::mouseClick(deleteButton(), Qt::LeftButton);

        QTRY_COMPARE(table()->rowCount(), 0);
    }

private:
    QTableWidget *table() const
    {
        auto *catalogTable = view->findChild<QTableWidget *>("supplierCatalogTable");
        Q_ASSERT(catalogTable != nullptr);
        return catalogTable;
    }

    QPushButton *addButton() const
    {
        auto *button = view->findChild<QPushButton *>("supplierCatalogAddButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    QPushButton *deleteButton() const
    {
        auto *button = view->findChild<QPushButton *>("supplierCatalogDeleteButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    int findRowByCode(const QString &code) const
    {
        for (int row = 0; row < table()->rowCount(); ++row) {
            auto *item = table()->item(row, 5);
            if (item != nullptr && item->text() == code) {
                return row;
            }
        }

        return -1;
    }
    ApiManagerMock *apiManagerMock{nullptr};
    SupplierCatalogModel *supplierCatalogModel{nullptr};
    SupplierCatalogViewModel *viewModel{nullptr};
    SupplierCatalogView *view{nullptr};
};

QTEST_MAIN(SupplierCatalogViewTest)
#include "SupplierCatalogViewTest.moc"
