#include <functional>

#include <QApplication>
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
#include <QtTest>

#include "Ui/Models/ProductTypesModel.hpp"
#include "Ui/ViewModels/ProductTypesViewModel.hpp"
#include "Ui/Views/ProductTypesView.hpp"

#include "mocks/ApiManagerMock.hpp"

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
        openDialogAndFill([](QDialog *dialog) {
            auto *codeField = dialog->findChild<QLineEdit *>("productTypeCodeField");
            auto *nameField = dialog->findChild<QLineEdit *>("productTypeNameField");
            auto *barcodeField = dialog->findChild<QLineEdit *>("productTypeBarcodeField");
            auto *unitField = dialog->findChild<QLineEdit *>("productTypeUnitField");
            auto *priceField = dialog->findChild<QLineEdit *>("productTypePriceField");
            auto *descriptionField = dialog->findChild<QPlainTextEdit *>("productTypeDescriptionField");
            auto *importedCheckbox = dialog->findChild<QCheckBox *>("productTypeImportedCheckbox");
            auto *buttons = dialog->findChild<QDialogButtonBox *>();

            QVERIFY(codeField != nullptr);
            QVERIFY(nameField != nullptr);
            QVERIFY(barcodeField != nullptr);
            QVERIFY(unitField != nullptr);
            QVERIFY(priceField != nullptr);
            QVERIFY(descriptionField != nullptr);
            QVERIFY(importedCheckbox != nullptr);
            QVERIFY(buttons != nullptr);

            codeField->setText("PT-003");
            nameField->setText("Air filter");
            barcodeField->setText("789");
            unitField->setText("pcs");
            priceField->setText("8.25");
            descriptionField->setPlainText("Cabin air filter");
            importedCheckbox->setChecked(true);
            QTest::mouseClick(buttons->button(QDialogButtonBox::Ok), Qt::LeftButton);
        });

        QTest::mouseClick(addButton(), Qt::LeftButton);

        QTRY_COMPARE(table()->rowCount(), 3);
        QVERIFY(findRowByCode("PT-003") >= 0);
    }

    void testEditProductTypeFromView()
    {
        const int row = findRowByCode("PT-001");
        QVERIFY(row >= 0);
        table()->selectRow(row);

        openDialogAndFill([](QDialog *dialog) {
            auto *nameField = dialog->findChild<QLineEdit *>("productTypeNameField");
            auto *priceField = dialog->findChild<QLineEdit *>("productTypePriceField");
            auto *buttons = dialog->findChild<QDialogButtonBox *>();

            QVERIFY(nameField != nullptr);
            QVERIFY(priceField != nullptr);
            QVERIFY(buttons != nullptr);

            nameField->setText("Engine Oil Pro");
            priceField->setText("11.99");
            QTest::mouseClick(buttons->button(QDialogButtonBox::Ok), Qt::LeftButton);
        });

        QTest::mouseClick(editButton(), Qt::LeftButton);

        const int editedRow = findRowByCode("PT-001");
        QVERIFY(editedRow >= 0);
        QCOMPARE(table()->item(editedRow, 2)->text(), QString("Engine Oil Pro"));
        QCOMPARE(table()->item(editedRow, 5)->text(), QString("11.99"));
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

    QPushButton *editButton() const
    {
        auto *button = view->findChild<QPushButton *>("productTypesEditButton");
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
    ProductTypesModel *productTypesModel{nullptr};
    ProductTypesViewModel *viewModel{nullptr};
    ProductTypesView *view{nullptr};
};

QTEST_MAIN(ProductTypesViewTest)
#include "ProductTypesViewTest.moc"
