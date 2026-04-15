#include "Ui/Views/StocksView.hpp"

#include "Ui/Views/TableInteractionHelpers.hpp"

#include <QAbstractItemView>
#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidgetItem>
#include <limits>

#include "Ui/ViewModels/StocksViewModel.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
bool matchesFilter(const DisplayData::InventoryStock &stock, const QString &filterText)
{
    if (filterText.isEmpty()) {
        return true;
    }

    const QStringList haystack = {stock.productLabel, stock.quantityAvailable, stock.employeeId};
    for (const auto &value : haystack) {
        if (value.contains(filterText, Qt::CaseInsensitive)) {
            return true;
        }
    }

    return false;
}
} // namespace

StocksView::StocksView(StocksViewModel &viewModel, QWidget *parent)
    : BaseView(viewModel, parent), m_viewModel(viewModel)
{
    SPDLOG_TRACE("StocksView::StocksView");

    m_addButton->setObjectName("stocksAddButton");
    m_deleteButton->setObjectName("stocksDeleteButton");
    m_dataTable->setObjectName("stocksTable");
    m_status->setObjectName("stocksStatusLabel");

    enableInlineEditing();

    auto *filterLabel = new QLabel(tr("Filter"), this);
    m_filterField     = new QLineEdit(this);
    m_filterField->setObjectName("stocksFilterField");
    m_filterField->setPlaceholderText(tr("Search by product, quantity, or employee"));
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
    installSpinBoxDelegate(m_dataTable, 3, 0, std::numeric_limits<int>::max(),
                           QStringLiteral("stocksQuantityEditor"));
    installSpinBoxDelegate(m_dataTable, 4, 1, std::numeric_limits<int>::max(),
                           QStringLiteral("stocksEmployeeEditor"));
    installComboDelegate(
        m_dataTable, 2, QStringLiteral("stocksProductTypeCombo"),
        [this](const QModelIndex &) {
            QVector<TableComboChoice> choices;
            const auto productTypes = m_viewModel.productTypes();
            choices.reserve(productTypes.size());
            for (const auto &productType : productTypes) {
                choices.push_back(
                    {productType.code + QStringLiteral(" | ") + productType.name, productType.id});
            }
            return choices;
        },
        [this](const QModelIndex &index, const TableComboChoice &choice) {
            if (m_isSyncingTable) {
                return;
            }

            auto *productTypeIdItem = m_dataTable->item(index.row(), 1);
            if (productTypeIdItem == nullptr || choice.value.toString().isEmpty()) {
                return;
            }

            {
                const QSignalBlocker blocker(m_dataTable);
                productTypeIdItem->setText(choice.value.toString());
                productTypeIdItem->setData(Qt::UserRole, choice.value);
            }
            persistRow(index.row());
        });

    connect(m_addButton, &QPushButton::clicked, this, &StocksView::onAddButtonClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &StocksView::onDeleteButtonClicked);
    connect(m_filterField, &QLineEdit::textChanged, this, &StocksView::onFilterTextChanged);
    connect(&m_viewModel, &StocksViewModel::stocksChanged, this, &StocksView::onStocksChanged);
    connect(m_dataTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &StocksView::onSelectionChanged);
    connect(m_dataTable, &QTableWidget::itemChanged, this, &StocksView::onItemChanged);

    updateActionButtons();
    updateStatus(0, 0);
}

void StocksView::onAddButtonClicked()
{
    const auto productTypes = m_viewModel.productTypes();
    if (productTypes.isEmpty()) {
        m_status->setText(tr("Status: create a product type before adding stock"));
        return;
    }

    QString availableProductTypeId;
    QString availableProductLabel;
    for (const auto &productType : productTypes) {
        const bool alreadyTracked =
            std::any_of(m_allStocks.cbegin(), m_allStocks.cend(), [&](const auto &stock) {
                return stock.productTypeId == productType.id;
            });
        if (!alreadyTracked) {
            availableProductTypeId = productType.id;
            availableProductLabel = productType.code + " | " + productType.name;
            break;
        }
    }

    if (availableProductTypeId.isEmpty()) {
        emit errorOccurred(tr("All product types already have stock records."));
        return;
    }

    DisplayData::InventoryStock stock;
    stock.productTypeId = availableProductTypeId;
    stock.productLabel = availableProductLabel;
    stock.quantityAvailable = QStringLiteral("0");
    stock.employeeId = QStringLiteral("1");
    m_viewModel.createStock(stock);
}

void StocksView::onDeleteButtonClicked()
{
    const auto selectedItems = m_dataTable->selectedItems();
    if (!selectedItems.isEmpty()) {
        const int row = selectedItems.first()->row();
        m_viewModel.deleteStock(m_dataTable->item(row, 0)->text());
    } else {
        SPDLOG_WARN("No stock record selected for deletion.");
    }
}

void StocksView::onFilterTextChanged(const QString &text)
{
    Q_UNUSED(text);
    applyFilter();
}

void StocksView::onStocksChanged()
{
    m_allStocks = m_viewModel.stocks();
    applyFilter();
}

void StocksView::onSelectionChanged() { updateActionButtons(); }

void StocksView::onItemChanged(QTableWidgetItem *item)
{
    if (m_isSyncingTable || item == nullptr || (item->column() != 3 && item->column() != 4)) {
        return;
    }

    const QString currentValue = item->text().trimmed();
    bool ok = false;
    currentValue.toInt(&ok);
    const bool valid = ok && (item->column() != 4 || currentValue.toInt() > 0);
    if (!valid) {
        restoreItemText(m_dataTable, item, item->data(Qt::UserRole).toString());
        emit errorOccurred(item->column() == 3
                               ? tr("Quantity must be a valid non-negative integer.")
                               : tr("Employee ID must be a valid positive integer."));
        return;
    }
    if (restoreIfUnchanged(m_dataTable, item, currentValue)) {
        return;
    }

    if (item->text() != currentValue) {
        restoreItemText(m_dataTable, item, currentValue);
    }
    {
        const QSignalBlocker blocker(m_dataTable);
        item->setData(Qt::UserRole, currentValue);
    }
    persistRow(item->row());
}

void StocksView::applyFilter()
{
    QVector<DisplayData::InventoryStock> filteredStocks;
    const QString filterText = m_filterField->text().trimmed();

    for (const auto &stock : m_allStocks) {
        if (matchesFilter(stock, filterText)) {
            filteredStocks.push_back(stock);
        }
    }

    fillTable(filteredStocks);
    updateStatus(filteredStocks.size(), m_allStocks.size());
}

void StocksView::fillTable(const QVector<DisplayData::InventoryStock> &stocks)
{
    m_isSyncingTable = true;
    m_dataTable->setUpdatesEnabled(false);
    m_dataTable->clearContents();
    m_dataTable->setRowCount(stocks.size());
    m_dataTable->setColumnCount(DisplayData::InventoryStock::VAR_COUNT);
    m_dataTable->setHorizontalHeaderLabels(
        {tr("ID"), tr("Product Type ID"), tr("Product"), tr("Quantity"), tr("Employee ID")});

    for (int row = 0; row < stocks.size(); ++row) {
        const auto &stock = stocks[row];
        m_dataTable->setItem(row, 0, createReadOnlyTableItem(stock.id));
        m_dataTable->setItem(row, 1, createReadOnlyTableItem(stock.productTypeId));
        m_dataTable->setItem(row, 2, createComboTableItem(stock.productLabel, stock.productTypeId));
        m_dataTable->setItem(row, 3, createEditableTableItem(stock.quantityAvailable));
        m_dataTable->setItem(row, 4, createEditableTableItem(stock.employeeId));
    }

    m_dataTable->setColumnHidden(0, true);
    m_dataTable->setColumnHidden(1, true);
    m_dataTable->setUpdatesEnabled(true);
    refreshPersistentComboEditors(m_dataTable);
    m_isSyncingTable = false;
    updateActionButtons();
}

void StocksView::persistRow(int row)
{
    if (row < 0) {
        return;
    }

    DisplayData::InventoryStock stock;
    stock.id = m_dataTable->item(row, 0)->text();
    stock.productTypeId = m_dataTable->item(row, 1)->text();
    stock.productLabel = m_dataTable->item(row, 2)->text();
    stock.quantityAvailable = m_dataTable->item(row, 3)->text().trimmed();
    stock.employeeId = m_dataTable->item(row, 4)->text().trimmed();

    if (stock.productTypeId.isEmpty() || stock.quantityAvailable.isEmpty() || stock.employeeId.isEmpty()) {
        emit errorOccurred(tr("Quantity and employee ID are required."));
        return;
    }

    m_viewModel.editStock(stock);
}

void StocksView::updateStatus(int visibleCount, int totalCount)
{
    m_status->setText(tr("Status: showing %1 of %2 stock records")
                          .arg(QString::number(visibleCount), QString::number(totalCount)));
}

void StocksView::updateActionButtons()
{
    const bool hasSelection = !m_dataTable->selectedItems().isEmpty();
    m_deleteButton->setEnabled(hasSelection);
}
