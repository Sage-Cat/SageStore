#include "Ui/Views/SupplierCatalogView.hpp"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QVBoxLayout>
#include <algorithm>

#include "Ui/ViewModels/SupplierCatalogViewModel.hpp"

namespace {
bool matchesFilter(const Common::Entities::SuppliersProductInfo &supplierProduct,
                   const SupplierCatalogViewModel &viewModel, const QString &filterText)
{
    if (filterText.isEmpty()) {
        return true;
    }

    const QStringList haystack = {QString::fromStdString(supplierProduct.code),
                                  viewModel.supplierLabel(
                                      QString::fromStdString(supplierProduct.supplierID)),
                                  viewModel.productTypeLabel(
                                      QString::fromStdString(supplierProduct.productTypeId))};

    return std::any_of(haystack.begin(), haystack.end(), [&](const QString &value) {
        return value.contains(filterText, Qt::CaseInsensitive);
    });
}
} // namespace

SupplierCatalogView::SupplierCatalogView(SupplierCatalogViewModel &viewModel, QWidget *parent)
    : QWidget(parent), m_viewModel(viewModel)
{
    setupUi();

    connect(this, &SupplierCatalogView::errorOccurred, &m_viewModel,
            &SupplierCatalogViewModel::errorOccurred);
    connect(&m_viewModel, &SupplierCatalogViewModel::supplierProductsChanged, this,
            &SupplierCatalogView::onSupplierProductsChanged);
    connect(m_addButton, &QPushButton::clicked, this, &SupplierCatalogView::onAddClicked);
    connect(m_editButton, &QPushButton::clicked, this, &SupplierCatalogView::onEditClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &SupplierCatalogView::onDeleteClicked);
    connect(m_importButton, &QPushButton::clicked, this, &SupplierCatalogView::onImportCsvClicked);
    connect(m_filterField, &QLineEdit::textChanged, this, &SupplierCatalogView::onFilterChanged);
    connect(m_table->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &SupplierCatalogView::onSelectionChanged);
}

void SupplierCatalogView::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    auto *toolbar = new QHBoxLayout;

    auto *titleLabel = new QLabel(tr("Supplier Catalog"), this);
    titleLabel->setObjectName("supplierCatalogTitleLabel");

    m_filterField = new QLineEdit(this);
    m_filterField->setObjectName("supplierCatalogFilterField");
    m_filterField->setPlaceholderText(tr("Search by supplier, product type, or supplier code"));
    m_filterField->setClearButtonEnabled(true);

    m_addButton = new QPushButton(tr("Add"), this);
    m_addButton->setObjectName("supplierCatalogAddButton");
    m_editButton = new QPushButton(tr("Edit"), this);
    m_editButton->setObjectName("supplierCatalogEditButton");
    m_deleteButton = new QPushButton(tr("Delete"), this);
    m_deleteButton->setObjectName("supplierCatalogDeleteButton");
    m_deleteButton->setProperty("destructive", true);
    m_importButton = new QPushButton(tr("Import CSV"), this);
    m_importButton->setObjectName("supplierCatalogImportButton");

    toolbar->addWidget(titleLabel);
    toolbar->addStretch();
    toolbar->addWidget(m_filterField);
    toolbar->addWidget(m_addButton);
    toolbar->addWidget(m_editButton);
    toolbar->addWidget(m_deleteButton);
    toolbar->addWidget(m_importButton);

    m_table = new QTableWidget(this);
    m_table->setObjectName("supplierCatalogTable");
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->setSortingEnabled(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_statusLabel = new QLabel(tr("Status: ready"), this);
    m_statusLabel->setObjectName("supplierCatalogStatusLabel");

    layout->addLayout(toolbar);
    layout->addWidget(m_table);
    layout->addWidget(m_statusLabel);

    setObjectName("supplierCatalogView");
    setStyleSheet(
        "#supplierCatalogView { background-color: #fffaf2; }"
        "#supplierCatalogTitleLabel { color: #233130; font-size: 22px; font-weight: 700; }"
        "QLineEdit, QComboBox { background-color: white; border: 1px solid #cbd5e1; border-radius: 6px; padding: 6px; }"
        "QPushButton { background-color: #1d4ed8; color: white; border: 0; border-radius: 6px; padding: 6px 12px; }"
        "QPushButton:hover { background-color: #1e40af; }"
        "QPushButton[destructive='true'] { background-color: #b91c1c; }"
        "QPushButton[destructive='true']:hover { background-color: #991b1b; }"
        "QTableWidget { background-color: #fbfbfd; alternate-background-color: #f1f4f8; border: 1px solid #d7dde7; border-radius: 8px; }"
        "QHeaderView::section { background-color: #e8eef6; color: #0f172a; padding: 6px; border: 0; border-bottom: 1px solid #d7dde7; font-weight: 600; }");

    updateActionButtons();
}

void SupplierCatalogView::onSupplierProductsChanged()
{
    m_allSupplierProducts = m_viewModel.supplierProducts();
    applyFilter();
}

void SupplierCatalogView::onAddClicked()
{
    Common::Entities::SuppliersProductInfo supplierProduct;
    if (showSupplierProductDialog(supplierProduct, tr("Create supplier catalog mapping"))) {
        m_viewModel.createSupplierProduct(supplierProduct);
    }
}

void SupplierCatalogView::onEditClicked()
{
    const auto selected = m_table->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    const int row = selected.first()->row();
    Common::Entities::SuppliersProductInfo supplierProduct{
        .id = m_table->item(row, 0)->text().toStdString(),
        .supplierID = m_table->item(row, 1)->text().toStdString(),
        .productTypeId = m_table->item(row, 3)->text().toStdString(),
        .code = m_table->item(row, 5)->text().toStdString()};
    if (showSupplierProductDialog(supplierProduct, tr("Edit supplier catalog mapping"))) {
        m_viewModel.editSupplierProduct(supplierProduct);
    }
}

void SupplierCatalogView::onDeleteClicked()
{
    const auto selected = m_table->selectedItems();
    if (!selected.isEmpty()) {
        m_viewModel.deleteSupplierProduct(m_table->item(selected.first()->row(), 0)->text());
    }
}

void SupplierCatalogView::onImportCsvClicked()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this, tr("Import supplier catalog CSV"), QString(), tr("CSV files (*.csv);;All files (*)"));
    if (filePath.isEmpty()) {
        return;
    }

    if (importCsvFile(filePath)) {
        m_statusLabel->setText(tr("Status: CSV import submitted"));
    }
}

void SupplierCatalogView::onFilterChanged(const QString &text)
{
    Q_UNUSED(text);
    applyFilter();
}

void SupplierCatalogView::onSelectionChanged() { updateActionButtons(); }

void SupplierCatalogView::applyFilter()
{
    QVector<Common::Entities::SuppliersProductInfo> filtered;
    const QString filterText = m_filterField->text().trimmed();
    for (const auto &supplierProduct : m_allSupplierProducts) {
        if (matchesFilter(supplierProduct, m_viewModel, filterText)) {
            filtered.push_back(supplierProduct);
        }
    }

    fillTable(filtered);
    updateStatus(filtered.size(), m_allSupplierProducts.size());
}

void SupplierCatalogView::updateActionButtons()
{
    const bool hasSelection = !m_table->selectedItems().isEmpty();
    m_editButton->setEnabled(hasSelection);
    m_deleteButton->setEnabled(hasSelection);
}

void SupplierCatalogView::updateStatus(int visibleCount, int totalCount)
{
    m_statusLabel->setText(
        tr("Status: showing %1 of %2 supplier catalog mappings")
            .arg(QString::number(visibleCount), QString::number(totalCount)));
}

void SupplierCatalogView::fillTable(
    const QVector<Common::Entities::SuppliersProductInfo> &supplierProducts)
{
    m_table->setSortingEnabled(false);
    m_table->clearContents();
    m_table->setRowCount(supplierProducts.size());
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels(
        {tr("ID"), tr("Supplier ID"), tr("Supplier"), tr("Product Type ID"), tr("Product Type"),
         tr("Supplier Code")});

    for (int row = 0; row < supplierProducts.size(); ++row) {
        const auto &supplierProduct = supplierProducts[row];
        const QString supplierId = QString::fromStdString(supplierProduct.supplierID);
        const QString productTypeId = QString::fromStdString(supplierProduct.productTypeId);
        m_table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(supplierProduct.id)));
        m_table->setItem(row, 1, new QTableWidgetItem(supplierId));
        m_table->setItem(row, 2, new QTableWidgetItem(m_viewModel.supplierLabel(supplierId)));
        m_table->setItem(row, 3, new QTableWidgetItem(productTypeId));
        m_table->setItem(row, 4,
                         new QTableWidgetItem(m_viewModel.productTypeLabel(productTypeId)));
        m_table->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(supplierProduct.code)));
    }

    m_table->setColumnHidden(0, true);
    m_table->setColumnHidden(1, true);
    m_table->setColumnHidden(3, true);
    m_table->setSortingEnabled(true);
    updateActionButtons();
}

bool SupplierCatalogView::showSupplierProductDialog(
    Common::Entities::SuppliersProductInfo &supplierProduct, const QString &title)
{
    const auto suppliers = m_viewModel.suppliers();
    const auto productTypes = m_viewModel.productTypes();
    if (suppliers.isEmpty() || productTypes.isEmpty()) {
        emit errorOccurred(tr("Suppliers and product types must exist before catalog mapping."));
        return false;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setMinimumWidth(460);

    auto *layout = new QFormLayout(&dialog);
    auto *supplierBox = new QComboBox(&dialog);
    auto *productTypeBox = new QComboBox(&dialog);
    auto *codeField = new QLineEdit(QString::fromStdString(supplierProduct.code), &dialog);

    for (const auto &supplier : suppliers) {
        supplierBox->addItem(QString::fromStdString(supplier.name), QString::fromStdString(supplier.id));
    }
    for (const auto &productType : productTypes) {
        productTypeBox->addItem(
            QString::fromStdString(productType.code) + " | " + QString::fromStdString(productType.name),
            QString::fromStdString(productType.id));
    }

    if (!supplierProduct.supplierID.empty()) {
        supplierBox->setCurrentIndex(
            supplierBox->findData(QString::fromStdString(supplierProduct.supplierID)));
    }
    if (!supplierProduct.productTypeId.empty()) {
        productTypeBox->setCurrentIndex(
            productTypeBox->findData(QString::fromStdString(supplierProduct.productTypeId)));
    }

    layout->addRow(tr("Supplier"), supplierBox);
    layout->addRow(tr("Product type"), productTypeBox);
    layout->addRow(tr("Supplier code"), codeField);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, [this, &dialog, codeField]() {
        if (codeField->text().trimmed().isEmpty()) {
            emit errorOccurred(tr("Supplier code is required."));
            return;
        }
        dialog.accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    supplierProduct.supplierID = supplierBox->currentData().toString().toStdString();
    supplierProduct.productTypeId = productTypeBox->currentData().toString().toStdString();
    supplierProduct.code = codeField->text().trimmed().toStdString();
    return true;
}

bool SupplierCatalogView::importCsvFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred(tr("Unable to open CSV file."));
        return false;
    }

    QTextStream stream(&file);
    int importedCount = 0;
    while (!stream.atEnd()) {
        const QString line = stream.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }

        const QStringList columns = line.split(',');
        if (columns.size() < 3) {
            continue;
        }
        if (columns[0].compare("supplierId", Qt::CaseInsensitive) == 0) {
            continue;
        }

        Common::Entities::SuppliersProductInfo supplierProduct{
            .id = "",
            .supplierID = columns[0].trimmed().toStdString(),
            .productTypeId = columns[1].trimmed().toStdString(),
            .code = columns[2].trimmed().toStdString()};
        m_viewModel.createSupplierProduct(supplierProduct);
        ++importedCount;
    }

    if (importedCount == 0) {
        emit errorOccurred(tr("CSV file did not contain valid supplier catalog rows."));
        return false;
    }

    return true;
}
