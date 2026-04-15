#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QtTest>

#include "Ui/Models/UsersManagementModel.hpp"
#include "Ui/ViewModels/UsersManagementViewModel.hpp"
#include "Ui/Views/UsersView.hpp"

#include "mocks/ApiManagerMock.hpp"
#include "wrappers/TableUiTestHelpers.hpp"

class UsersViewTest : public QObject {
    Q_OBJECT

private slots:
    void init()
    {
        apiManagerMock = new ApiManagerMock();
        usersModel = new UsersManagementModel(*apiManagerMock);
        viewModel = new UsersManagementViewModel(*usersModel);
        view = new UsersView(*viewModel);
        view->show();
        viewModel->fetchRoles();
        viewModel->fetchUsers();
        QTRY_COMPARE(table()->rowCount(), 2);
    }

    void cleanup()
    {
        delete view;
        delete viewModel;
        delete usersModel;
        delete apiManagerMock;
    }

    void testActionButtonsFollowSelection()
    {
        QVERIFY(!deleteButton()->isEnabled());

        table()->selectRow(0);

        QVERIFY(deleteButton()->isEnabled());
    }

    void testAddUserFromView()
    {
        QTest::mouseClick(addButton(), Qt::LeftButton);

        QTRY_COMPARE(table()->rowCount(), 3);
        const int row = findRowByUsernamePrefix("user-");
        QVERIFY(row >= 0);
        QCOMPARE(table()->item(row, 4)->text(), QString("Administrator"));
    }

    void testEditUserFromView()
    {
        const int row = findRowByUsername("admin");
        QVERIFY(row >= 0);
        table()->selectRow(row);
        QVERIFY(table()->isPersistentEditorOpen(table()->item(row, 4)));

        TableUiTestHelpers::editTextCell(table(), row, 1, "admin-root");
        TableUiTestHelpers::setComboCellByText(table(), row, 4, "Manager");

        const int editedRow = findRowByUsername("admin-root");
        QVERIFY(editedRow >= 0);
        QCOMPARE(table()->item(editedRow, 4)->text(), QString("Manager"));
    }

    void testUnchangedRoleSelectionDoesNotPersist()
    {
        const int row = findRowByUsername("admin");
        QVERIFY(row >= 0);

        const int editCountBefore = apiManagerMock->userEditCount();
        TableUiTestHelpers::setComboCellByText(table(), row, 4, "Administrator");

        QTRY_COMPARE(apiManagerMock->userEditCount(), editCountBefore);
        QCOMPARE(table()->item(row, 4)->text(), QString("Administrator"));
    }

    void testDeleteUserFromView()
    {
        const int row = findRowByUsername("manager");
        QVERIFY(row >= 0);
        table()->selectRow(row);

        QTest::mouseClick(deleteButton(), Qt::LeftButton);

        QTRY_COMPARE(table()->rowCount(), 1);
        QCOMPARE(findRowByUsername("manager"), -1);
    }

private:
    QTableWidget *table() const
    {
        auto *usersTable = view->findChild<QTableWidget *>("usersTable");
        Q_ASSERT(usersTable != nullptr);
        return usersTable;
    }

    QPushButton *addButton() const
    {
        auto *button = view->findChild<QPushButton *>("usersAddButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    QPushButton *deleteButton() const
    {
        auto *button = view->findChild<QPushButton *>("usersDeleteButton");
        Q_ASSERT(button != nullptr);
        return button;
    }

    int findRowByUsername(const QString &username) const
    {
        for (int row = 0; row < table()->rowCount(); ++row) {
            auto *item = table()->item(row, 1);
            if (item != nullptr && item->text() == username) {
                return row;
            }
        }

        return -1;
    }

    int findRowByUsernamePrefix(const QString &prefix) const
    {
        for (int row = 0; row < table()->rowCount(); ++row) {
            auto *item = table()->item(row, 1);
            if (item != nullptr && item->text().startsWith(prefix)) {
                return row;
            }
        }
        return -1;
    }

    ApiManagerMock *apiManagerMock{nullptr};
    UsersManagementModel *usersModel{nullptr};
    UsersManagementViewModel *viewModel{nullptr};
    UsersView *view{nullptr};
};

QTEST_MAIN(UsersViewTest)
#include "UsersViewTest.moc"
