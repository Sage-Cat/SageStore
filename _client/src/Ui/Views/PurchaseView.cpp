#include "Ui/Views/PurchaseView.hpp"

#include <QAbstractItemView>
#include <QDateEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QDate>
#include <QSpinBox>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <algorithm>

#include "Ui/ViewModels/PurchaseViewModel.hpp"

namespace {
QString canonicalStatus(const QString &status)
{
    const QString normalized = status.trimmed();
    if (normalized.compare(QStringLiteral("Draft"), Qt::CaseInsensitive) == 0 ||
        normalized == QStringLiteral("Чернетка")) {
        return QStringLiteral("Draft");
    }
    if (normalized.compare(QStringLiteral("Ordered"), Qt::CaseInsensitive) == 0 ||
        normalized.compare(QStringLiteral("Pending"), Qt::CaseInsensitive) == 0 ||
        normalized == QStringLiteral("Замовлено")) {
        return QStringLiteral("Ordered");
    }
    if (normalized.compare(QStringLiteral("Received"), Qt::CaseInsensitive) == 0 ||
        normalized == QStringLiteral("Отримано")) {
        return QStringLiteral("Received");
    }
    return normalized;
}

QString displayStatus(const std::string &status)
{
    const QString canonical = canonicalStatus(QString::fromStdString(status));
    if (canonical == QStringLiteral("Draft")) {
        return QObject::tr("Draft");
    }
    if (canonical == QStringLiteral("Ordered")) {
        return QObject::tr("Ordered");
    }
    if (canonical == QStringLiteral("Received")) {
        return QObject::tr("Received");
    }
    return canonical;
}
} // namespace

PurchaseView::PurchaseView(PurchaseViewModel &viewModel, QWidget *parent)
    : QWidget(parent), m_viewModel(viewModel)
{
    setupUi();

    connect(this, &PurchaseView::errorOccurred, &m_viewModel, &PurchaseViewModel::errorOccurred);
    connect(&m_viewModel, &PurchaseViewModel::ordersChanged, this, &PurchaseView::onOrdersChanged);
    connect(&m_viewModel, &PurchaseViewModel::orderRecordsChanged, this,
            &PurchaseView::onOrderRecordsChanged);
    connect(&m_viewModel, &PurchaseViewModel::referenceDataChanged, this,
            &PurchaseView::onReferenceDataChanged);
    connect(&m_viewModel, &PurchaseViewModel::purchaseReceiptPosted, this,
            &PurchaseView::onPurchaseReceiptPosted);

    connect(m_addOrderButton, &QPushButton::clicked, this, &PurchaseView::onAddOrder);
    connect(m_editOrderButton, &QPushButton::clicked, this, &PurchaseView::onEditOrder);
    connect(m_deleteOrderButton, &QPushButton::clicked, this, &PurchaseView::onDeleteOrder);
    connect(m_addRecordButton, &QPushButton::clicked, this, &PurchaseView::onAddRecord);
    connect(m_editRecordButton, &QPushButton::clicked, this, &PurchaseView::onEditRecord);
    connect(m_deleteRecordButton, &QPushButton::clicked, this, &PurchaseView::onDeleteRecord);
    connect(m_receiveButton, &QPushButton::clicked, this, &PurchaseView::onReceiveOrder);

    connect(m_ordersFilterField, &QLineEdit::textChanged, this, &PurchaseView::onOrdersFilterChanged);
    connect(m_receiptsFilterField, &QLineEdit::textChanged, this,
            &PurchaseView::onReceiptsFilterChanged);
    connect(m_ordersTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &PurchaseView::onOrdersSelectionChanged);
    connect(m_receiptOrdersTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &PurchaseView::onReceiptOrdersSelectionChanged);
}

void PurchaseView::showSection(Section section)
{
    m_tabs->setCurrentIndex(section == Section::Orders ? 0 : 1);
}

void PurchaseView::setupUi()
{
    auto *rootLayout = new QVBoxLayout(this);
    auto *headerCard = new QFrame(this);
    auto *headerLayout = new QVBoxLayout(headerCard);
    auto *titleLabel = new QLabel(tr("Purchasing"), headerCard);
    auto *summaryLabel = new QLabel(
        tr("Manage purchase orders, line items, and receipt posting into stock. Supplier and "
           "product master data are reused directly from the shared ERP directories."),
        headerCard);
    summaryLabel->setWordWrap(true);
    titleLabel->setObjectName("purchaseTitleLabel");

    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(summaryLabel);

    m_tabs = new QTabWidget(this);
    m_tabs->setObjectName("purchaseTabs");

    auto *ordersPage = new QWidget(this);
    auto *ordersLayout = new QVBoxLayout(ordersPage);
    auto *ordersToolbar = new QHBoxLayout;

    m_ordersFilterField = new QLineEdit(ordersPage);
    m_ordersFilterField->setObjectName("purchaseOrdersFilterField");
    m_ordersFilterField->setPlaceholderText(
        tr("Search by date, user, supplier, or status"));
    m_ordersFilterField->setClearButtonEnabled(true);
    m_addOrderButton = new QPushButton(tr("Add Order"), ordersPage);
    m_addOrderButton->setObjectName("purchaseAddOrderButton");
    m_editOrderButton = new QPushButton(tr("Edit Order"), ordersPage);
    m_editOrderButton->setObjectName("purchaseEditOrderButton");
    m_deleteOrderButton = new QPushButton(tr("Delete Order"), ordersPage);
    m_deleteOrderButton->setObjectName("purchaseDeleteOrderButton");
    m_deleteOrderButton->setProperty("destructive", true);
    ordersToolbar->addWidget(new QLabel(tr("Filter"), ordersPage));
    ordersToolbar->addWidget(m_ordersFilterField);
    ordersToolbar->addWidget(m_addOrderButton);
    ordersToolbar->addWidget(m_editOrderButton);
    ordersToolbar->addWidget(m_deleteOrderButton);

    m_ordersTable = new QTableWidget(ordersPage);
    m_ordersTable->setObjectName("purchaseOrdersTable");
    m_ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ordersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ordersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ordersTable->setAlternatingRowColors(true);
    m_ordersTable->setSortingEnabled(true);
    m_ordersTable->verticalHeader()->setVisible(false);
    m_ordersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto *recordsToolbar = new QHBoxLayout;
    m_addRecordButton = new QPushButton(tr("Add Line"), ordersPage);
    m_addRecordButton->setObjectName("purchaseAddRecordButton");
    m_editRecordButton = new QPushButton(tr("Edit Line"), ordersPage);
    m_editRecordButton->setObjectName("purchaseEditRecordButton");
    m_deleteRecordButton = new QPushButton(tr("Delete Line"), ordersPage);
    m_deleteRecordButton->setObjectName("purchaseDeleteRecordButton");
    m_deleteRecordButton->setProperty("destructive", true);
    recordsToolbar->addWidget(new QLabel(tr("Order lines"), ordersPage));
    recordsToolbar->addStretch();
    recordsToolbar->addWidget(m_addRecordButton);
    recordsToolbar->addWidget(m_editRecordButton);
    recordsToolbar->addWidget(m_deleteRecordButton);

    m_orderRecordsTable = new QTableWidget(ordersPage);
    m_orderRecordsTable->setObjectName("purchaseOrderRecordsTable");
    m_orderRecordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_orderRecordsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_orderRecordsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_orderRecordsTable->setAlternatingRowColors(true);
    m_orderRecordsTable->setSortingEnabled(true);
    m_orderRecordsTable->verticalHeader()->setVisible(false);
    m_orderRecordsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_ordersStatusLabel = new QLabel(tr("Status: ready"), ordersPage);
    m_ordersStatusLabel->setObjectName("purchaseOrdersStatusLabel");

    ordersLayout->addLayout(ordersToolbar);
    ordersLayout->addWidget(m_ordersTable);
    ordersLayout->addLayout(recordsToolbar);
    ordersLayout->addWidget(m_orderRecordsTable);
    ordersLayout->addWidget(m_ordersStatusLabel);

    auto *receiptsPage = new QWidget(this);
    auto *receiptsLayout = new QVBoxLayout(receiptsPage);
    auto *receiptsToolbar = new QHBoxLayout;

    m_receiptsFilterField = new QLineEdit(receiptsPage);
    m_receiptsFilterField->setObjectName("purchaseReceiptsFilterField");
    m_receiptsFilterField->setPlaceholderText(
        tr("Search open receipts by date, supplier, or status"));
    m_receiptsFilterField->setClearButtonEnabled(true);
    m_receiptEmployeeBox = new QComboBox(receiptsPage);
    m_receiptEmployeeBox->setObjectName("purchaseReceiptEmployeeBox");
    m_receiveButton = new QPushButton(tr("Receive to Stock"), receiptsPage);
    m_receiveButton->setObjectName("purchaseReceiveButton");

    receiptsToolbar->addWidget(new QLabel(tr("Filter"), receiptsPage));
    receiptsToolbar->addWidget(m_receiptsFilterField);
    receiptsToolbar->addWidget(new QLabel(tr("Handled by"), receiptsPage));
    receiptsToolbar->addWidget(m_receiptEmployeeBox);
    receiptsToolbar->addWidget(m_receiveButton);

    m_receiptOrdersTable = new QTableWidget(receiptsPage);
    m_receiptOrdersTable->setObjectName("purchaseReceiptOrdersTable");
    m_receiptOrdersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_receiptOrdersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_receiptOrdersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_receiptOrdersTable->setAlternatingRowColors(true);
    m_receiptOrdersTable->setSortingEnabled(true);
    m_receiptOrdersTable->verticalHeader()->setVisible(false);
    m_receiptOrdersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_receiptRecordsTable = new QTableWidget(receiptsPage);
    m_receiptRecordsTable->setObjectName("purchaseReceiptRecordsTable");
    m_receiptRecordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_receiptRecordsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_receiptRecordsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_receiptRecordsTable->setAlternatingRowColors(true);
    m_receiptRecordsTable->setSortingEnabled(true);
    m_receiptRecordsTable->verticalHeader()->setVisible(false);
    m_receiptRecordsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_receiptsStatusLabel = new QLabel(tr("Status: ready"), receiptsPage);
    m_receiptsStatusLabel->setObjectName("purchaseReceiptsStatusLabel");

    receiptsLayout->addLayout(receiptsToolbar);
    receiptsLayout->addWidget(m_receiptOrdersTable);
    receiptsLayout->addWidget(m_receiptRecordsTable);
    receiptsLayout->addWidget(m_receiptsStatusLabel);

    m_tabs->addTab(ordersPage, tr("Orders"));
    m_tabs->addTab(receiptsPage, tr("Goods Receipts"));

    rootLayout->addWidget(headerCard);
    rootLayout->addWidget(m_tabs);

    setObjectName("purchaseView");
    setStyleSheet(
        "#purchaseView { background-color: #fffaf2; }"
        "QFrame { background-color: #f6efe4; border: 1px solid #d8cdbd; border-radius: 12px; }"
        "#purchaseTitleLabel { color: #233130; font-size: 24px; font-weight: 700; }"
        "QLineEdit, QComboBox, QDateEdit, QSpinBox, QDoubleSpinBox { background-color: white; border: 1px solid #cbd5e1; border-radius: 6px; padding: 6px; }"
        "QPushButton { background-color: #1d4ed8; color: white; border: 0; border-radius: 6px; padding: 6px 12px; }"
        "QPushButton:hover { background-color: #1e40af; }"
        "QPushButton[destructive='true'] { background-color: #b91c1c; }"
        "QPushButton[destructive='true']:hover { background-color: #991b1b; }"
        "QTableWidget { background-color: #fbfbfd; alternate-background-color: #f1f4f8; border: 1px solid #d7dde7; border-radius: 8px; }"
        "QHeaderView::section { background-color: #e8eef6; color: #0f172a; padding: 6px; border: 0; border-bottom: 1px solid #d7dde7; font-weight: 600; }");

    updateOrdersActions();
    updateReceiptActions();
}

void PurchaseView::onOrdersChanged()
{
    m_allOrders = m_viewModel.orders();
    applyOrdersFilter();
    applyReceiptsFilter();
    updateStatus();
}

void PurchaseView::onOrderRecordsChanged()
{
    m_currentRecords = m_viewModel.orderRecords();
    if (m_recordsContext == RecordsContext::Orders) {
        fillRecordsTable(m_orderRecordsTable, m_currentRecords);
    } else if (m_recordsContext == RecordsContext::Receipts) {
        fillRecordsTable(m_receiptRecordsTable, m_currentRecords);
    }
    updateStatus();
}

void PurchaseView::onReferenceDataChanged()
{
    applyOrdersFilter();
    applyReceiptsFilter();
    refreshEmployeeOptions();
    if (m_recordsContext == RecordsContext::Orders) {
        fillRecordsTable(m_orderRecordsTable, m_currentRecords);
    } else if (m_recordsContext == RecordsContext::Receipts) {
        fillRecordsTable(m_receiptRecordsTable, m_currentRecords);
    }
}

void PurchaseView::onPurchaseReceiptPosted()
{
    m_receiptsStatusLabel->setText(tr("Status: goods receipt posted to stock"));
    m_viewModel.fetchOrders();
}

void PurchaseView::onAddOrder()
{
    Common::Entities::PurchaseOrder order;
    if (showOrderDialog(order, tr("Create purchase order"))) {
        m_viewModel.createOrder(order);
    }
}

void PurchaseView::onEditOrder()
{
    const auto selected = m_ordersTable->selectedItems();
    if (selected.isEmpty()) {
        return;
    }
    const int row = selected.first()->row();
    Common::Entities::PurchaseOrder order{
        .id = m_ordersTable->item(row, 0)->text().toStdString(),
        .date = m_ordersTable->item(row, 1)->text().toStdString(),
        .userId = m_ordersTable->item(row, 2)->text().toStdString(),
        .supplierId = m_ordersTable->item(row, 4)->text().toStdString(),
        .status = m_ordersTable->item(row, 6)->data(Qt::UserRole).toString().toStdString()};
    if (showOrderDialog(order, tr("Edit purchase order"))) {
        m_viewModel.editOrder(order);
    }
}

void PurchaseView::onDeleteOrder()
{
    const QString orderId = selectedOrderId(m_ordersTable);
    if (!orderId.isEmpty()) {
        m_viewModel.deleteOrder(orderId);
    }
}

void PurchaseView::onAddRecord()
{
    const QString orderId = selectedOrderId(m_ordersTable);
    if (orderId.isEmpty()) {
        emit errorOccurred(tr("Select a purchase order before adding line items."));
        return;
    }

    Common::Entities::PurchaseOrderRecord record;
    record.orderId = orderId.toStdString();
    if (showOrderRecordDialog(record, tr("Create purchase order line"))) {
        m_viewModel.createOrderRecord(record);
    }
}

void PurchaseView::onEditRecord()
{
    const auto selected = m_orderRecordsTable->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    const int row = selected.first()->row();
    Common::Entities::PurchaseOrderRecord record{
        .id = m_orderRecordsTable->item(row, 0)->text().toStdString(),
        .orderId = m_orderRecordsTable->item(row, 1)->text().toStdString(),
        .productTypeId = m_orderRecordsTable->item(row, 2)->text().toStdString(),
        .quantity = m_orderRecordsTable->item(row, 4)->text().toStdString(),
        .price = m_orderRecordsTable->item(row, 5)->text().toStdString()};
    if (showOrderRecordDialog(record, tr("Edit purchase order line"))) {
        m_viewModel.editOrderRecord(record);
    }
}

void PurchaseView::onDeleteRecord()
{
    const auto selected = m_orderRecordsTable->selectedItems();
    if (!selected.isEmpty()) {
        m_viewModel.deleteOrderRecord(m_orderRecordsTable->item(selected.first()->row(), 0)->text());
    }
}

void PurchaseView::onReceiveOrder()
{
    const QString orderId = selectedOrderId(m_receiptOrdersTable);
    if (orderId.isEmpty()) {
        emit errorOccurred(tr("Select an open purchase order to receive."));
        return;
    }

    if (m_receiptEmployeeBox->currentData().toString().isEmpty()) {
        emit errorOccurred(tr("Select the employee responsible for the receipt."));
        return;
    }

    m_viewModel.postReceipt(orderId, m_receiptEmployeeBox->currentData().toString());
}

void PurchaseView::onOrdersFilterChanged(const QString &text)
{
    Q_UNUSED(text);
    applyOrdersFilter();
}

void PurchaseView::onReceiptsFilterChanged(const QString &text)
{
    Q_UNUSED(text);
    applyReceiptsFilter();
}

void PurchaseView::onOrdersSelectionChanged()
{
    updateOrdersActions();
    const QString orderId = selectedOrderId(m_ordersTable);
    if (!orderId.isEmpty()) {
        m_recordsContext = RecordsContext::Orders;
        m_viewModel.fetchOrderRecords(orderId);
    } else {
        fillRecordsTable(m_orderRecordsTable, {});
    }
}

void PurchaseView::onReceiptOrdersSelectionChanged()
{
    updateReceiptActions();
    const QString orderId = selectedOrderId(m_receiptOrdersTable);
    if (!orderId.isEmpty()) {
        m_recordsContext = RecordsContext::Receipts;
        m_viewModel.fetchOrderRecords(orderId);
    } else {
        fillRecordsTable(m_receiptRecordsTable, {});
    }
}

void PurchaseView::applyOrdersFilter()
{
    QVector<Common::Entities::PurchaseOrder> filtered;
    const QString filter = m_ordersFilterField->text().trimmed();
    for (const auto &order : m_allOrders) {
        const QStringList haystack = {QString::fromStdString(order.date),
                                      m_viewModel.userLabel(QString::fromStdString(order.userId)),
                                      m_viewModel.supplierLabel(
                                          QString::fromStdString(order.supplierId)),
                                      displayStatus(order.status),
                                      canonicalStatus(QString::fromStdString(order.status))};
        if (filter.isEmpty() || std::any_of(haystack.begin(), haystack.end(), [&](const QString &value) {
                return value.contains(filter, Qt::CaseInsensitive);
            })) {
            filtered.push_back(order);
        }
    }
    fillOrdersTable(m_ordersTable, filtered);
    updateOrdersActions();
    updateStatus();
}

void PurchaseView::applyReceiptsFilter()
{
    QVector<Common::Entities::PurchaseOrder> filtered;
    const QString filter = m_receiptsFilterField->text().trimmed();
    for (const auto &order : m_allOrders) {
        if (canonicalStatus(QString::fromStdString(order.status)) == QStringLiteral("Received")) {
            continue;
        }

        const QStringList haystack = {QString::fromStdString(order.date),
                                      m_viewModel.supplierLabel(
                                          QString::fromStdString(order.supplierId)),
                                      displayStatus(order.status),
                                      canonicalStatus(QString::fromStdString(order.status))};
        if (filter.isEmpty() || std::any_of(haystack.begin(), haystack.end(), [&](const QString &value) {
                return value.contains(filter, Qt::CaseInsensitive);
            })) {
            filtered.push_back(order);
        }
    }
    fillOrdersTable(m_receiptOrdersTable, filtered);
    updateReceiptActions();
    updateStatus();
}

void PurchaseView::fillOrdersTable(
    QTableWidget *table, const QVector<Common::Entities::PurchaseOrder> &orders) const
{
    table->setSortingEnabled(false);
    table->clearContents();
    table->setRowCount(orders.size());
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels(
        {tr("ID"), tr("Date"), tr("User ID"), tr("User"), tr("Supplier ID"), tr("Supplier"),
         tr("Status")});

    for (int row = 0; row < orders.size(); ++row) {
        const auto &order = orders[row];
        const QString userId = QString::fromStdString(order.userId);
        const QString supplierId = QString::fromStdString(order.supplierId);
        const QString orderStatus = canonicalStatus(QString::fromStdString(order.status));
        table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(order.id)));
        table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(order.date)));
        table->setItem(row, 2, new QTableWidgetItem(userId));
        table->setItem(row, 3, new QTableWidgetItem(m_viewModel.userLabel(userId)));
        table->setItem(row, 4, new QTableWidgetItem(supplierId));
        table->setItem(row, 5, new QTableWidgetItem(m_viewModel.supplierLabel(supplierId)));
        auto *statusItem = new QTableWidgetItem(displayStatus(order.status));
        statusItem->setData(Qt::UserRole, orderStatus);
        table->setItem(row, 6, statusItem);
    }

    table->setColumnHidden(0, true);
    table->setColumnHidden(2, true);
    table->setColumnHidden(4, true);
    table->setSortingEnabled(true);
}

void PurchaseView::fillRecordsTable(
    QTableWidget *table, const QVector<Common::Entities::PurchaseOrderRecord> &records) const
{
    table->setSortingEnabled(false);
    table->clearContents();
    table->setRowCount(records.size());
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels(
        {tr("ID"), tr("Order ID"), tr("Product Type ID"), tr("Product"), tr("Quantity"),
         tr("Price")});

    for (int row = 0; row < records.size(); ++row) {
        const auto &record = records[row];
        const QString productTypeId = QString::fromStdString(record.productTypeId);
        table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(record.id)));
        table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(record.orderId)));
        table->setItem(row, 2, new QTableWidgetItem(productTypeId));
        table->setItem(row, 3, new QTableWidgetItem(m_viewModel.productTypeLabel(productTypeId)));
        table->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(record.quantity)));
        table->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(record.price)));
    }

    table->setColumnHidden(0, true);
    table->setColumnHidden(1, true);
    table->setColumnHidden(2, true);
    table->setSortingEnabled(true);
}

void PurchaseView::updateOrdersActions()
{
    const bool hasOrderSelection = !m_ordersTable->selectedItems().isEmpty();
    const bool hasRecordSelection = !m_orderRecordsTable->selectedItems().isEmpty();
    m_editOrderButton->setEnabled(hasOrderSelection);
    m_deleteOrderButton->setEnabled(hasOrderSelection);
    m_addRecordButton->setEnabled(hasOrderSelection);
    m_editRecordButton->setEnabled(hasRecordSelection);
    m_deleteRecordButton->setEnabled(hasRecordSelection);
}

void PurchaseView::updateReceiptActions()
{
    const bool hasSelection = !m_receiptOrdersTable->selectedItems().isEmpty();
    m_receiveButton->setEnabled(hasSelection && !m_receiptEmployeeBox->currentData().toString().isEmpty());
}

void PurchaseView::updateStatus()
{
    m_ordersStatusLabel->setText(
        tr("Status: %1 purchase orders, %2 current lines")
            .arg(QString::number(m_allOrders.size()), QString::number(m_currentRecords.size())));
    m_receiptsStatusLabel->setText(
        tr("Status: %1 purchase orders ready for receipt")
            .arg(QString::number(m_receiptOrdersTable->rowCount())));
}

void PurchaseView::refreshEmployeeOptions()
{
    const QString currentEmployeeId = m_receiptEmployeeBox->currentData().toString();
    m_receiptEmployeeBox->clear();
    for (const auto &employee : m_viewModel.employees()) {
        m_receiptEmployeeBox->addItem(QString::fromStdString(employee.name),
                                      QString::fromStdString(employee.id));
    }

    const int index = m_receiptEmployeeBox->findData(currentEmployeeId);
    if (index >= 0) {
        m_receiptEmployeeBox->setCurrentIndex(index);
    }
    updateReceiptActions();
}

QString PurchaseView::selectedOrderId(QTableWidget *table) const
{
    const auto selected = table->selectedItems();
    if (selected.isEmpty()) {
        return {};
    }
    return table->item(selected.first()->row(), 0)->text();
}

bool PurchaseView::showOrderDialog(Common::Entities::PurchaseOrder &order, const QString &title)
{
    if (m_viewModel.users().isEmpty() || m_viewModel.suppliers().isEmpty()) {
        emit errorOccurred(tr("Users and suppliers must exist before creating purchase orders."));
        return false;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setMinimumWidth(460);

    auto *layout = new QFormLayout(&dialog);
    auto *dateEdit = new QDateEdit(&dialog);
    auto *userBox = new QComboBox(&dialog);
    auto *supplierBox = new QComboBox(&dialog);
    auto *statusBox = new QComboBox(&dialog);

    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("yyyy-MM-dd");
    dateEdit->setDate(order.date.empty() ? QDate::currentDate()
                                         : QDate::fromString(QString::fromStdString(order.date),
                                                             "yyyy-MM-dd"));

    for (const auto &user : m_viewModel.users()) {
        userBox->addItem(QString::fromStdString(user.username), QString::fromStdString(user.id));
    }
    for (const auto &supplier : m_viewModel.suppliers()) {
        supplierBox->addItem(QString::fromStdString(supplier.name),
                             QString::fromStdString(supplier.id));
    }
    statusBox->addItem(tr("Draft"), QStringLiteral("Draft"));
    statusBox->addItem(tr("Ordered"), QStringLiteral("Ordered"));
    statusBox->addItem(tr("Received"), QStringLiteral("Received"));

    if (!order.userId.empty()) {
        userBox->setCurrentIndex(userBox->findData(QString::fromStdString(order.userId)));
    }
    if (!order.supplierId.empty()) {
        supplierBox->setCurrentIndex(
            supplierBox->findData(QString::fromStdString(order.supplierId)));
    }
    if (!order.status.empty()) {
        statusBox->setCurrentIndex(
            statusBox->findData(canonicalStatus(QString::fromStdString(order.status))));
    } else {
        statusBox->setCurrentIndex(statusBox->findData(QStringLiteral("Draft")));
    }

    layout->addRow(tr("Date"), dateEdit);
    layout->addRow(tr("User"), userBox);
    layout->addRow(tr("Supplier"), supplierBox);
    layout->addRow(tr("Status"), statusBox);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    order.date = dateEdit->date().toString("yyyy-MM-dd").toStdString();
    order.userId = userBox->currentData().toString().toStdString();
    order.supplierId = supplierBox->currentData().toString().toStdString();
    order.status = statusBox->currentData().toString().toStdString();
    return true;
}

bool PurchaseView::showOrderRecordDialog(Common::Entities::PurchaseOrderRecord &record,
                                         const QString &title)
{
    if (m_viewModel.productTypes().isEmpty()) {
        emit errorOccurred(tr("Product types must exist before creating purchase lines."));
        return false;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setMinimumWidth(420);

    auto *layout = new QFormLayout(&dialog);
    auto *productTypeBox = new QComboBox(&dialog);
    auto *quantityBox = new QSpinBox(&dialog);
    auto *priceBox = new QDoubleSpinBox(&dialog);

    quantityBox->setRange(1, 1'000'000);
    priceBox->setRange(0.0, 1'000'000.0);
    priceBox->setDecimals(2);

    for (const auto &productType : m_viewModel.productTypes()) {
        productTypeBox->addItem(
            QString::fromStdString(productType.code) + " | " + QString::fromStdString(productType.name),
            QString::fromStdString(productType.id));
    }

    if (!record.productTypeId.empty()) {
        productTypeBox->setCurrentIndex(
            productTypeBox->findData(QString::fromStdString(record.productTypeId)));
    }
    quantityBox->setValue(record.quantity.empty() ? 1 : QString::fromStdString(record.quantity).toInt());
    priceBox->setValue(record.price.empty() ? 0.0 : QString::fromStdString(record.price).toDouble());

    layout->addRow(tr("Product type"), productTypeBox);
    layout->addRow(tr("Quantity"), quantityBox);
    layout->addRow(tr("Price"), priceBox);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    record.productTypeId = productTypeBox->currentData().toString().toStdString();
    record.quantity = QString::number(quantityBox->value()).toStdString();
    record.price = QString::number(priceBox->value(), 'f', 2).toStdString();
    return true;
}
