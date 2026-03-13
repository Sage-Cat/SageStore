#include <functional>

#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTableWidget>
#include <QTimer>
#include <QtTest>

#include "Ui/Models/SalesModel.hpp"
#include "Ui/ViewModels/SalesViewModel.hpp"
#include "Ui/Views/SalesView.hpp"

#include "mocks/ApiManagerMock.hpp"

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

        openDialogAndFill([](QDialog *dialog) {
            auto *statusBox = dialog->findChild<QComboBox *>("salesOrderStatusField");
            auto *buttons   = dialog->findChild<QDialogButtonBox *>();

            QVERIFY(statusBox != nullptr);
            QVERIFY(buttons != nullptr);
            QCOMPARE(statusBox->currentData().toString(), QString("Pending"));
            QCOMPARE(statusBox->currentText(), QString("Pending"));

            QTest::mouseClick(buttons->button(QDialogButtonBox::Ok), Qt::LeftButton);
        });

        QVERIFY(QMetaObject::invokeMethod(view, "onEditOrder"));

        QTRY_COMPARE(ordersTable()->item(0, 8)->data(Qt::UserRole).toString(), QString("Pending"));
        QCOMPARE(ordersTable()->item(0, 8)->text(), QString("Pending"));
    }

private:
    QTableWidget *ordersTable() const
    {
        auto *table = view->findChild<QTableWidget *>("salesOrdersTable");
        Q_ASSERT(table != nullptr);
        return table;
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
    SalesModel *salesModel{nullptr};
    SalesViewModel *viewModel{nullptr};
    SalesView *view{nullptr};
};

QTEST_MAIN(SalesViewTest)
#include "SalesViewTest.moc"
