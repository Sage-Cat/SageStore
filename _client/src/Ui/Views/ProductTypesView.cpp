#include "Ui/Views/ProductTypesView.hpp"

#include "Ui/Views/TableInteractionHelpers.hpp"

#include <QAbstractItemView>
#include <QDateTime>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidgetItem>

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

QString normalizedPriceText(const QString &value)
{
    bool ok = false;
    const double price = value.trimmed().toDouble(&ok);
    if (!ok) {
        return value;
    }

    return QString::number(price, 'f', 2);
}
} // namespace

ProductTypesView::ProductTypesView(ProductTypesViewModel &viewModel, QWidget *parent)
    : BaseView(viewModel, parent), m_viewModel(viewModel)
{
    SPDLOG_TRACE("ProductTypesView::ProductTypesView");

    m_addButton->setObjectName("productTypesAddButton");
    m_deleteButton->setObjectName("productTypesDeleteButton");
    m_dataTable->setObjectName("productTypesTable");
    m_status->setObjectName("productTypesStatusLabel");

    enableInlineEditing();

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
    m_dataTable->setAlternatingRowColors(true);
    m_dataTable->setSortingEnabled(false);
    m_dataTable->verticalHeader()->setVisible(false);
    installDoubleSpinBoxDelegate(m_dataTable, 5, 0.0, 1000000000.0, 2, 0.25,
                                 QStringLiteral("productTypesPriceEditor"));

    connect(m_addButton, &QPushButton::clicked, this, &ProductTypesView::onAddButtonClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &ProductTypesView::onDeleteButtonClicked);
    connect(m_filterField, &QLineEdit::textChanged, this, &ProductTypesView::onFilterTextChanged);
    connect(&m_viewModel, &ProductTypesViewModel::productTypesChanged, this,
            &ProductTypesView::onProductTypesChanged);
    connect(m_dataTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &ProductTypesView::onSelectionChanged);
    connect(m_dataTable, &QTableWidget::itemChanged, this, &ProductTypesView::onItemChanged);

    updateActionButtons();
    updateStatus(0, 0);
}

void ProductTypesView::onAddButtonClicked()
{
    const QString token = QDateTime::currentDateTimeUtc().toString(QStringLiteral("hhmmsszzz"));
    DisplayData::ProductType productType;
    productType.code = QStringLiteral("PT-%1").arg(token);
    productType.name = tr("New product %1").arg(token);
    productType.barcode = {};
    productType.unit = QStringLiteral("pcs");
    productType.lastPrice = QStringLiteral("0.00");
    productType.isImported = tr("No");
    productType.description = {};
    m_viewModel.createProductType(productType);
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

void ProductTypesView::onItemChanged(QTableWidgetItem *item)
{
    if (m_isSyncingTable || item == nullptr) {
        return;
    }

    const int row = item->row();
    const QString previousValue = item->data(Qt::UserRole).toString();
    QString currentValue = item->column() == 6
                               ? (item->checkState() == Qt::Checked ? tr("Yes") : tr("No"))
                               : item->text().trimmed();

    if (item->column() != 6 && item->text() != currentValue) {
        restoreItemText(m_dataTable, item, currentValue);
    }

    const QString code = m_dataTable->item(row, 1)->text().trimmed();
    const QString name = m_dataTable->item(row, 2)->text().trimmed();
    const QString unit = m_dataTable->item(row, 4)->text().trimmed();
    const QString price = normalizedPriceText(m_dataTable->item(row, 5)->text());

    if (item->column() != 6 && currentValue.isEmpty() &&
        (item->column() == 1 || item->column() == 2 || item->column() == 4)) {
        restoreItemText(m_dataTable, item, previousValue);
        emit errorOccurred(tr("Code, name, and unit are required."));
        return;
    }

    if (!price.isEmpty()) {
        bool ok = false;
        price.toDouble(&ok);
        if (!ok) {
            restoreItemText(m_dataTable, m_dataTable->item(row, 5),
                            m_dataTable->item(row, 5)->data(Qt::UserRole).toString());
            emit errorOccurred(tr("Last price must be a valid number."));
            return;
        }
    }

    if (code.isEmpty() || name.isEmpty() || unit.isEmpty()) {
        restoreItemText(m_dataTable, item, previousValue);
        emit errorOccurred(tr("Code, name, and unit are required."));
        return;
    }

    if (item->column() == 6) {
        if (previousValue == currentValue) {
            return;
        }
    } else {
        const QString normalizedCurrentValue = item->column() == 5 ? price : currentValue;
        if (restoreIfUnchanged(m_dataTable, item, normalizedCurrentValue)) {
            return;
        }
    }

    {
        const QSignalBlocker blocker(m_dataTable);
        item->setData(Qt::UserRole, currentValue);
        if (item->column() == 6) {
            item->setText(currentValue);
        } else if (item->column() == 5) {
            item->setText(price);
            item->setData(Qt::UserRole, price);
        }
    }

    DisplayData::ProductType productType;
    productType.id = m_dataTable->item(row, 0)->text();
    productType.code = code;
    productType.name = name;
    productType.barcode = m_dataTable->item(row, 3)->text().trimmed();
    productType.unit = unit;
    productType.lastPrice = price;
    productType.isImported = m_dataTable->item(row, 6)->text();
    productType.description = m_dataTable->item(row, 7)->text().trimmed();
    m_viewModel.editProductType(productType);
}

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
    m_isSyncingTable = true;
    m_dataTable->setUpdatesEnabled(false);
    m_dataTable->clearContents();
    m_dataTable->setRowCount(productTypes.size());
    m_dataTable->setColumnCount(DisplayData::ProductType::VAR_COUNT);
    m_dataTable->setHorizontalHeaderLabels(
        {tr("ID"), tr("Code"), tr("Name"), tr("Barcode"), tr("Unit"), tr("Last price"),
         tr("Imported"), tr("Description")});

    for (int row = 0; row < productTypes.size(); ++row) {
        const auto &productType = productTypes[row];
        m_dataTable->setItem(row, 0, createReadOnlyTableItem(productType.id));
        m_dataTable->setItem(row, 1, createEditableTableItem(productType.code));
        m_dataTable->setItem(row, 2, createEditableTableItem(productType.name));
        m_dataTable->setItem(row, 3, createEditableTableItem(productType.barcode));
        m_dataTable->setItem(row, 4, createEditableTableItem(productType.unit));
        m_dataTable->setItem(row, 5, createEditableTableItem(normalizedPriceText(productType.lastPrice)));
        auto *importedItem = createReadOnlyTableItem(productType.isImported);
        importedItem->setFlags((importedItem->flags() | Qt::ItemIsUserCheckable) &
                               ~Qt::ItemIsEditable);
        importedItem->setCheckState(productType.isImported.compare(tr("Yes"), Qt::CaseInsensitive) == 0
                                        ? Qt::Checked
                                        : Qt::Unchecked);
        m_dataTable->setItem(row, 6, importedItem);
        m_dataTable->setItem(row, 7, createEditableTableItem(productType.description));
    }

    m_dataTable->setColumnHidden(0, true);
    m_dataTable->setUpdatesEnabled(true);
    m_isSyncingTable = false;
    updateActionButtons();
}

void ProductTypesView::updateStatus(int visibleCount, int totalCount)
{
    m_status->setText(tr("Status: showing %1 of %2 product types")
                          .arg(QString::number(visibleCount), QString::number(totalCount)));
}

void ProductTypesView::updateActionButtons()
{
    const bool hasSelection = !m_dataTable->selectedItems().isEmpty();
    m_deleteButton->setEnabled(hasSelection);
}
