#include <functional>

#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
#include <QtTest>

#include "Ui/Models/StocksModel.hpp"
#include "Ui/ViewModels/StocksViewModel.hpp"
#include "Ui/Views/StocksView.hpp"

#include "mocks/ApiManagerMock.hpp"

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
        openDialogAndFill([](QDialog *dialog) {
            auto *productTypeBox = dialog->findChild<QComboBox *>("stockProductTypeCombo");
            auto *quantityField  = dialog->findChild<QLineEdit *>("stockQuantityField");
            auto *employeeField  = dialog->findChild<QLineEdit *>("stockEmployeeIdField");
            auto *buttons        = dialog->findChild<QDialogButtonBox *>();

            QVERIFY(productTypeBox != nullptr);
            QVERIFY(quantityField != nullptr);
            QVERIFY(employeeField != nullptr);
            QVERIFY(buttons != nullptr);

            productTypeBox->setCurrentIndex(1);
            quantityField->setText("14");
            employeeField->setText("2");
            QTest::mouseClick(buttons->button(QDialogButtonBox::Ok), Qt::LeftButton);
        });

        QTest::mouseClick(addButton(), Qt::LeftButton);

        QTRY_COMPARE(table()->rowCount(), 2);
        QVERIFY(findRowByProductLabel("PT-002 | Brake fluid") >= 0);
    }

    void testEditStockFromView()
    {
        table()->selectRow(0);

        openDialogAndFill([](QDialog *dialog) {
            auto *quantityField = dialog->findChild<QLineEdit *>("stockQuantityField");
            auto *employeeField = dialog->findChild<QLineEdit *>("stockEmployeeIdField");
            auto *buttons       = dialog->findChild<QDialogButtonBox *>();

            QVERIFY(quantityField != nullptr);
            QVERIFY(employeeField != nullptr);
            QVERIFY(buttons != nullptr);

            quantityField->setText("19");
            employeeField->setText("3");
            QTest::mouseClick(buttons->button(QDialogButtonBox::Ok), Qt::LeftButton);
        });

        QTest::mouseClick(editButton(), Qt::LeftButton);

        QCOMPARE(table()->item(0, 3)->text(), QString("19"));
        QCOMPARE(table()->item(0, 4)->text(), QString("3"));
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

    QPushButton *editButton() const
    {
        auto *button = view->findChild<QPushButton *>("stocksEditButton");
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
    StocksModel *stocksModel{nullptr};
    StocksViewModel *viewModel{nullptr};
    StocksView *view{nullptr};
};

QTEST_MAIN(StocksViewTest)
#include "StocksViewTest.moc"
