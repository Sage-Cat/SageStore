#include "Ui/Views/SalesView.hpp"

#include <QAbstractItemView>
#include <QDateEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QTextStream>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSpinBox>
#include <algorithm>

#include "Ui/ViewModels/SalesViewModel.hpp"

namespace {
QString canonicalSalesStatus(const QString &status)
{
    const QString normalized = status.trimmed();
    if (normalized.compare(QStringLiteral("Draft"), Qt::CaseInsensitive) == 0 ||
        normalized == QStringLiteral("Чернетка")) {
        return QStringLiteral("Draft");
    }
    if (normalized.compare(QStringLiteral("Confirmed"), Qt::CaseInsensitive) == 0 ||
        normalized == QStringLiteral("Підтверджено")) {
        return QStringLiteral("Confirmed");
    }
    if (normalized.compare(QStringLiteral("Invoiced"), Qt::CaseInsensitive) == 0 ||
        normalized == QStringLiteral("Виставлено рахунок")) {
        return QStringLiteral("Invoiced");
    }
    return normalized;
}

QString displaySalesStatus(const std::string &status)
{
    const QString canonical = canonicalSalesStatus(QString::fromStdString(status));
    if (canonical == QStringLiteral("Draft")) {
        return QObject::tr("Draft");
    }
    if (canonical == QStringLiteral("Confirmed")) {
        return QObject::tr("Confirmed");
    }
    if (canonical == QStringLiteral("Invoiced")) {
        return QObject::tr("Invoiced");
    }
    return canonical;
}
} // namespace

SalesView::SalesView(SalesViewModel &viewModel, QWidget *parent)
    : QWidget(parent), m_viewModel(viewModel)
{
    setupUi();

    connect(this, &SalesView::errorOccurred, &m_viewModel, &SalesViewModel::errorOccurred);
    connect(&m_viewModel, &SalesViewModel::ordersChanged, this, &SalesView::onOrdersChanged);
    connect(&m_viewModel, &SalesViewModel::orderRecordsChanged, this,
            &SalesView::onOrderRecordsChanged);
    connect(&m_viewModel, &SalesViewModel::referenceDataChanged, this,
            &SalesView::onReferenceDataChanged);

    connect(m_addOrderButton, &QPushButton::clicked, this, &SalesView::onAddOrder);
    connect(m_editOrderButton, &QPushButton::clicked, this, &SalesView::onEditOrder);
    connect(m_deleteOrderButton, &QPushButton::clicked, this, &SalesView::onDeleteOrder);
    connect(m_addRecordButton, &QPushButton::clicked, this, &SalesView::onAddRecord);
    connect(m_editRecordButton, &QPushButton::clicked, this, &SalesView::onEditRecord);
    connect(m_deleteRecordButton, &QPushButton::clicked, this, &SalesView::onDeleteRecord);
    connect(m_exportInvoiceButton, &QPushButton::clicked, this, &SalesView::onExportInvoice);

    connect(m_ordersFilterField, &QLineEdit::textChanged, this, &SalesView::onOrdersFilterChanged);
    connect(m_invoicesFilterField, &QLineEdit::textChanged, this,
            &SalesView::onInvoicesFilterChanged);
    connect(m_ordersTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &SalesView::onOrdersSelectionChanged);
    connect(m_invoiceOrdersTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &SalesView::onInvoiceSelectionChanged);
}

void SalesView::showSection(Section section)
{
    m_tabs->setCurrentIndex(section == Section::Orders ? 0 : 1);
}

void SalesView::setupUi()
{
    auto *rootLayout = new QVBoxLayout(this);
    auto *headerCard = new QFrame(this);
    auto *headerLayout = new QVBoxLayout(headerCard);
    auto *titleLabel = new QLabel(tr("Sales"), headerCard);
    auto *summaryLabel = new QLabel(
        tr("Manage sales orders, order lines, and simple invoice export. Customer and employee "
           "records are reused from the shared management workspace."),
        headerCard);
    summaryLabel->setWordWrap(true);
    titleLabel->setObjectName("salesTitleLabel");
    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(summaryLabel);

    m_tabs = new QTabWidget(this);

    auto *ordersPage = new QWidget(this);
    auto *ordersLayout = new QVBoxLayout(ordersPage);
    auto *ordersToolbar = new QHBoxLayout;
    m_ordersFilterField = new QLineEdit(ordersPage);
    m_ordersFilterField->setObjectName("salesOrdersFilterField");
    m_ordersFilterField->setPlaceholderText(tr("Search by date, customer, employee, or status"));
    m_ordersFilterField->setClearButtonEnabled(true);
    m_addOrderButton = new QPushButton(tr("Add Order"), ordersPage);
    m_editOrderButton = new QPushButton(tr("Edit Order"), ordersPage);
    m_deleteOrderButton = new QPushButton(tr("Delete Order"), ordersPage);
    m_deleteOrderButton->setProperty("destructive", true);
    ordersToolbar->addWidget(new QLabel(tr("Filter"), ordersPage));
    ordersToolbar->addWidget(m_ordersFilterField);
    ordersToolbar->addWidget(m_addOrderButton);
    ordersToolbar->addWidget(m_editOrderButton);
    ordersToolbar->addWidget(m_deleteOrderButton);

    m_ordersTable = new QTableWidget(ordersPage);
    m_ordersTable->setObjectName("salesOrdersTable");
    m_ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ordersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ordersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ordersTable->setAlternatingRowColors(true);
    m_ordersTable->setSortingEnabled(true);
    m_ordersTable->verticalHeader()->setVisible(false);
    m_ordersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto *recordsToolbar = new QHBoxLayout;
    m_addRecordButton = new QPushButton(tr("Add Line"), ordersPage);
    m_editRecordButton = new QPushButton(tr("Edit Line"), ordersPage);
    m_deleteRecordButton = new QPushButton(tr("Delete Line"), ordersPage);
    m_deleteRecordButton->setProperty("destructive", true);
    recordsToolbar->addWidget(new QLabel(tr("Order lines"), ordersPage));
    recordsToolbar->addStretch();
    recordsToolbar->addWidget(m_addRecordButton);
    recordsToolbar->addWidget(m_editRecordButton);
    recordsToolbar->addWidget(m_deleteRecordButton);

    m_orderRecordsTable = new QTableWidget(ordersPage);
    m_orderRecordsTable->setObjectName("salesOrderRecordsTable");
    m_orderRecordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_orderRecordsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_orderRecordsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_orderRecordsTable->setAlternatingRowColors(true);
    m_orderRecordsTable->setSortingEnabled(true);
    m_orderRecordsTable->verticalHeader()->setVisible(false);
    m_orderRecordsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_ordersStatusLabel = new QLabel(tr("Status: ready"), ordersPage);

    ordersLayout->addLayout(ordersToolbar);
    ordersLayout->addWidget(m_ordersTable);
    ordersLayout->addLayout(recordsToolbar);
    ordersLayout->addWidget(m_orderRecordsTable);
    ordersLayout->addWidget(m_ordersStatusLabel);

    auto *invoicesPage = new QWidget(this);
    auto *invoicesLayout = new QVBoxLayout(invoicesPage);
    auto *invoicesToolbar = new QHBoxLayout;
    m_invoicesFilterField = new QLineEdit(invoicesPage);
    m_invoicesFilterField->setObjectName("salesInvoicesFilterField");
    m_invoicesFilterField->setPlaceholderText(tr("Search invoices by customer, date, or status"));
    m_invoicesFilterField->setClearButtonEnabled(true);
    m_exportInvoiceButton = new QPushButton(tr("Export Invoice"), invoicesPage);
    m_exportInvoiceButton->setObjectName("salesExportInvoiceButton");
    invoicesToolbar->addWidget(new QLabel(tr("Filter"), invoicesPage));
    invoicesToolbar->addWidget(m_invoicesFilterField);
    invoicesToolbar->addWidget(m_exportInvoiceButton);

    m_invoiceOrdersTable = new QTableWidget(invoicesPage);
    m_invoiceOrdersTable->setObjectName("salesInvoiceOrdersTable");
    m_invoiceOrdersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_invoiceOrdersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_invoiceOrdersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_invoiceOrdersTable->setAlternatingRowColors(true);
    m_invoiceOrdersTable->setSortingEnabled(true);
    m_invoiceOrdersTable->verticalHeader()->setVisible(false);
    m_invoiceOrdersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_invoiceRecordsTable = new QTableWidget(invoicesPage);
    m_invoiceRecordsTable->setObjectName("salesInvoiceRecordsTable");
    m_invoiceRecordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_invoiceRecordsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_invoiceRecordsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_invoiceRecordsTable->setAlternatingRowColors(true);
    m_invoiceRecordsTable->setSortingEnabled(true);
    m_invoiceRecordsTable->verticalHeader()->setVisible(false);
    m_invoiceRecordsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_invoicePreview = new QPlainTextEdit(invoicesPage);
    m_invoicePreview->setObjectName("salesInvoicePreview");
    m_invoicePreview->setReadOnly(true);
    m_invoicePreview->setPlaceholderText(tr("Select an invoice to preview export output."));

    m_invoicesStatusLabel = new QLabel(tr("Status: ready"), invoicesPage);

    invoicesLayout->addLayout(invoicesToolbar);
    invoicesLayout->addWidget(m_invoiceOrdersTable);
    invoicesLayout->addWidget(m_invoiceRecordsTable);
    invoicesLayout->addWidget(m_invoicePreview);
    invoicesLayout->addWidget(m_invoicesStatusLabel);

    m_tabs->addTab(ordersPage, tr("Orders"));
    m_tabs->addTab(invoicesPage, tr("Invoicing"));

    rootLayout->addWidget(headerCard);
    rootLayout->addWidget(m_tabs);

    setObjectName("salesView");
    setStyleSheet(
        "#salesView { background-color: #fffaf2; }"
        "QFrame { background-color: #f6efe4; border: 1px solid #d8cdbd; border-radius: 12px; }"
        "#salesTitleLabel { color: #233130; font-size: 24px; font-weight: 700; }"
        "QLineEdit, QComboBox, QDateEdit, QSpinBox, QDoubleSpinBox, QPlainTextEdit { background-color: white; border: 1px solid #cbd5e1; border-radius: 6px; padding: 6px; }"
        "QPushButton { background-color: #1d4ed8; color: white; border: 0; border-radius: 6px; padding: 6px 12px; }"
        "QPushButton:hover { background-color: #1e40af; }"
        "QPushButton[destructive='true'] { background-color: #b91c1c; }"
        "QPushButton[destructive='true']:hover { background-color: #991b1b; }"
        "QTableWidget { background-color: #fbfbfd; alternate-background-color: #f1f4f8; border: 1px solid #d7dde7; border-radius: 8px; }"
        "QHeaderView::section { background-color: #e8eef6; color: #0f172a; padding: 6px; border: 0; border-bottom: 1px solid #d7dde7; font-weight: 600; }");

    updateActions();
}

void SalesView::onOrdersChanged()
{
    m_allOrders = m_viewModel.orders();
    applyOrdersFilter();
    applyInvoicesFilter();
}

void SalesView::onOrderRecordsChanged()
{
    m_currentRecords = m_viewModel.orderRecords();
    if (m_recordsContext == RecordsContext::Orders) {
        fillRecordsTable(m_orderRecordsTable, m_currentRecords);
    } else if (m_recordsContext == RecordsContext::Invoices) {
        fillRecordsTable(m_invoiceRecordsTable, m_currentRecords);
        updateInvoicePreview();
    }
    updateActions();
}

void SalesView::onReferenceDataChanged()
{
    applyOrdersFilter();
    applyInvoicesFilter();
    if (m_recordsContext == RecordsContext::Orders) {
        fillRecordsTable(m_orderRecordsTable, m_currentRecords);
    } else if (m_recordsContext == RecordsContext::Invoices) {
        fillRecordsTable(m_invoiceRecordsTable, m_currentRecords);
        updateInvoicePreview();
    }
}

void SalesView::onAddOrder()
{
    Common::Entities::SaleOrder order;
    if (showOrderDialog(order, tr("Create sales order"))) {
        m_viewModel.createOrder(order);
    }
}

void SalesView::onEditOrder()
{
    const auto selected = m_ordersTable->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    const int row = selected.first()->row();
    Common::Entities::SaleOrder order{
        .id = m_ordersTable->item(row, 0)->text().toStdString(),
        .date = m_ordersTable->item(row, 1)->text().toStdString(),
        .userId = m_ordersTable->item(row, 2)->text().toStdString(),
        .contactId = m_ordersTable->item(row, 4)->text().toStdString(),
        .employeeId = m_ordersTable->item(row, 6)->text().toStdString(),
        .status = m_ordersTable->item(row, 8)->data(Qt::UserRole).toString().toStdString()};
    if (showOrderDialog(order, tr("Edit sales order"))) {
        m_viewModel.editOrder(order);
    }
}

void SalesView::onDeleteOrder()
{
    const QString orderId = selectedOrderId(m_ordersTable);
    if (!orderId.isEmpty()) {
        m_viewModel.deleteOrder(orderId);
    }
}

void SalesView::onAddRecord()
{
    const QString orderId = selectedOrderId(m_ordersTable);
    if (orderId.isEmpty()) {
        emit errorOccurred(tr("Select a sales order before adding line items."));
        return;
    }

    Common::Entities::SalesOrderRecord record;
    record.orderId = orderId.toStdString();
    if (showOrderRecordDialog(record, tr("Create sales line"))) {
        m_viewModel.createOrderRecord(record);
    }
}

void SalesView::onEditRecord()
{
    const auto selected = m_orderRecordsTable->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    const int row = selected.first()->row();
    Common::Entities::SalesOrderRecord record{
        .id = m_orderRecordsTable->item(row, 0)->text().toStdString(),
        .orderId = m_orderRecordsTable->item(row, 1)->text().toStdString(),
        .productTypeId = m_orderRecordsTable->item(row, 2)->text().toStdString(),
        .quantity = m_orderRecordsTable->item(row, 4)->text().toStdString(),
        .price = m_orderRecordsTable->item(row, 5)->text().toStdString()};
    if (showOrderRecordDialog(record, tr("Edit sales line"))) {
        m_viewModel.editOrderRecord(record);
    }
}

void SalesView::onDeleteRecord()
{
    const auto selected = m_orderRecordsTable->selectedItems();
    if (!selected.isEmpty()) {
        m_viewModel.deleteOrderRecord(m_orderRecordsTable->item(selected.first()->row(), 0)->text());
    }
}

void SalesView::onOrdersSelectionChanged()
{
    updateActions();
    const QString orderId = selectedOrderId(m_ordersTable);
    if (!orderId.isEmpty()) {
        m_recordsContext = RecordsContext::Orders;
        m_viewModel.fetchOrderRecords(orderId);
    } else {
        fillRecordsTable(m_orderRecordsTable, {});
    }
}

void SalesView::onInvoiceSelectionChanged()
{
    updateActions();
    const QString orderId = selectedOrderId(m_invoiceOrdersTable);
    if (!orderId.isEmpty()) {
        m_recordsContext = RecordsContext::Invoices;
        m_viewModel.fetchOrderRecords(orderId);
    } else {
        fillRecordsTable(m_invoiceRecordsTable, {});
        m_invoicePreview->clear();
    }
}

void SalesView::onOrdersFilterChanged(const QString &text)
{
    Q_UNUSED(text);
    applyOrdersFilter();
}

void SalesView::onInvoicesFilterChanged(const QString &text)
{
    Q_UNUSED(text);
    applyInvoicesFilter();
}

void SalesView::onExportInvoice()
{
    const QString orderId = selectedOrderId(m_invoiceOrdersTable);
    if (orderId.isEmpty()) {
        emit errorOccurred(tr("Select an invoice before exporting."));
        return;
    }

    const QString defaultFile = QString("invoice-%1.txt").arg(orderId);
    const QString filePath = QFileDialog::getSaveFileName(
        this, tr("Export invoice"), defaultFile, tr("Text files (*.txt);;All files (*)"));
    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred(tr("Unable to write the invoice export file."));
        return;
    }

    QTextStream stream(&file);
    stream << m_invoicePreview->toPlainText();
    m_invoicesStatusLabel->setText(tr("Status: invoice exported"));
}

void SalesView::applyOrdersFilter()
{
    QVector<Common::Entities::SaleOrder> filtered;
    const QString filter = m_ordersFilterField->text().trimmed();
    for (const auto &order : m_allOrders) {
        const QStringList haystack = {QString::fromStdString(order.date),
                                      m_viewModel.contactLabel(QString::fromStdString(order.contactId)),
                                      m_viewModel.employeeLabel(QString::fromStdString(order.employeeId)),
                                      displaySalesStatus(order.status),
                                      canonicalSalesStatus(QString::fromStdString(order.status))};
        if (filter.isEmpty() || std::any_of(haystack.begin(), haystack.end(), [&](const QString &value) {
                return value.contains(filter, Qt::CaseInsensitive);
            })) {
            filtered.push_back(order);
        }
    }
    fillOrdersTable(m_ordersTable, filtered);
    updateActions();
    m_ordersStatusLabel->setText(
        tr("Status: showing %1 sales orders").arg(QString::number(filtered.size())));
}

void SalesView::applyInvoicesFilter()
{
    QVector<Common::Entities::SaleOrder> filtered;
    const QString filter = m_invoicesFilterField->text().trimmed();
    for (const auto &order : m_allOrders) {
        const QStringList haystack = {QString::fromStdString(order.date),
                                      m_viewModel.contactLabel(QString::fromStdString(order.contactId)),
                                      displaySalesStatus(order.status),
                                      canonicalSalesStatus(QString::fromStdString(order.status))};
        if (filter.isEmpty() || std::any_of(haystack.begin(), haystack.end(), [&](const QString &value) {
                return value.contains(filter, Qt::CaseInsensitive);
            })) {
            filtered.push_back(order);
        }
    }
    fillOrdersTable(m_invoiceOrdersTable, filtered);
    updateActions();
    m_invoicesStatusLabel->setText(
        tr("Status: showing %1 invoices").arg(QString::number(filtered.size())));
}

void SalesView::fillOrdersTable(QTableWidget *table,
                                const QVector<Common::Entities::SaleOrder> &orders) const
{
    table->setSortingEnabled(false);
    table->clearContents();
    table->setRowCount(orders.size());
    table->setColumnCount(9);
    table->setHorizontalHeaderLabels(
        {tr("ID"), tr("Date"), tr("User ID"), tr("User"), tr("Customer ID"), tr("Customer"),
         tr("Employee ID"), tr("Employee"), tr("Status")});

    for (int row = 0; row < orders.size(); ++row) {
        const auto &order = orders[row];
        const QString userId = QString::fromStdString(order.userId);
        const QString contactId = QString::fromStdString(order.contactId);
        const QString employeeId = QString::fromStdString(order.employeeId);
        table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(order.id)));
        table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(order.date)));
        table->setItem(row, 2, new QTableWidgetItem(userId));
        table->setItem(row, 3, new QTableWidgetItem(m_viewModel.userLabel(userId)));
        table->setItem(row, 4, new QTableWidgetItem(contactId));
        table->setItem(row, 5, new QTableWidgetItem(m_viewModel.contactLabel(contactId)));
        table->setItem(row, 6, new QTableWidgetItem(employeeId));
        table->setItem(row, 7, new QTableWidgetItem(m_viewModel.employeeLabel(employeeId)));
        auto *statusItem = new QTableWidgetItem(displaySalesStatus(order.status));
        statusItem->setData(Qt::UserRole, canonicalSalesStatus(QString::fromStdString(order.status)));
        table->setItem(row, 8, statusItem);
    }

    table->setColumnHidden(0, true);
    table->setColumnHidden(2, true);
    table->setColumnHidden(4, true);
    table->setColumnHidden(6, true);
    table->setSortingEnabled(true);
}

void SalesView::fillRecordsTable(
    QTableWidget *table, const QVector<Common::Entities::SalesOrderRecord> &records) const
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

void SalesView::updateActions()
{
    const bool hasOrderSelection = !m_ordersTable->selectedItems().isEmpty();
    const bool hasRecordSelection = !m_orderRecordsTable->selectedItems().isEmpty();
    const bool hasInvoiceSelection = !m_invoiceOrdersTable->selectedItems().isEmpty();
    m_editOrderButton->setEnabled(hasOrderSelection);
    m_deleteOrderButton->setEnabled(hasOrderSelection);
    m_addRecordButton->setEnabled(hasOrderSelection);
    m_editRecordButton->setEnabled(hasRecordSelection);
    m_deleteRecordButton->setEnabled(hasRecordSelection);
    m_exportInvoiceButton->setEnabled(hasInvoiceSelection);
}

void SalesView::updateInvoicePreview()
{
    const QString orderId = selectedOrderId(m_invoiceOrdersTable);
    m_invoicePreview->setPlainText(orderId.isEmpty() ? QString() : m_viewModel.buildInvoicePreview(orderId));
}

QString SalesView::selectedOrderId(QTableWidget *table) const
{
    const auto selected = table->selectedItems();
    if (selected.isEmpty()) {
        return {};
    }
    return table->item(selected.first()->row(), 0)->text();
}

bool SalesView::showOrderDialog(Common::Entities::SaleOrder &order, const QString &title)
{
    if (m_viewModel.users().isEmpty() || m_viewModel.contacts().isEmpty() ||
        m_viewModel.employees().isEmpty()) {
        emit errorOccurred(tr("Users, customers, and employees must exist before sales orders."));
        return false;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setMinimumWidth(480);

    auto *layout = new QFormLayout(&dialog);
    auto *dateEdit = new QDateEdit(&dialog);
    auto *userBox = new QComboBox(&dialog);
    auto *contactBox = new QComboBox(&dialog);
    auto *employeeBox = new QComboBox(&dialog);
    auto *statusBox = new QComboBox(&dialog);
    userBox->setObjectName("salesOrderUserField");
    contactBox->setObjectName("salesOrderCustomerField");
    employeeBox->setObjectName("salesOrderEmployeeField");
    statusBox->setObjectName("salesOrderStatusField");

    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("yyyy-MM-dd");
    dateEdit->setDate(order.date.empty() ? QDate::currentDate()
                                         : QDate::fromString(QString::fromStdString(order.date),
                                                             "yyyy-MM-dd"));

    for (const auto &user : m_viewModel.users()) {
        userBox->addItem(QString::fromStdString(user.username), QString::fromStdString(user.id));
    }
    for (const auto &contact : m_viewModel.contacts()) {
        contactBox->addItem(QString::fromStdString(contact.name),
                            QString::fromStdString(contact.id));
    }
    for (const auto &employee : m_viewModel.employees()) {
        employeeBox->addItem(QString::fromStdString(employee.name),
                             QString::fromStdString(employee.id));
    }
    statusBox->addItem(tr("Draft"), QStringLiteral("Draft"));
    statusBox->addItem(tr("Confirmed"), QStringLiteral("Confirmed"));
    statusBox->addItem(tr("Invoiced"), QStringLiteral("Invoiced"));

    if (!order.userId.empty()) {
        userBox->setCurrentIndex(userBox->findData(QString::fromStdString(order.userId)));
    }
    if (!order.contactId.empty()) {
        contactBox->setCurrentIndex(contactBox->findData(QString::fromStdString(order.contactId)));
    }
    if (!order.employeeId.empty()) {
        employeeBox->setCurrentIndex(
            employeeBox->findData(QString::fromStdString(order.employeeId)));
    }
    if (!order.status.empty()) {
        const QString canonical = canonicalSalesStatus(QString::fromStdString(order.status));
        int index = statusBox->findData(canonical);
        if (index < 0) {
            statusBox->addItem(canonical, canonical);
            index = statusBox->findData(canonical);
        }
        statusBox->setCurrentIndex(index);
    } else {
        statusBox->setCurrentIndex(statusBox->findData(QStringLiteral("Draft")));
    }

    layout->addRow(tr("Date"), dateEdit);
    layout->addRow(tr("User"), userBox);
    layout->addRow(tr("Customer"), contactBox);
    layout->addRow(tr("Employee"), employeeBox);
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
    order.contactId = contactBox->currentData().toString().toStdString();
    order.employeeId = employeeBox->currentData().toString().toStdString();
    order.status = statusBox->currentData().toString().toStdString();
    return true;
}

bool SalesView::showOrderRecordDialog(Common::Entities::SalesOrderRecord &record,
                                      const QString &title)
{
    if (m_viewModel.productTypes().isEmpty()) {
        emit errorOccurred(tr("Product types must exist before sales lines."));
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
