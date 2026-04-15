#include <QPushButton>
#include <QTableWidget>
#include <QtTest>

#include "Ui/Models/UsersManagementModel.hpp"
#include "Ui/ViewModels/UsersManagementViewModel.hpp"
#include "Ui/Views/RolesView.hpp"

#include "mocks/ApiManagerMock.hpp"
#include "wrappers/TableUiTestHelpers.hpp"

class RolesViewTest : public QObject {
    Q_OBJECT

private slots:
    void init()
    {
        apiManagerMock = new ApiManagerMock();
        model          = new UsersManagementModel(*apiManagerMock);
        viewModel      = new UsersManagementViewModel(*model);
        view           = new RolesView(*viewModel);
        view->show();
        viewModel->fetchRoles();
        QTRY_COMPARE(table()->rowCount(), 2);
    }

    void cleanup()
    {
        delete view;
        delete viewModel;
        delete model;
        delete apiManagerMock;
        view           = nullptr;
        viewModel      = nullptr;
        model          = nullptr;
        apiManagerMock = nullptr;
    }

    void testAddRoleFromView()
    {
        QTest::mouseClick(addButton(), Qt::LeftButton);

        QTRY_COMPARE(table()->rowCount(), 3);
        const int row = table()->rowCount() - 1;
        QVERIFY(table()->item(row, 1)->text().startsWith("New role "));
    }

    void testEditRoleFromView()
    {
        const int row = findRowByName("Manager");
        QVERIFY(row >= 0);
        table()->selectRow(row);

        TableUiTestHelpers::editTextCell(table(), row, 1, "Inventory Lead");

        const int editedRow = findRowByName("Inventory Lead");
        QVERIFY(editedRow >= 0);
        QCOMPARE(table()->item(editedRow, 1)->text(), QString("Inventory Lead"));
    }

    void testUnchangedRoleEditDoesNotPersist()
    {
        const int row = findRowByName("Manager");
        QVERIFY(row >= 0);

        const int editCountBefore = apiManagerMock->roleEditCount();
        TableUiTestHelpers::editTextCell(table(), row, 1, "Manager");

        QTRY_COMPARE(apiManagerMock->roleEditCount(), editCountBefore);
        QCOMPARE(table()->item(row, 1)->text(), QString("Manager"));
    }

    void testDeleteRoleFromView()
    {
        const int row = findRowByName("Administrator");
        QVERIFY(row >= 0);
        table()->selectRow(row);

        QTest::mouseClick(deleteButton(), Qt::LeftButton);

        QTRY_COMPARE(table()->rowCount(), 1);
        QCOMPARE(findRowByName("Administrator"), -1);
    }

private:
    QTableWidget *table() const
    {
        auto *rolesTable = view->findChild<QTableWidget *>("rolesTable");
        Q_ASSERT(rolesTable != nullptr);
        return rolesTable;
    }

    QPushButton *addButton() const
    {
        auto *button = view->findChild<QPushButton *>("rolesAddButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    QPushButton *deleteButton() const
    {
        auto *button = view->findChild<QPushButton *>("rolesDeleteButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    int findRowByName(const QString &name) const
    {
        for (int row = 0; row < table()->rowCount(); ++row) {
            auto *nameItem = table()->item(row, 1);
            if (nameItem != nullptr && nameItem->text() == name) {
                return row;
            }
        }

        return -1;
    }
    ApiManagerMock *apiManagerMock{nullptr};
    UsersManagementModel *model{nullptr};
    UsersManagementViewModel *viewModel{nullptr};
    RolesView *view{nullptr};
};

QTEST_MAIN(RolesViewTest)
#include "RolesViewTest.moc"
