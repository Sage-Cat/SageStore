#include "Ui/Views/ProductTypesView.hpp"

#include <QAbstractItemView>
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleValidator>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include "Ui/ViewModels/ProductTypesViewModel.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
bool matchesFilter(const DisplayData::ProductType &productType, const QString &filterText)
{
    if (filterText.isEmpty()) {
        return true;
    }

    const QStringList haystack = {productType.code,        productType.name,  productType.barcode,
                                  productType.unit,        productType.lastPrice,
                                  productType.description, productType.isImported};

    for (const auto &value : haystack) {
        if (value.contains(filterText, Qt::CaseInsensitive)) {
            return true;
        }
    }

    return false;
}
} // namespace

ProductTypesView::ProductTypesView(ProductTypesViewModel &viewModel, QWidget *parent)
    : BaseView(viewModel, parent), m_viewModel(viewModel)
{
    SPDLOG_TRACE("ProductTypesView::ProductTypesView");

    m_addButton->setObjectName("productTypesAddButton");
    m_editButton->setObjectName("productTypesEditButton");
    m_deleteButton->setObjectName("productTypesDeleteButton");
    m_dataTable->setObjectName("productTypesTable");
    m_status->setObjectName("productTypesStatusLabel");

    auto *filterLabel = new QLabel(tr("Filter"), this);
    m_filterField     = new QLineEdit(this);
    m_filterField->setObjectName("productTypesFilterField");
    m_filterField->setPlaceholderText(
        tr("Search by code, name, barcode, unit, description, or imported state"));
    m_filterField->setClearButtonEnabled(true);

    auto *filterRow = new QHBoxLayout;
    filterRow->addWidget(filterLabel);
    filterRow->addWidget(m_filterField);
    m_mainLayout->insertLayout(1, filterRow);

    m_dataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_dataTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_dataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_dataTable->setAlternatingRowColors(true);
    m_dataTable->setSortingEnabled(true);
    m_dataTable->verticalHeader()->setVisible(false);

    connect(m_addButton, &QPushButton::clicked, this, &ProductTypesView::onAddButtonClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &ProductTypesView::onDeleteButtonClicked);
    connect(m_editButton, &QPushButton::clicked, this, &ProductTypesView::onEditButtonClicked);
    connect(m_filterField, &QLineEdit::textChanged, this, &ProductTypesView::onFilterTextChanged);
    connect(&m_viewModel, &ProductTypesViewModel::productTypesChanged, this,
            &ProductTypesView::onProductTypesChanged);
    connect(m_dataTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &ProductTypesView::onSelectionChanged);

    updateActionButtons();
    updateStatus(0, 0);
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
    m_allProductTypes = m_viewModel.productTypes();
    applyFilter();
}

void ProductTypesView::onFilterTextChanged(const QString &text)
{
    Q_UNUSED(text);
    applyFilter();
}

void ProductTypesView::onSelectionChanged() { updateActionButtons(); }

void ProductTypesView::applyFilter()
{
    QVector<DisplayData::ProductType> filteredProductTypes;
    const QString filterText = m_filterField->text().trimmed();

    for (const auto &productType : m_allProductTypes) {
        if (matchesFilter(productType, filterText)) {
            filteredProductTypes.push_back(productType);
        }
    }

    fillTable(filteredProductTypes);
    updateStatus(filteredProductTypes.size(), m_allProductTypes.size());
}

void ProductTypesView::fillTable(const QVector<DisplayData::ProductType> &productTypes)
{
    m_dataTable->setSortingEnabled(false);
    m_dataTable->setUpdatesEnabled(false);
    m_dataTable->clearContents();
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
    m_dataTable->setUpdatesEnabled(true);
    m_dataTable->setSortingEnabled(true);
    updateActionButtons();
}

bool ProductTypesView::showProductTypeDialog(DisplayData::ProductType &productType,
                                             const QString &title)
{
    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setObjectName("productTypeDialog");
    dialog.setMinimumWidth(480);

    auto *layout            = new QFormLayout(&dialog);
    auto *codeField         = new QLineEdit(productType.code, &dialog);
    auto *nameField         = new QLineEdit(productType.name, &dialog);
    auto *barcodeField      = new QLineEdit(productType.barcode, &dialog);
    auto *unitField         = new QLineEdit(productType.unit, &dialog);
    auto *priceField        = new QLineEdit(productType.lastPrice, &dialog);
    auto *descriptionField = new QPlainTextEdit(productType.description, &dialog);
    auto *importedCheckbox = new QCheckBox(&dialog);
    importedCheckbox->setChecked(productType.isImported.compare("Yes", Qt::CaseInsensitive) == 0);
    priceField->setValidator(new QDoubleValidator(0.0, 1'000'000.0, 2, priceField));
    descriptionField->setTabChangesFocus(true);
    descriptionField->setFixedHeight(96);

    codeField->setObjectName("productTypeCodeField");
    nameField->setObjectName("productTypeNameField");
    barcodeField->setObjectName("productTypeBarcodeField");
    unitField->setObjectName("productTypeUnitField");
    priceField->setObjectName("productTypePriceField");
    descriptionField->setObjectName("productTypeDescriptionField");
    importedCheckbox->setObjectName("productTypeImportedCheckbox");

    layout->addRow(tr("Code"), codeField);
    layout->addRow(tr("Name"), nameField);
    layout->addRow(tr("Barcode"), barcodeField);
    layout->addRow(tr("Unit"), unitField);
    layout->addRow(tr("Last price"), priceField);
    layout->addRow(tr("Description"), descriptionField);
    layout->addRow(tr("Imported"), importedCheckbox);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, [this, &dialog, codeField, nameField,
                                                            unitField, priceField]() {
        if (codeField->text().trimmed().isEmpty() || nameField->text().trimmed().isEmpty() ||
            unitField->text().trimmed().isEmpty()) {
            emit errorOccurred(tr("Code, name, and unit are required."));
            return;
        }

        if (!priceField->text().trimmed().isEmpty() && !priceField->hasAcceptableInput()) {
            emit errorOccurred(tr("Last price must be a valid number."));
            return;
        }

        dialog.accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    productType.code        = codeField->text();
    productType.name        = nameField->text();
    productType.barcode     = barcodeField->text();
    productType.unit        = unitField->text();
    productType.lastPrice   = priceField->text();
    productType.description = descriptionField->toPlainText();
    productType.isImported  = importedCheckbox->isChecked() ? "Yes" : "No";

    return true;
}

void ProductTypesView::updateStatus(int visibleCount, int totalCount)
{
    m_status->setText(tr("Status: showing %1 of %2 product types")
                          .arg(QString::number(visibleCount), QString::number(totalCount)));
}

void ProductTypesView::updateActionButtons()
{
    const bool hasSelection = !m_dataTable->selectedItems().isEmpty();
    m_editButton->setEnabled(hasSelection);
    m_deleteButton->setEnabled(hasSelection);
}
