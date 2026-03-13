#include <functional>

#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
#include <QtTest>

#include "Ui/Models/UsersManagementModel.hpp"
#include "Ui/ViewModels/UsersManagementViewModel.hpp"
#include "Ui/Views/RolesView.hpp"

#include "mocks/ApiManagerMock.hpp"

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
        openDialogAndFill([](QDialog *dialog) {
            auto *nameField = dialog->findChild<QLineEdit *>("roleNameField");
            auto *buttons   = dialog->findChild<QDialogButtonBox *>();

            QVERIFY(nameField != nullptr);
            QVERIFY(buttons != nullptr);

            nameField->setText("Auditor");
            QTest::mouseClick(buttons->button(QDialogButtonBox::Ok), Qt::LeftButton);
        });

        QTest::mouseClick(addButton(), Qt::LeftButton);

        QTRY_COMPARE(table()->rowCount(), 3);
        QVERIFY(findRowByName("Auditor") >= 0);
    }

    void testEditRoleFromView()
    {
        const int row = findRowByName("Manager");
        QVERIFY(row >= 0);
        table()->selectRow(row);

        openDialogAndFill([](QDialog *dialog) {
            auto *nameField = dialog->findChild<QLineEdit *>("roleNameField");
            auto *buttons   = dialog->findChild<QDialogButtonBox *>();

            QVERIFY(nameField != nullptr);
            QVERIFY(buttons != nullptr);

            nameField->setText("Inventory Lead");
            QTest::mouseClick(buttons->button(QDialogButtonBox::Ok), Qt::LeftButton);
        });

        QTest::mouseClick(editButton(), Qt::LeftButton);

        const int editedRow = findRowByName("Inventory Lead");
        QVERIFY(editedRow >= 0);
        QCOMPARE(table()->item(editedRow, 1)->text(), QString("Inventory Lead"));
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

    QPushButton *editButton() const
    {
        auto *button = view->findChild<QPushButton *>("rolesEditButton");
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

    void openDialogAndFill(const std::function<void(QDialog *)> &fillDialog)
    {
        QTimer::singleShot(0, [fillDialog]() {
            auto *dialog = qobject_cast<QDialog *>(QApplication::activeModalWidget());
            if (dialog == nullptr) {
                for (auto *widget : QApplication::topLevelWidgets()) {
                    dialog = qobject_cast<QDialog *>(widget);
                    if (dialog != nullptr) {
                        break;
                    }
                }
            }

            Q_ASSERT(dialog != nullptr);
            fillDialog(dialog);
        });
    }

    ApiManagerMock *apiManagerMock{nullptr};
    UsersManagementModel *model{nullptr};
    UsersManagementViewModel *viewModel{nullptr};
    RolesView *view{nullptr};
};

QTEST_MAIN(RolesViewTest)
#include "RolesViewTest.moc"
