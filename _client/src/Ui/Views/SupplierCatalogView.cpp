#include "Ui/Views/SupplierCatalogView.hpp"

#include "Ui/Views/TableInteractionHelpers.hpp"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
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
    installComboDelegate(
        m_table, 2, QStringLiteral("supplierCatalogSupplierCombo"),
        [this](const QModelIndex &) {
            QVector<TableComboChoice> choices;
            const auto suppliers = m_viewModel.suppliers();
            choices.reserve(suppliers.size());
            for (const auto &supplier : suppliers) {
                choices.push_back(
                    {QString::fromStdString(supplier.name), QString::fromStdString(supplier.id)});
            }
            return choices;
        },
        [this](const QModelIndex &index, const TableComboChoice &choice) {
            if (m_isSyncingTable) {
                return;
            }

            auto *supplierIdItem = m_table->item(index.row(), 1);
            if (supplierIdItem == nullptr || choice.value.toString().isEmpty()) {
                return;
            }

            {
                const QSignalBlocker blocker(m_table);
                supplierIdItem->setText(choice.value.toString());
                supplierIdItem->setData(Qt::UserRole, choice.value);
            }
            persistRow(index.row());
        });
    installComboDelegate(
        m_table, 4, QStringLiteral("supplierCatalogProductTypeCombo"),
        [this](const QModelIndex &) {
            QVector<TableComboChoice> choices;
            const auto productTypes = m_viewModel.productTypes();
            choices.reserve(productTypes.size());
            for (const auto &productType : productTypes) {
                choices.push_back({QString::fromStdString(productType.code) + QStringLiteral(" | ") +
                                       QString::fromStdString(productType.name),
                                   QString::fromStdString(productType.id)});
            }
            return choices;
        },
        [this](const QModelIndex &index, const TableComboChoice &choice) {
            if (m_isSyncingTable) {
                return;
            }

            auto *productTypeIdItem = m_table->item(index.row(), 3);
            if (productTypeIdItem == nullptr || choice.value.toString().isEmpty()) {
                return;
            }

            {
                const QSignalBlocker blocker(m_table);
                productTypeIdItem->setText(choice.value.toString());
                productTypeIdItem->setData(Qt::UserRole, choice.value);
            }
            persistRow(index.row());
        });

    connect(this, &SupplierCatalogView::errorOccurred, &m_viewModel,
            &SupplierCatalogViewModel::errorOccurred);
    connect(&m_viewModel, &SupplierCatalogViewModel::supplierProductsChanged, this,
            &SupplierCatalogView::onSupplierProductsChanged);
    connect(m_addButton, &QPushButton::clicked, this, &SupplierCatalogView::onAddClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &SupplierCatalogView::onDeleteClicked);
    connect(m_importButton, &QPushButton::clicked, this, &SupplierCatalogView::onImportCsvClicked);
    connect(m_filterField, &QLineEdit::textChanged, this, &SupplierCatalogView::onFilterChanged);
    connect(m_table->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &SupplierCatalogView::onSelectionChanged);
    connect(m_table, &QTableWidget::itemChanged, this, &SupplierCatalogView::onItemChanged);
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
    m_deleteButton = new QPushButton(tr("Delete"), this);
    m_deleteButton->setObjectName("supplierCatalogDeleteButton");
    m_deleteButton->setProperty("destructive", true);
    m_importButton = new QPushButton(tr("Import CSV"), this);
    m_importButton->setObjectName("supplierCatalogImportButton");

    toolbar->addWidget(titleLabel);
    toolbar->addStretch();
    toolbar->addWidget(m_filterField);
    toolbar->addWidget(m_addButton);
    toolbar->addWidget(m_deleteButton);
    toolbar->addWidget(m_importButton);

    m_table = new QTableWidget(this);
    m_table->setObjectName("supplierCatalogTable");
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setAlternatingRowColors(true);
    m_table->setSortingEnabled(false);
    m_table->verticalHeader()->setVisible(false);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    configureEditableTable(m_table, QStringLiteral("supplierCatalogInlineEditor"));

    m_statusLabel = new QLabel(tr("Status: ready"), this);
    m_statusLabel->setObjectName("supplierCatalogStatusLabel");
    m_statusLabel->setProperty("muted", true);

    layout->addLayout(toolbar);
    layout->addWidget(m_table);
    layout->addWidget(m_statusLabel);

    setObjectName("supplierCatalogView");
    setStyleSheet(
        "#supplierCatalogView { background-color: transparent; }"
        "#supplierCatalogTitleLabel { color: #0f172a; font-size: 22px; font-weight: 700; }");

    updateActionButtons();
}

void SupplierCatalogView::onSupplierProductsChanged()
{
    m_allSupplierProducts = m_viewModel.supplierProducts();
    applyFilter();
}

void SupplierCatalogView::onAddClicked()
{
    if (m_viewModel.suppliers().isEmpty() || m_viewModel.productTypes().isEmpty()) {
        emit errorOccurred(tr("Suppliers and product types must exist before catalog mapping."));
        return;
    }

    const QString token = QDateTime::currentDateTimeUtc().toString(QStringLiteral("hhmmsszzz"));
    Common::Entities::SuppliersProductInfo supplierProduct;
    supplierProduct.supplierID = m_viewModel.suppliers().first().id;
    supplierProduct.productTypeId = m_viewModel.productTypes().first().id;
    supplierProduct.code = QStringLiteral("CODE-%1").arg(token).toStdString();
    m_viewModel.createSupplierProduct(supplierProduct);
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

void SupplierCatalogView::onItemChanged(QTableWidgetItem *item)
{
    if (m_isSyncingTable || item == nullptr || item->column() != 5) {
        return;
    }

    const QString currentValue = item->text().trimmed();
    if (currentValue.isEmpty()) {
        restoreItemText(m_table, item, item->data(Qt::UserRole).toString());
        emit errorOccurred(tr("Supplier code is required."));
        return;
    }
    if (restoreIfUnchanged(m_table, item, currentValue)) {
        return;
    }

    if (item->text() != currentValue) {
        restoreItemText(m_table, item, currentValue);
    }
    {
        const QSignalBlocker blocker(m_table);
        item->setData(Qt::UserRole, currentValue);
    }
    persistRow(item->row());
}

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
    m_isSyncingTable = true;
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
        m_table->setItem(row, 0, createReadOnlyTableItem(QString::fromStdString(supplierProduct.id)));
        m_table->setItem(row, 1, createReadOnlyTableItem(supplierId));
        m_table->setItem(row, 2,
                         createComboTableItem(m_viewModel.supplierLabel(supplierId), supplierId));
        m_table->setItem(row, 3, createReadOnlyTableItem(productTypeId));
        m_table->setItem(row, 4,
                         createComboTableItem(m_viewModel.productTypeLabel(productTypeId),
                                              productTypeId));
        m_table->setItem(row, 5,
                         createEditableTableItem(QString::fromStdString(supplierProduct.code)));
    }

    m_table->setColumnHidden(0, true);
    m_table->setColumnHidden(1, true);
    m_table->setColumnHidden(3, true);
    refreshPersistentComboEditors(m_table);
    m_isSyncingTable = false;
    updateActionButtons();
}

void SupplierCatalogView::persistRow(int row)
{
    if (row < 0) {
        return;
    }

    Common::Entities::SuppliersProductInfo supplierProduct{
        .id = m_table->item(row, 0)->text().toStdString(),
        .supplierID = m_table->item(row, 1)->text().toStdString(),
        .productTypeId = m_table->item(row, 3)->text().toStdString(),
        .code = m_table->item(row, 5)->text().trimmed().toStdString()};
    if (supplierProduct.code.empty()) {
        emit errorOccurred(tr("Supplier code is required."));
        return;
    }
    m_viewModel.editSupplierProduct(supplierProduct);
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
