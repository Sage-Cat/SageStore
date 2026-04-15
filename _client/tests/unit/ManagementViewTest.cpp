#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QtTest>

#include "Ui/Models/ManagementModel.hpp"
#include "Ui/ViewModels/ManagementViewModel.hpp"
#include "Ui/Views/ManagementView.hpp"

#include "mocks/ApiManagerMock.hpp"
#include "wrappers/TableUiTestHelpers.hpp"

class ManagementViewTest : public QObject {
    Q_OBJECT

private slots:
    void init()
    {
        apiManagerMock = new ApiManagerMock();
        managementModel = new ManagementModel(*apiManagerMock);
        viewModel = new ManagementViewModel(*managementModel);
        view = new ManagementView(*viewModel);
        view->show();
        viewModel->fetchAll();
        QTRY_COMPARE(contactsTable()->rowCount(), 1);
        QTRY_COMPARE(suppliersTable()->rowCount(), 1);
        QTRY_COMPARE(employeesTable()->rowCount(), 1);
    }

    void cleanup()
    {
        delete view;
        delete viewModel;
        delete managementModel;
        delete apiManagerMock;
    }

    void testAddContactFromView()
    {
        view->showSection(ManagementView::Section::Contacts);

        QTest::mouseClick(contactAddButton(), Qt::LeftButton);

        QTRY_COMPARE(contactsTable()->rowCount(), 2);
        const int row = contactsTable()->rowCount() - 1;
        QVERIFY(contactsTable()->item(row, 1)->text().startsWith("New customer "));
        QVERIFY(contactsTable()->isPersistentEditorOpen(contactsTable()->item(row, 2)));
        TableUiTestHelpers::editTextCell(contactsTable(), row, 1, "B2B Partner");
        TableUiTestHelpers::setComboCellByText(contactsTable(), row, 2, "Partner");
        TableUiTestHelpers::editTextCell(contactsTable(), row, 3, "partner@example.com");
        TableUiTestHelpers::editTextCell(contactsTable(), row, 4, "555-0101");

        const int updatedRow = findRowByText(contactsTable(), 1, "B2B Partner");
        QVERIFY(updatedRow >= 0);
        QCOMPARE(contactsTable()->item(updatedRow, 2)->text(), QString("Partner"));
    }

    void testEditSupplierFromView()
    {
        view->showSection(ManagementView::Section::Suppliers);
        suppliersTable()->selectRow(0);

        TableUiTestHelpers::editTextCell(suppliersTable(), 0, 1, "Supplier Prime");
        TableUiTestHelpers::editTextCell(suppliersTable(), 0, 4, "Industrial Zone 7");

        const int row = findRowByText(suppliersTable(), 1, "Supplier Prime");
        QVERIFY(row >= 0);
        QCOMPARE(suppliersTable()->item(row, 4)->text(), QString("Industrial Zone 7"));
    }

    void testDoubleClickSupplierRowOpensEditor()
    {
        view->showSection(ManagementView::Section::Suppliers);
        QVERIFY(suppliersTable()->rowCount() > 0);

        TableUiTestHelpers::editTextCell(suppliersTable(), 0, 1, "Supplier Inline");

        const int row = findRowByText(suppliersTable(), 1, "Supplier Inline");
        QVERIFY(row >= 0);
    }

    void testInlineEditorsStayWithinManagementCellGeometry()
    {
        view->showSection(ManagementView::Section::Suppliers);
        auto *editor = TableUiTestHelpers::activateEditor(suppliersTable(), 0, 1);
        QVERIFY(editor != nullptr);

        const QRect cellRect = suppliersTable()->visualItemRect(suppliersTable()->item(0, 1));
        const QRect editorRect = editor->geometry();
        QVERIFY(cellRect.adjusted(-1, -1, 1, 1).contains(editorRect.topLeft()));
        QVERIFY(cellRect.adjusted(-1, -1, 1, 1).contains(editorRect.bottomRight()));
    }

    void testDeleteEmployeeFromView()
    {
        view->showSection(ManagementView::Section::Employees);
        employeesTable()->selectRow(0);

        QTest::mouseClick(employeeDeleteButton(), Qt::LeftButton);

        QTRY_COMPARE(employeesTable()->rowCount(), 0);
    }

private:
    QTableWidget *contactsTable() const
    {
        auto *table = view->findChild<QTableWidget *>("managementContactsTable");
        Q_ASSERT(table != nullptr);
        return table;
    }

    QTableWidget *suppliersTable() const
    {
        auto *table = view->findChild<QTableWidget *>("managementSuppliersTable");
        Q_ASSERT(table != nullptr);
        return table;
    }

    QTableWidget *employeesTable() const
    {
        auto *table = view->findChild<QTableWidget *>("managementEmployeesTable");
        Q_ASSERT(table != nullptr);
        return table;
    }

    QPushButton *contactAddButton() const
    {
        auto *button = view->findChild<QPushButton *>("managementContactsAddButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    QPushButton *employeeDeleteButton() const
    {
        auto *button = view->findChild<QPushButton *>("managementEmployeesDeleteButton");
        Q_ASSERT(button != nullptr);
        return button;
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
    ApiManagerMock *apiManagerMock{nullptr};
    ManagementModel *managementModel{nullptr};
    ManagementViewModel *viewModel{nullptr};
    ManagementView *view{nullptr};
};

QTEST_MAIN(ManagementViewTest)
#include "ManagementViewTest.moc"
