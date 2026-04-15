#include "Ui/Views/SalesView.hpp"

#include "Ui/Views/TableInteractionHelpers.hpp"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDate>
#include <QFile>
#include <QFileDialog>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QTextStream>
#include <QVBoxLayout>
#include <algorithm>
#include <limits>

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

bool isValidSalesDate(const QString &text)
{
    return QDate::fromString(text.trimmed(), QStringLiteral("yyyy-MM-dd")).isValid();
}

bool parseSalesQuantity(const QString &text, QString *normalized = nullptr)
{
    bool ok = false;
    const int value = text.trimmed().toInt(&ok);
    if (!ok || value <= 0) {
        return false;
    }

    if (normalized != nullptr) {
        *normalized = QString::number(value);
    }
    return true;
}

bool parseSalesPrice(const QString &text, QString *normalized = nullptr)
{
    bool ok = false;
    const double value = text.trimmed().toDouble(&ok);
    if (!ok || value < 0.0) {
        return false;
    }

    if (normalized != nullptr) {
        *normalized = QString::number(value, 'f', 2);
    }
    return true;
}

const Common::Entities::SaleOrder *findSalesOrderById(
    const QVector<Common::Entities::SaleOrder> &orders, const QString &orderId)
{
    const auto orderIt =
        std::find_if(orders.cbegin(), orders.cend(), [&orderId](const auto &order) {
            return QString::fromStdString(order.id) == orderId;
        });
    return orderIt != orders.cend() ? &(*orderIt) : nullptr;
}

const Common::Entities::SalesOrderRecord *findSalesOrderRecordById(
    const QVector<Common::Entities::SalesOrderRecord> &records, const QString &recordId)
{
    const auto recordIt =
        std::find_if(records.cbegin(), records.cend(), [&recordId](const auto &record) {
            return QString::fromStdString(record.id) == recordId;
        });
    return recordIt != records.cend() ? &(*recordIt) : nullptr;
}

bool matchesSalesOrderSnapshot(const Common::Entities::SaleOrder &order, const QString &date,
                               const QString &userId, const QString &contactId,
                               const QString &employeeId, const QString &status)
{
    return QString::fromStdString(order.date) == date &&
           QString::fromStdString(order.userId) == userId &&
           QString::fromStdString(order.contactId) == contactId &&
           QString::fromStdString(order.employeeId) == employeeId &&
           canonicalSalesStatus(QString::fromStdString(order.status)) ==
               canonicalSalesStatus(status);
}

bool matchesSalesOrderRecordSnapshot(const Common::Entities::SalesOrderRecord &record,
                                     const QString &productTypeId, const QString &quantity,
                                     const QString &price)
{
    QString normalizedRecordQuantity = QString::fromStdString(record.quantity);
    QString normalizedRecordPrice    = QString::fromStdString(record.price);
    parseSalesQuantity(normalizedRecordQuantity, &normalizedRecordQuantity);
    parseSalesPrice(normalizedRecordPrice, &normalizedRecordPrice);

    return QString::fromStdString(record.productTypeId) == productTypeId &&
           normalizedRecordQuantity == quantity && normalizedRecordPrice == price;
}
} // namespace

SalesView::SalesView(SalesViewModel &viewModel, QWidget *parent)
    : QWidget(parent), m_viewModel(viewModel)
{
    m_invoiceExportPathProvider =
        [](QWidget *parentWidget, const QString &title, const QString &defaultFile,
           const QString &filter) {
            return QFileDialog::getSaveFileName(parentWidget, title, defaultFile, filter);
        };

    setupUi();
    installComboDelegate(
        m_ordersTable, 3, QStringLiteral("salesOrderUserCombo"),
        [this](const QModelIndex &) {
            QVector<TableComboChoice> choices;
            const auto users = m_viewModel.users();
            choices.reserve(users.size());
            for (const auto &user : users) {
                choices.push_back(
                    {QString::fromStdString(user.username), QString::fromStdString(user.id)});
            }
            return choices;
        },
        [this](const QModelIndex &index, const TableComboChoice &choice) {
            if (m_isSyncingTables) {
                return;
            }

            auto *userIdItem = m_ordersTable->item(index.row(), 2);
            if (userIdItem == nullptr || choice.value.toString().isEmpty()) {
                return;
            }

            {
                const QSignalBlocker blocker(m_ordersTable);
                userIdItem->setText(choice.value.toString());
                userIdItem->setData(Qt::UserRole, choice.value);
            }
            persistOrderRow(index.row());
        });
    installComboDelegate(
        m_ordersTable, 5, QStringLiteral("salesOrderCustomerCombo"),
        [this](const QModelIndex &) {
            QVector<TableComboChoice> choices;
            const auto contacts = m_viewModel.contacts();
            choices.reserve(contacts.size());
            for (const auto &contact : contacts) {
                choices.push_back(
                    {QString::fromStdString(contact.name), QString::fromStdString(contact.id)});
            }
            return choices;
        },
        [this](const QModelIndex &index, const TableComboChoice &choice) {
            if (m_isSyncingTables) {
                return;
            }

            auto *contactIdItem = m_ordersTable->item(index.row(), 4);
            if (contactIdItem == nullptr || choice.value.toString().isEmpty()) {
                return;
            }

            {
                const QSignalBlocker blocker(m_ordersTable);
                contactIdItem->setText(choice.value.toString());
                contactIdItem->setData(Qt::UserRole, choice.value);
            }
            persistOrderRow(index.row());
        });
    installComboDelegate(
        m_ordersTable, 7, QStringLiteral("salesOrderEmployeeCombo"),
        [this](const QModelIndex &) {
            QVector<TableComboChoice> choices;
            const auto employees = m_viewModel.employees();
            choices.reserve(employees.size());
            for (const auto &employee : employees) {
                choices.push_back(
                    {QString::fromStdString(employee.name), QString::fromStdString(employee.id)});
            }
            return choices;
        },
        [this](const QModelIndex &index, const TableComboChoice &choice) {
            if (m_isSyncingTables) {
                return;
            }

            auto *employeeIdItem = m_ordersTable->item(index.row(), 6);
            if (employeeIdItem == nullptr || choice.value.toString().isEmpty()) {
                return;
            }

            {
                const QSignalBlocker blocker(m_ordersTable);
                employeeIdItem->setText(choice.value.toString());
                employeeIdItem->setData(Qt::UserRole, choice.value);
            }
            persistOrderRow(index.row());
        });
    installComboDelegate(
        m_ordersTable, 8, QStringLiteral("salesOrderStatusCombo"),
        [this](const QModelIndex &index) {
            QVector<TableComboChoice> choices{
                {tr("Draft"), QStringLiteral("Draft")},
                {tr("Confirmed"), QStringLiteral("Confirmed")},
                {tr("Invoiced"), QStringLiteral("Invoiced")}};

            const auto *statusItem = m_ordersTable->item(index.row(), 8);
            const QString currentStatus =
                statusItem != nullptr ? statusItem->data(Qt::UserRole).toString() : QString();
            if (!currentStatus.isEmpty() &&
                std::none_of(choices.cbegin(), choices.cend(), [&](const TableComboChoice &choice) {
                    return choice.value.toString() == currentStatus;
                })) {
                choices.push_back({statusItem->text(), currentStatus});
            }
            return choices;
        },
        [this](const QModelIndex &index, const TableComboChoice &) {
            if (m_isSyncingTables) {
                return;
            }
            persistOrderRow(index.row());
        });
    installComboDelegate(
        m_orderRecordsTable, 3, QStringLiteral("salesRecordProductTypeCombo"),
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
            if (m_isSyncingTables) {
                return;
            }

            auto *productTypeIdItem = m_orderRecordsTable->item(index.row(), 2);
            if (productTypeIdItem == nullptr || choice.value.toString().isEmpty()) {
                return;
            }

            {
                const QSignalBlocker blocker(m_orderRecordsTable);
                productTypeIdItem->setText(choice.value.toString());
                productTypeIdItem->setData(Qt::UserRole, choice.value);
            }
            persistOrderRecordRow(index.row());
        });

    connect(this, &SalesView::errorOccurred, &m_viewModel, &SalesViewModel::errorOccurred);
    connect(&m_viewModel, &SalesViewModel::ordersChanged, this, &SalesView::onOrdersChanged);
    connect(&m_viewModel, &SalesViewModel::orderRecordsChanged, this,
            &SalesView::onOrderRecordsChanged);
    connect(&m_viewModel, &SalesViewModel::referenceDataChanged, this,
            &SalesView::onReferenceDataChanged);

    connect(m_addOrderButton, &QPushButton::clicked, this, &SalesView::onAddOrder);
    connect(m_deleteOrderButton, &QPushButton::clicked, this, &SalesView::onDeleteOrder);
    connect(m_addRecordButton, &QPushButton::clicked, this, &SalesView::onAddRecord);
    connect(m_deleteRecordButton, &QPushButton::clicked, this, &SalesView::onDeleteRecord);
    connect(m_exportInvoiceButton, &QPushButton::clicked, this, &SalesView::onExportInvoice);

    connect(m_ordersFilterField, &QLineEdit::textChanged, this, &SalesView::onOrdersFilterChanged);
    connect(m_invoicesFilterField, &QLineEdit::textChanged, this,
            &SalesView::onInvoicesFilterChanged);
    connect(m_ordersTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &SalesView::onOrdersSelectionChanged);
    connect(m_orderRecordsTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &SalesView::onOrderRecordsSelectionChanged);
    connect(m_invoiceOrdersTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &SalesView::onInvoiceSelectionChanged);
    connect(m_ordersTable, &QTableWidget::itemChanged, this,
            &SalesView::onOrdersTableItemChanged);
    connect(m_orderRecordsTable, &QTableWidget::itemChanged, this,
            &SalesView::onOrderRecordsTableItemChanged);
}

void SalesView::showSection(Section section)
{
    m_tabs->setCurrentIndex(section == Section::Orders ? 0 : 1);
}

void SalesView::setInvoiceExportPathProvider(InvoiceExportPathProvider provider)
{
    if (provider) {
        m_invoiceExportPathProvider = std::move(provider);
    }
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
    headerCard->setProperty("card", true);
    summaryLabel->setWordWrap(true);
    summaryLabel->setProperty("muted", true);
    titleLabel->setObjectName("salesTitleLabel");
    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(summaryLabel);

    m_tabs = new QTabWidget(this);
    m_tabs->setObjectName("salesTabs");

    auto *ordersPage = new QWidget(this);
    auto *ordersLayout = new QVBoxLayout(ordersPage);
    auto *ordersToolbar = new QHBoxLayout;
    m_ordersFilterField = new QLineEdit(ordersPage);
    m_ordersFilterField->setObjectName("salesOrdersFilterField");
    m_ordersFilterField->setPlaceholderText(tr("Search by date, customer, employee, or status"));
    m_ordersFilterField->setClearButtonEnabled(true);
    m_addOrderButton = new QPushButton(tr("Add Order"), ordersPage);
    m_deleteOrderButton = new QPushButton(tr("Delete Order"), ordersPage);
    m_addOrderButton->setObjectName("salesAddOrderButton");
    m_deleteOrderButton->setObjectName("salesDeleteOrderButton");
    m_deleteOrderButton->setProperty("destructive", true);
    ordersToolbar->addWidget(new QLabel(tr("Filter"), ordersPage));
    ordersToolbar->addWidget(m_ordersFilterField);
    ordersToolbar->addWidget(m_addOrderButton);
    ordersToolbar->addWidget(m_deleteOrderButton);

    m_ordersTable = new QTableWidget(ordersPage);
    m_ordersTable->setObjectName("salesOrdersTable");
    m_ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ordersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ordersTable->setAlternatingRowColors(true);
    m_ordersTable->setSortingEnabled(false);
    m_ordersTable->verticalHeader()->setVisible(false);
    m_ordersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    configureEditableTable(m_ordersTable, QStringLiteral("salesInlineEditor"));
    installDateEditDelegate(m_ordersTable, 1, QStringLiteral("yyyy-MM-dd"),
                            QStringLiteral("salesOrderDateEditor"));

    auto *recordsToolbar = new QHBoxLayout;
    m_addRecordButton = new QPushButton(tr("Add Line"), ordersPage);
    m_deleteRecordButton = new QPushButton(tr("Delete Line"), ordersPage);
    m_addRecordButton->setObjectName("salesAddRecordButton");
    m_deleteRecordButton->setObjectName("salesDeleteRecordButton");
    m_deleteRecordButton->setProperty("destructive", true);
    recordsToolbar->addWidget(new QLabel(tr("Order lines"), ordersPage));
    recordsToolbar->addStretch();
    recordsToolbar->addWidget(m_addRecordButton);
    recordsToolbar->addWidget(m_deleteRecordButton);

    m_orderRecordsTable = new QTableWidget(ordersPage);
    m_orderRecordsTable->setObjectName("salesOrderRecordsTable");
    m_orderRecordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_orderRecordsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_orderRecordsTable->setAlternatingRowColors(true);
    m_orderRecordsTable->setSortingEnabled(false);
    m_orderRecordsTable->verticalHeader()->setVisible(false);
    m_orderRecordsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    configureEditableTable(m_orderRecordsTable, QStringLiteral("salesInlineEditor"));
    installSpinBoxDelegate(m_orderRecordsTable, 4, 1, std::numeric_limits<int>::max(),
                           QStringLiteral("salesQuantityEditor"));
    installDoubleSpinBoxDelegate(m_orderRecordsTable, 5, 0.0, 1000000000.0, 2, 0.25,
                                 QStringLiteral("salesPriceEditor"));

    m_ordersStatusLabel = new QLabel(tr("Status: ready"), ordersPage);
    m_ordersStatusLabel->setProperty("muted", true);

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
    m_invoiceOrdersTable->setAlternatingRowColors(true);
    m_invoiceOrdersTable->setSortingEnabled(false);
    m_invoiceOrdersTable->verticalHeader()->setVisible(false);
    m_invoiceOrdersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    configureReadOnlyTable(m_invoiceOrdersTable);

    m_invoiceRecordsTable = new QTableWidget(invoicesPage);
    m_invoiceRecordsTable->setObjectName("salesInvoiceRecordsTable");
    m_invoiceRecordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_invoiceRecordsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_invoiceRecordsTable->setAlternatingRowColors(true);
    m_invoiceRecordsTable->setSortingEnabled(false);
    m_invoiceRecordsTable->verticalHeader()->setVisible(false);
    m_invoiceRecordsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    configureReadOnlyTable(m_invoiceRecordsTable);

    m_invoicePreview = new QPlainTextEdit(invoicesPage);
    m_invoicePreview->setObjectName("salesInvoicePreview");
    m_invoicePreview->setReadOnly(true);
    m_invoicePreview->setPlaceholderText(tr("Select an invoice to preview export output."));

    m_invoicesStatusLabel = new QLabel(tr("Status: ready"), invoicesPage);
    m_invoicesStatusLabel->setProperty("muted", true);

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
        "#salesView { background-color: transparent; }"
        "#salesTitleLabel { color: #0f172a; font-size: 24px; font-weight: 700; }");

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
    if (m_viewModel.users().isEmpty() || m_viewModel.contacts().isEmpty() ||
        m_viewModel.employees().isEmpty()) {
        emit errorOccurred(tr("Users, customers, and employees must exist before sales orders."));
        return;
    }

    Common::Entities::SaleOrder order;
    order.date = QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd")).toStdString();
    order.userId = m_viewModel.users().front().id;
    order.contactId = m_viewModel.contacts().front().id;
    order.employeeId = m_viewModel.employees().front().id;
    order.status = "Draft";
    m_viewModel.createOrder(order);
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

    if (m_viewModel.productTypes().isEmpty()) {
        emit errorOccurred(tr("Product types must exist before sales lines."));
        return;
    }

    Common::Entities::SalesOrderRecord record;
    record.orderId = orderId.toStdString();
    record.productTypeId = m_viewModel.productTypes().front().id;
    record.quantity = "1";
    record.price = "0.00";
    m_viewModel.createOrderRecord(record);
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
        m_currentRecords.clear();
        fillRecordsTable(m_orderRecordsTable, {});
        updateActions();
    }
}

void SalesView::onOrderRecordsSelectionChanged() { updateActions(); }

void SalesView::onInvoiceSelectionChanged()
{
    updateActions();
    const QString orderId = selectedOrderId(m_invoiceOrdersTable);
    if (!orderId.isEmpty()) {
        m_recordsContext = RecordsContext::Invoices;
        m_viewModel.fetchOrderRecords(orderId);
    } else {
        m_currentRecords.clear();
        fillRecordsTable(m_invoiceRecordsTable, {});
        m_invoicePreview->clear();
        updateActions();
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

void SalesView::onOrdersTableItemChanged(QTableWidgetItem *item)
{
    if (m_isSyncingTables || item == nullptr || item->column() != 1) {
        return;
    }

    const QString date = item->text().trimmed();
    if (!isValidSalesDate(date)) {
        emit errorOccurred(tr("Date must use the YYYY-MM-DD format."));
        restoreItemText(m_ordersTable, item, item->data(Qt::UserRole).toString());
        return;
    }
    if (restoreIfUnchanged(m_ordersTable, item, date)) {
        return;
    }

    persistOrderRow(item->row());
}

void SalesView::onOrderRecordsTableItemChanged(QTableWidgetItem *item)
{
    if (m_isSyncingTables || item == nullptr) {
        return;
    }

    if (item->column() == 4) {
        QString normalized;
        if (!parseSalesQuantity(item->text(), &normalized)) {
            emit errorOccurred(tr("Quantity must be a positive integer."));
            restoreItemText(m_orderRecordsTable, item, item->data(Qt::UserRole).toString());
            return;
        }
        if (restoreIfUnchanged(m_orderRecordsTable, item, normalized)) {
            return;
        }
    } else if (item->column() == 5) {
        QString normalized;
        if (!parseSalesPrice(item->text(), &normalized)) {
            emit errorOccurred(tr("Price must be a non-negative number."));
            restoreItemText(m_orderRecordsTable, item, item->data(Qt::UserRole).toString());
            return;
        }
        if (restoreIfUnchanged(m_orderRecordsTable, item, normalized)) {
            return;
        }
    } else {
        return;
    }

    persistOrderRecordRow(item->row());
}

void SalesView::onExportInvoice()
{
    const QString orderId = selectedOrderId(m_invoiceOrdersTable);
    if (orderId.isEmpty()) {
        emit errorOccurred(tr("Select an invoice before exporting."));
        return;
    }

    const QString defaultFile = QString("invoice-%1.txt").arg(orderId);
    const QString filePath = m_invoiceExportPathProvider(
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
                                const QVector<Common::Entities::SaleOrder> &orders)
{
    m_isSyncingTables = true;
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
        const QString orderStatus = canonicalSalesStatus(QString::fromStdString(order.status));
        const bool editableOrder = table == m_ordersTable;
        table->setItem(row, 0, createReadOnlyTableItem(QString::fromStdString(order.id)));
        table->setItem(row, 1, editableOrder
                                   ? createEditableTableItem(QString::fromStdString(order.date))
                                   : createReadOnlyTableItem(QString::fromStdString(order.date)));
        table->setItem(row, 2, createReadOnlyTableItem(userId));
        table->setItem(row, 3,
                       createComboTableItem(m_viewModel.userLabel(userId), userId, editableOrder));
        table->setItem(row, 4, createReadOnlyTableItem(contactId));
        table->setItem(
            row, 5, createComboTableItem(m_viewModel.contactLabel(contactId), contactId, editableOrder));
        table->setItem(row, 6, createReadOnlyTableItem(employeeId));
        table->setItem(row, 7, createComboTableItem(m_viewModel.employeeLabel(employeeId),
                                                    employeeId, editableOrder));
        table->setItem(row, 8, createComboTableItem(displaySalesStatus(order.status), orderStatus,
                                                    editableOrder));
    }

    table->setColumnHidden(0, true);
    table->setColumnHidden(2, true);
    table->setColumnHidden(4, true);
    table->setColumnHidden(6, true);
    refreshPersistentComboEditors(table);
    m_isSyncingTables = false;
}

void SalesView::fillRecordsTable(
    QTableWidget *table, const QVector<Common::Entities::SalesOrderRecord> &records)
{
    m_isSyncingTables = true;
    table->clearContents();
    table->setRowCount(records.size());
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels(
        {tr("ID"), tr("Order ID"), tr("Product Type ID"), tr("Product"), tr("Quantity"),
         tr("Price")});

    for (int row = 0; row < records.size(); ++row) {
        const auto &record = records[row];
        const QString productTypeId = QString::fromStdString(record.productTypeId);
        const bool editableRecords = table == m_orderRecordsTable;
        table->setItem(row, 0, createReadOnlyTableItem(QString::fromStdString(record.id)));
        table->setItem(row, 1, createReadOnlyTableItem(QString::fromStdString(record.orderId)));
        table->setItem(row, 2, createReadOnlyTableItem(productTypeId));
        table->setItem(row, 3,
                       createComboTableItem(m_viewModel.productTypeLabel(productTypeId),
                                            productTypeId, editableRecords));
        table->setItem(row, 4, editableRecords
                                   ? createEditableTableItem(QString::fromStdString(record.quantity))
                                   : createReadOnlyTableItem(QString::fromStdString(record.quantity)));
        table->setItem(row, 5, editableRecords
                                   ? createEditableTableItem(QString::fromStdString(record.price))
                                   : createReadOnlyTableItem(QString::fromStdString(record.price)));
    }

    table->setColumnHidden(0, true);
    table->setColumnHidden(1, true);
    table->setColumnHidden(2, true);
    refreshPersistentComboEditors(table);
    m_isSyncingTables = false;
}

void SalesView::updateActions()
{
    const bool hasOrderSelection = !m_ordersTable->selectedItems().isEmpty();
    const bool hasRecordSelection = !m_orderRecordsTable->selectedItems().isEmpty();
    const bool hasInvoiceSelection = !m_invoiceOrdersTable->selectedItems().isEmpty();
    m_deleteOrderButton->setEnabled(hasOrderSelection);
    m_addRecordButton->setEnabled(hasOrderSelection);
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

void SalesView::persistOrderRow(int row)
{
    auto *idItem = m_ordersTable->item(row, 0);
    auto *dateItem = m_ordersTable->item(row, 1);
    auto *userIdItem = m_ordersTable->item(row, 2);
    auto *contactIdItem = m_ordersTable->item(row, 4);
    auto *employeeIdItem = m_ordersTable->item(row, 6);
    auto *statusItem = m_ordersTable->item(row, 8);
    if (idItem == nullptr || dateItem == nullptr || userIdItem == nullptr ||
        contactIdItem == nullptr || employeeIdItem == nullptr || statusItem == nullptr) {
        return;
    }

    const QString date = dateItem->text().trimmed();
    if (!isValidSalesDate(date)) {
        emit errorOccurred(tr("Date must use the YYYY-MM-DD format."));
        restoreItemText(m_ordersTable, dateItem, dateItem->data(Qt::UserRole).toString());
        return;
    }

    const QString userId = userIdItem->text().trimmed();
    const QString contactId = contactIdItem->text().trimmed();
    const QString employeeId = employeeIdItem->text().trimmed();
    const QString status = statusItem->data(Qt::UserRole).toString().trimmed();
    if (userId.isEmpty() || contactId.isEmpty() || employeeId.isEmpty() || status.isEmpty()) {
        emit errorOccurred(tr("Sales order references must stay assigned."));
        return;
    }

    Common::Entities::SaleOrder order{.id = idItem->text().toStdString(),
                                      .date = date.toStdString(),
                                      .userId = userId.toStdString(),
                                      .contactId = contactId.toStdString(),
                                      .employeeId = employeeId.toStdString(),
                                      .status = status.toStdString()};
    {
        const QSignalBlocker blocker(m_ordersTable);
        dateItem->setData(Qt::UserRole, date);
    }

    if (const auto *existingOrder = findSalesOrderById(m_allOrders, idItem->text());
        existingOrder != nullptr &&
        matchesSalesOrderSnapshot(*existingOrder, date, userId, contactId, employeeId, status)) {
        return;
    }

    m_viewModel.editOrder(order);
}

void SalesView::persistOrderRecordRow(int row)
{
    auto *idItem = m_orderRecordsTable->item(row, 0);
    auto *orderIdItem = m_orderRecordsTable->item(row, 1);
    auto *productTypeIdItem = m_orderRecordsTable->item(row, 2);
    auto *quantityItem = m_orderRecordsTable->item(row, 4);
    auto *priceItem = m_orderRecordsTable->item(row, 5);
    if (idItem == nullptr || orderIdItem == nullptr || productTypeIdItem == nullptr ||
        quantityItem == nullptr || priceItem == nullptr) {
        return;
    }

    const QString productTypeId = productTypeIdItem->text().trimmed();
    QString normalizedQuantity;
    QString normalizedPrice;
    if (productTypeId.isEmpty()) {
        emit errorOccurred(tr("Product type is required."));
        return;
    }
    if (!parseSalesQuantity(quantityItem->text(), &normalizedQuantity)) {
        emit errorOccurred(tr("Quantity must be a positive integer."));
        restoreItemText(m_orderRecordsTable, quantityItem,
                        quantityItem->data(Qt::UserRole).toString());
        return;
    }
    if (!parseSalesPrice(priceItem->text(), &normalizedPrice)) {
        emit errorOccurred(tr("Price must be a non-negative number."));
        restoreItemText(m_orderRecordsTable, priceItem, priceItem->data(Qt::UserRole).toString());
        return;
    }

    {
        const QSignalBlocker blocker(m_orderRecordsTable);
        quantityItem->setText(normalizedQuantity);
        priceItem->setText(normalizedPrice);
        quantityItem->setData(Qt::UserRole, normalizedQuantity);
        priceItem->setData(Qt::UserRole, normalizedPrice);
    }

    if (const auto *existingRecord = findSalesOrderRecordById(m_currentRecords, idItem->text());
        existingRecord != nullptr &&
        matchesSalesOrderRecordSnapshot(*existingRecord, productTypeId, normalizedQuantity,
                                        normalizedPrice)) {
        return;
    }

    Common::Entities::SalesOrderRecord record{.id = idItem->text().toStdString(),
                                              .orderId = orderIdItem->text().toStdString(),
                                              .productTypeId = productTypeId.toStdString(),
                                              .quantity = normalizedQuantity.toStdString(),
                                              .price = normalizedPrice.toStdString()};
    m_viewModel.editOrderRecord(record);
}
