#include "Ui/Views/ProductTypesView.hpp"

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QTableWidgetItem>

#include "Ui/ViewModels/ProductTypesViewModel.hpp"

#include "common/SpdlogConfig.hpp"

ProductTypesView::ProductTypesView(ProductTypesViewModel &viewModel, QWidget *parent)
    : BaseView(viewModel, parent), m_viewModel(viewModel)
{
    SPDLOG_TRACE("ProductTypesView::ProductTypesView");

    connect(m_addButton, &QPushButton::clicked, this, &ProductTypesView::onAddButtonClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &ProductTypesView::onDeleteButtonClicked);
    connect(m_editButton, &QPushButton::clicked, this, &ProductTypesView::onEditButtonClicked);
    connect(&m_viewModel, &ProductTypesViewModel::productTypesChanged, this,
            &ProductTypesView::onProductTypesChanged);

    m_viewModel.fetchProductTypes();
}

void ProductTypesView::onAddButtonClicked()
{
    DisplayData::ProductType productType;
    if (showProductTypeDialog(productType, tr("Create product type"))) {
        m_viewModel.createProductType(productType);
    }
}

void ProductTypesView::onDeleteButtonClicked()
{
    const auto selectedItems = m_dataTable->selectedItems();
    if (!selectedItems.isEmpty()) {
        const int row = selectedItems.first()->row();
        m_viewModel.deleteProductType(m_dataTable->item(row, 0)->text());
    } else {
        SPDLOG_WARN("No product type selected for deletion.");
    }
}

void ProductTypesView::onEditButtonClicked()
{
    const auto selectedItems = m_dataTable->selectedItems();
    if (selectedItems.isEmpty()) {
        SPDLOG_WARN("No product type selected for editing.");
        return;
    }

    const int row = selectedItems.first()->row();
    DisplayData::ProductType productType;
    productType.id          = m_dataTable->item(row, 0)->text();
    productType.code        = m_dataTable->item(row, 1)->text();
    productType.name        = m_dataTable->item(row, 2)->text();
    productType.barcode     = m_dataTable->item(row, 3)->text();
    productType.unit        = m_dataTable->item(row, 4)->text();
    productType.lastPrice   = m_dataTable->item(row, 5)->text();
    productType.isImported  = m_dataTable->item(row, 6)->text();
    productType.description = m_dataTable->item(row, 7)->text();

    if (showProductTypeDialog(productType, tr("Edit product type"))) {
        m_viewModel.editProductType(productType);
    }
}

void ProductTypesView::onProductTypesChanged()
{
    fillTable(m_viewModel.productTypes());
}

void ProductTypesView::fillTable(const QVector<DisplayData::ProductType> &productTypes)
{
    m_dataTable->clear();
    m_dataTable->setRowCount(productTypes.size());
    m_dataTable->setColumnCount(DisplayData::ProductType::VAR_COUNT);
    m_dataTable->setHorizontalHeaderLabels(
        {"ID", "Code", "Name", "Barcode", "Unit", "Last Price", "Imported", "Description"});

    for (int row = 0; row < productTypes.size(); ++row) {
        const auto &productType = productTypes[row];
        m_dataTable->setItem(row, 0, new QTableWidgetItem(productType.id));
        m_dataTable->setItem(row, 1, new QTableWidgetItem(productType.code));
        m_dataTable->setItem(row, 2, new QTableWidgetItem(productType.name));
        m_dataTable->setItem(row, 3, new QTableWidgetItem(productType.barcode));
        m_dataTable->setItem(row, 4, new QTableWidgetItem(productType.unit));
        m_dataTable->setItem(row, 5, new QTableWidgetItem(productType.lastPrice));
        m_dataTable->setItem(row, 6, new QTableWidgetItem(productType.isImported));
        m_dataTable->setItem(row, 7, new QTableWidgetItem(productType.description));
    }

    m_dataTable->setColumnHidden(0, true);
}

bool ProductTypesView::showProductTypeDialog(DisplayData::ProductType &productType,
                                             const QString &title)
{
    QDialog dialog(this);
    dialog.setWindowTitle(title);

    auto *layout      = new QFormLayout(&dialog);
    auto *codeField   = new QLineEdit(productType.code, &dialog);
    auto *nameField   = new QLineEdit(productType.name, &dialog);
    auto *barcodeField = new QLineEdit(productType.barcode, &dialog);
    auto *unitField   = new QLineEdit(productType.unit, &dialog);
    auto *priceField  = new QLineEdit(productType.lastPrice, &dialog);
    auto *descriptionField = new QLineEdit(productType.description, &dialog);
    auto *importedCheckbox = new QCheckBox(&dialog);
    importedCheckbox->setChecked(productType.isImported.compare("Yes", Qt::CaseInsensitive) == 0);

    layout->addRow(tr("Code"), codeField);
    layout->addRow(tr("Name"), nameField);
    layout->addRow(tr("Barcode"), barcodeField);
    layout->addRow(tr("Unit"), unitField);
    layout->addRow(tr("Last price"), priceField);
    layout->addRow(tr("Description"), descriptionField);
    layout->addRow(tr("Imported"), importedCheckbox);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    productType.code        = codeField->text();
    productType.name        = nameField->text();
    productType.barcode     = barcodeField->text();
    productType.unit        = unitField->text();
    productType.lastPrice   = priceField->text();
    productType.description = descriptionField->text();
    productType.isImported  = importedCheckbox->isChecked() ? "Yes" : "No";

    return true;
}
