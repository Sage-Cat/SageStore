#include "Ui/Views/StocksView.hpp"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIntValidator>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidgetItem>

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
    m_editButton->setObjectName("stocksEditButton");
    m_deleteButton->setObjectName("stocksDeleteButton");
    m_dataTable->setObjectName("stocksTable");
    m_status->setObjectName("stocksStatusLabel");

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
    m_dataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_dataTable->setAlternatingRowColors(true);
    m_dataTable->setSortingEnabled(true);
    m_dataTable->verticalHeader()->setVisible(false);

    connect(m_addButton, &QPushButton::clicked, this, &StocksView::onAddButtonClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &StocksView::onDeleteButtonClicked);
    connect(m_editButton, &QPushButton::clicked, this, &StocksView::onEditButtonClicked);
    connect(m_filterField, &QLineEdit::textChanged, this, &StocksView::onFilterTextChanged);
    connect(&m_viewModel, &StocksViewModel::stocksChanged, this, &StocksView::onStocksChanged);
    connect(m_dataTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &StocksView::onSelectionChanged);

    updateActionButtons();
    updateStatus(0, 0);
}

void StocksView::onAddButtonClicked()
{
    DisplayData::InventoryStock stock;
    stock.employeeId = "1";
    if (showStockDialog(stock, tr("Create stock record"))) {
        m_viewModel.createStock(stock);
    }
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

void StocksView::onEditButtonClicked()
{
    const auto selectedItems = m_dataTable->selectedItems();
    if (selectedItems.isEmpty()) {
        SPDLOG_WARN("No stock record selected for editing.");
        return;
    }

    const int row = selectedItems.first()->row();
    DisplayData::InventoryStock stock;
    stock.id                = m_dataTable->item(row, 0)->text();
    stock.productTypeId     = m_dataTable->item(row, 1)->text();
    stock.productLabel      = m_dataTable->item(row, 2)->text();
    stock.quantityAvailable = m_dataTable->item(row, 3)->text();
    stock.employeeId        = m_dataTable->item(row, 4)->text();

    if (showStockDialog(stock, tr("Edit stock record"))) {
        m_viewModel.editStock(stock);
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
    m_dataTable->setSortingEnabled(false);
    m_dataTable->setUpdatesEnabled(false);
    m_dataTable->clearContents();
    m_dataTable->setRowCount(stocks.size());
    m_dataTable->setColumnCount(DisplayData::InventoryStock::VAR_COUNT);
    m_dataTable->setHorizontalHeaderLabels(
        {"ID", "Product Type ID", "Product", "Quantity", "Employee ID"});

    for (int row = 0; row < stocks.size(); ++row) {
        const auto &stock = stocks[row];
        m_dataTable->setItem(row, 0, new QTableWidgetItem(stock.id));
        m_dataTable->setItem(row, 1, new QTableWidgetItem(stock.productTypeId));
        m_dataTable->setItem(row, 2, new QTableWidgetItem(stock.productLabel));
        m_dataTable->setItem(row, 3, new QTableWidgetItem(stock.quantityAvailable));
        m_dataTable->setItem(row, 4, new QTableWidgetItem(stock.employeeId));
    }

    m_dataTable->setColumnHidden(0, true);
    m_dataTable->setColumnHidden(1, true);
    m_dataTable->setUpdatesEnabled(true);
    m_dataTable->setSortingEnabled(true);
    updateActionButtons();
}

void StocksView::updateStatus(int visibleCount, int totalCount)
{
    m_status->setText(tr("Status: showing %1 of %2 stock records")
                          .arg(QString::number(visibleCount), QString::number(totalCount)));
}

void StocksView::updateActionButtons()
{
    const bool hasSelection = !m_dataTable->selectedItems().isEmpty();
    m_editButton->setEnabled(hasSelection);
    m_deleteButton->setEnabled(hasSelection);
}

bool StocksView::showStockDialog(DisplayData::InventoryStock &stock, const QString &title)
{
    const auto productTypes = m_viewModel.productTypes();
    if (productTypes.isEmpty()) {
        m_status->setText(tr("Status: create a product type before adding stock"));
        return false;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setObjectName("stockDialog");
    dialog.setMinimumWidth(420);

    auto *layout          = new QFormLayout(&dialog);
    auto *productTypeBox  = new QComboBox(&dialog);
    auto *quantityField   = new QLineEdit(stock.quantityAvailable, &dialog);
    auto *employeeIdField =
        new QLineEdit(stock.employeeId.isEmpty() ? "1" : stock.employeeId, &dialog);

    productTypeBox->setObjectName("stockProductTypeCombo");
    quantityField->setObjectName("stockQuantityField");
    employeeIdField->setObjectName("stockEmployeeIdField");
    quantityField->setValidator(new QIntValidator(0, 1'000'000, quantityField));
    employeeIdField->setValidator(new QIntValidator(1, 1'000'000, employeeIdField));

    int currentIndex = 0;
    for (int index = 0; index < productTypes.size(); ++index) {
        const auto &productType = productTypes[index];
        const QString label     = productType.code + " | " + productType.name;
        productTypeBox->addItem(label, productType.id);
        if (!stock.productTypeId.isEmpty() && productType.id == stock.productTypeId) {
            currentIndex = index;
        }
    }
    productTypeBox->setCurrentIndex(currentIndex);

    layout->addRow(tr("Product type"), productTypeBox);
    layout->addRow(tr("Quantity"), quantityField);
    layout->addRow(tr("Employee ID"), employeeIdField);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, [this, &dialog, quantityField,
                                                            employeeIdField]() {
        if (quantityField->text().trimmed().isEmpty() || employeeIdField->text().trimmed().isEmpty()) {
            emit errorOccurred(tr("Quantity and employee ID are required."));
            return;
        }

        if (!quantityField->hasAcceptableInput() || !employeeIdField->hasAcceptableInput()) {
            emit errorOccurred(tr("Quantity and employee ID must be valid positive integers."));
            return;
        }

        dialog.accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    stock.productTypeId     = productTypeBox->currentData().toString();
    stock.productLabel      = productTypeBox->currentText();
    stock.quantityAvailable = quantityField->text();
    stock.employeeId        = employeeIdField->text();
    return true;
}
