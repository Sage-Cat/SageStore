#include "Ui/Views/PurchaseView.hpp"

#include "Ui/Views/TableInteractionHelpers.hpp"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDate>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <algorithm>
#include <limits>

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

bool isLockedPurchaseStatus(const QString &status)
{
    return canonicalStatus(status) == QStringLiteral("Received");
}

bool isValidIsoDate(const QString &text)
{
    return QDate::fromString(text.trimmed(), QStringLiteral("yyyy-MM-dd")).isValid();
}

bool parsePositiveInteger(const QString &text, QString *normalized = nullptr)
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

bool parseNonNegativeDecimal(const QString &text, QString *normalized = nullptr)
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

const Common::Entities::PurchaseOrder *findOrderById(
    const QVector<Common::Entities::PurchaseOrder> &orders, const QString &orderId)
{
    const auto orderIt =
        std::find_if(orders.cbegin(), orders.cend(), [&orderId](const auto &order) {
            return QString::fromStdString(order.id) == orderId;
        });
    return orderIt != orders.cend() ? &(*orderIt) : nullptr;
}

const Common::Entities::PurchaseOrderRecord *findOrderRecordById(
    const QVector<Common::Entities::PurchaseOrderRecord> &records, const QString &recordId)
{
    const auto recordIt =
        std::find_if(records.cbegin(), records.cend(), [&recordId](const auto &record) {
            return QString::fromStdString(record.id) == recordId;
        });
    return recordIt != records.cend() ? &(*recordIt) : nullptr;
}

bool matchesOrderSnapshot(const Common::Entities::PurchaseOrder &order, const QString &date,
                          const QString &userId, const QString &supplierId, const QString &status)
{
    return QString::fromStdString(order.date) == date &&
           QString::fromStdString(order.userId) == userId &&
           QString::fromStdString(order.supplierId) == supplierId &&
           canonicalStatus(QString::fromStdString(order.status)) == canonicalStatus(status);
}

bool matchesOrderRecordSnapshot(const Common::Entities::PurchaseOrderRecord &record,
                                const QString &productTypeId, const QString &quantity,
                                const QString &price)
{
    QString normalizedRecordQuantity = QString::fromStdString(record.quantity);
    QString normalizedRecordPrice    = QString::fromStdString(record.price);
    parsePositiveInteger(normalizedRecordQuantity, &normalizedRecordQuantity);
    parseNonNegativeDecimal(normalizedRecordPrice, &normalizedRecordPrice);

    return QString::fromStdString(record.productTypeId) == productTypeId &&
           normalizedRecordQuantity == quantity && normalizedRecordPrice == price;
}
} // namespace

PurchaseView::PurchaseView(PurchaseViewModel &viewModel, QWidget *parent)
    : QWidget(parent), m_viewModel(viewModel)
{
    setupUi();
    installComboDelegate(
        m_ordersTable, 3, QStringLiteral("purchaseOrderUserCombo"),
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
        m_ordersTable, 5, QStringLiteral("purchaseOrderSupplierCombo"),
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
            if (m_isSyncingTables) {
                return;
            }

            auto *supplierIdItem = m_ordersTable->item(index.row(), 4);
            if (supplierIdItem == nullptr || choice.value.toString().isEmpty()) {
                return;
            }

            {
                const QSignalBlocker blocker(m_ordersTable);
                supplierIdItem->setText(choice.value.toString());
                supplierIdItem->setData(Qt::UserRole, choice.value);
            }
            persistOrderRow(index.row());
        });
    installComboDelegate(
        m_ordersTable, 6, QStringLiteral("purchaseOrderStatusCombo"),
        [this](const QModelIndex &index) {
            QVector<TableComboChoice> choices{
                {tr("Draft"), QStringLiteral("Draft")},
                {tr("Ordered"), QStringLiteral("Ordered")}};

            const auto *statusItem = m_ordersTable->item(index.row(), 6);
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
        m_orderRecordsTable, 3, QStringLiteral("purchaseRecordProductTypeCombo"),
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

    connect(this, &PurchaseView::errorOccurred, &m_viewModel, &PurchaseViewModel::errorOccurred);
    connect(&m_viewModel, &PurchaseViewModel::ordersChanged, this, &PurchaseView::onOrdersChanged);
    connect(&m_viewModel, &PurchaseViewModel::orderRecordsChanged, this,
            &PurchaseView::onOrderRecordsChanged);
    connect(&m_viewModel, &PurchaseViewModel::referenceDataChanged, this,
            &PurchaseView::onReferenceDataChanged);
    connect(&m_viewModel, &PurchaseViewModel::purchaseReceiptPosted, this,
            &PurchaseView::onPurchaseReceiptPosted);

    connect(m_addOrderButton, &QPushButton::clicked, this, &PurchaseView::onAddOrder);
    connect(m_deleteOrderButton, &QPushButton::clicked, this, &PurchaseView::onDeleteOrder);
    connect(m_addRecordButton, &QPushButton::clicked, this, &PurchaseView::onAddRecord);
    connect(m_deleteRecordButton, &QPushButton::clicked, this, &PurchaseView::onDeleteRecord);
    connect(m_receiveButton, &QPushButton::clicked, this, &PurchaseView::onReceiveOrder);

    connect(m_ordersFilterField, &QLineEdit::textChanged, this, &PurchaseView::onOrdersFilterChanged);
    connect(m_receiptsFilterField, &QLineEdit::textChanged, this,
            &PurchaseView::onReceiptsFilterChanged);
    connect(m_ordersTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &PurchaseView::onOrdersSelectionChanged);
    connect(m_orderRecordsTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &PurchaseView::onOrderRecordsSelectionChanged);
    connect(m_receiptOrdersTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &PurchaseView::onReceiptOrdersSelectionChanged);
    connect(m_ordersTable, &QTableWidget::itemChanged, this,
            &PurchaseView::onOrdersTableItemChanged);
    connect(m_orderRecordsTable, &QTableWidget::itemChanged, this,
            &PurchaseView::onOrderRecordsTableItemChanged);
    connect(m_receiptEmployeeBox, qOverload<int>(&QComboBox::currentIndexChanged), this,
            [this](int) { updateReceiptActions(); });
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
    headerCard->setProperty("card", true);
    summaryLabel->setWordWrap(true);
    summaryLabel->setProperty("muted", true);
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
    m_deleteOrderButton = new QPushButton(tr("Delete Order"), ordersPage);
    m_deleteOrderButton->setObjectName("purchaseDeleteOrderButton");
    m_deleteOrderButton->setProperty("destructive", true);
    ordersToolbar->addWidget(new QLabel(tr("Filter"), ordersPage));
    ordersToolbar->addWidget(m_ordersFilterField);
    ordersToolbar->addWidget(m_addOrderButton);
    ordersToolbar->addWidget(m_deleteOrderButton);

    m_ordersTable = new QTableWidget(ordersPage);
    m_ordersTable->setObjectName("purchaseOrdersTable");
    m_ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ordersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ordersTable->setAlternatingRowColors(true);
    m_ordersTable->setSortingEnabled(false);
    m_ordersTable->verticalHeader()->setVisible(false);
    m_ordersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    configureEditableTable(m_ordersTable, QStringLiteral("purchaseInlineEditor"));
    installDateEditDelegate(m_ordersTable, 1, QStringLiteral("yyyy-MM-dd"),
                            QStringLiteral("purchaseOrderDateEditor"));

    auto *recordsToolbar = new QHBoxLayout;
    m_addRecordButton = new QPushButton(tr("Add Line"), ordersPage);
    m_addRecordButton->setObjectName("purchaseAddRecordButton");
    m_deleteRecordButton = new QPushButton(tr("Delete Line"), ordersPage);
    m_deleteRecordButton->setObjectName("purchaseDeleteRecordButton");
    m_deleteRecordButton->setProperty("destructive", true);
    recordsToolbar->addWidget(new QLabel(tr("Order lines"), ordersPage));
    recordsToolbar->addStretch();
    recordsToolbar->addWidget(m_addRecordButton);
    recordsToolbar->addWidget(m_deleteRecordButton);

    m_orderRecordsTable = new QTableWidget(ordersPage);
    m_orderRecordsTable->setObjectName("purchaseOrderRecordsTable");
    m_orderRecordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_orderRecordsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_orderRecordsTable->setAlternatingRowColors(true);
    m_orderRecordsTable->setSortingEnabled(false);
    m_orderRecordsTable->verticalHeader()->setVisible(false);
    m_orderRecordsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    configureEditableTable(m_orderRecordsTable, QStringLiteral("purchaseInlineEditor"));
    installSpinBoxDelegate(m_orderRecordsTable, 4, 1, std::numeric_limits<int>::max(),
                           QStringLiteral("purchaseQuantityEditor"));
    installDoubleSpinBoxDelegate(m_orderRecordsTable, 5, 0.0, 1000000000.0, 2, 0.25,
                                 QStringLiteral("purchasePriceEditor"));

    m_ordersStatusLabel = new QLabel(tr("Status: ready"), ordersPage);
    m_ordersStatusLabel->setObjectName("purchaseOrdersStatusLabel");
    m_ordersStatusLabel->setProperty("muted", true);

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
    m_receiptEmployeeBox = new FormComboBox(receiptsPage);
    configureFormComboBox(m_receiptEmployeeBox, QStringLiteral("purchaseReceiptEmployeeBox"));
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
    m_receiptOrdersTable->setAlternatingRowColors(true);
    m_receiptOrdersTable->setSortingEnabled(false);
    m_receiptOrdersTable->verticalHeader()->setVisible(false);
    m_receiptOrdersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    configureReadOnlyTable(m_receiptOrdersTable);

    m_receiptRecordsTable = new QTableWidget(receiptsPage);
    m_receiptRecordsTable->setObjectName("purchaseReceiptRecordsTable");
    m_receiptRecordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_receiptRecordsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_receiptRecordsTable->setAlternatingRowColors(true);
    m_receiptRecordsTable->setSortingEnabled(false);
    m_receiptRecordsTable->verticalHeader()->setVisible(false);
    m_receiptRecordsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    configureReadOnlyTable(m_receiptRecordsTable);

    m_receiptsStatusLabel = new QLabel(tr("Status: ready"), receiptsPage);
    m_receiptsStatusLabel->setObjectName("purchaseReceiptsStatusLabel");
    m_receiptsStatusLabel->setProperty("muted", true);

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
        "#purchaseView { background-color: transparent; }"
        "#purchaseTitleLabel { color: #0f172a; font-size: 24px; font-weight: 700; }");

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
    updateOrdersActions();
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
    updateOrdersActions();
    updateReceiptActions();
}

void PurchaseView::onPurchaseReceiptPosted()
{
    m_receiptsStatusLabel->setText(tr("Status: goods receipt posted to stock"));
    m_viewModel.fetchOrders();
}

void PurchaseView::onAddOrder()
{
    if (m_viewModel.users().isEmpty() || m_viewModel.suppliers().isEmpty()) {
        emit errorOccurred(tr("Users and suppliers must exist before creating purchase orders."));
        return;
    }

    Common::Entities::PurchaseOrder order;
    order.date = QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd")).toStdString();
    order.userId = m_viewModel.users().front().id;
    order.supplierId = m_viewModel.suppliers().front().id;
    order.status = "Draft";
    m_viewModel.createOrder(order);
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

    if (isLockedPurchaseStatus(selectedOrderStatus(m_ordersTable))) {
        emit errorOccurred(tr("Received purchase orders cannot be edited."));
        return;
    }

    if (m_viewModel.productTypes().isEmpty()) {
        emit errorOccurred(tr("Product types must exist before creating purchase lines."));
        return;
    }

    Common::Entities::PurchaseOrderRecord record;
    record.orderId = orderId.toStdString();
    record.productTypeId = m_viewModel.productTypes().front().id;
    record.quantity = "1";
    record.price = "0.00";
    m_viewModel.createOrderRecord(record);
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
        m_currentRecords.clear();
        fillRecordsTable(m_orderRecordsTable, {});
        updateOrdersActions();
    }
}

void PurchaseView::onOrderRecordsSelectionChanged() { updateOrdersActions(); }

void PurchaseView::onReceiptOrdersSelectionChanged()
{
    updateReceiptActions();
    const QString orderId = selectedOrderId(m_receiptOrdersTable);
    if (!orderId.isEmpty()) {
        m_recordsContext = RecordsContext::Receipts;
        m_viewModel.fetchOrderRecords(orderId);
    } else {
        m_currentRecords.clear();
        fillRecordsTable(m_receiptRecordsTable, {});
        updateReceiptActions();
    }
}

void PurchaseView::onOrdersTableItemChanged(QTableWidgetItem *item)
{
    if (m_isSyncingTables || item == nullptr || item->column() != 1) {
        return;
    }

    const QString date = item->text().trimmed();
    if (!isValidIsoDate(date)) {
        emit errorOccurred(tr("Date must use the YYYY-MM-DD format."));
        restoreItemText(m_ordersTable, item, item->data(Qt::UserRole).toString());
        return;
    }
    if (restoreIfUnchanged(m_ordersTable, item, date)) {
        return;
    }

    persistOrderRow(item->row());
}

void PurchaseView::onOrderRecordsTableItemChanged(QTableWidgetItem *item)
{
    if (m_isSyncingTables || item == nullptr) {
        return;
    }

    if (item->column() == 4) {
        QString normalized;
        if (!parsePositiveInteger(item->text(), &normalized)) {
            emit errorOccurred(tr("Quantity must be a positive integer."));
            restoreItemText(m_orderRecordsTable, item, item->data(Qt::UserRole).toString());
            return;
        }
        if (restoreIfUnchanged(m_orderRecordsTable, item, normalized)) {
            return;
        }
    } else if (item->column() == 5) {
        QString normalized;
        if (!parseNonNegativeDecimal(item->text(), &normalized)) {
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
    QTableWidget *table, const QVector<Common::Entities::PurchaseOrder> &orders)
{
    m_isSyncingTables = true;
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
        const bool editableOrder = table == m_ordersTable && !isLockedPurchaseStatus(orderStatus);
        table->setItem(row, 0, createReadOnlyTableItem(QString::fromStdString(order.id)));
        table->setItem(row, 1, editableOrder
                                   ? createEditableTableItem(QString::fromStdString(order.date))
                                   : createReadOnlyTableItem(QString::fromStdString(order.date)));
        table->setItem(row, 2, createReadOnlyTableItem(userId));
        table->setItem(row, 3,
                       createComboTableItem(m_viewModel.userLabel(userId), userId, editableOrder));
        table->setItem(row, 4, createReadOnlyTableItem(supplierId));
        table->setItem(row, 5, createComboTableItem(m_viewModel.supplierLabel(supplierId),
                                                    supplierId, editableOrder));
        table->setItem(row, 6,
                       createComboTableItem(displayStatus(order.status), orderStatus, editableOrder));
    }

    table->setColumnHidden(0, true);
    table->setColumnHidden(2, true);
    table->setColumnHidden(4, true);
    refreshPersistentComboEditors(table);
    m_isSyncingTables = false;
}

void PurchaseView::fillRecordsTable(
    QTableWidget *table, const QVector<Common::Entities::PurchaseOrderRecord> &records)
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
        const bool editableRecords =
            table == m_orderRecordsTable && !isLockedPurchaseStatus(selectedOrderStatus(m_ordersTable));
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

void PurchaseView::updateOrdersActions()
{
    const bool hasOrderSelection = !m_ordersTable->selectedItems().isEmpty();
    const bool selectedOrderIsReceived =
        selectedOrderStatus(m_ordersTable) == QStringLiteral("Received");
    const bool hasRecordSelection = !m_orderRecordsTable->selectedItems().isEmpty();
    m_deleteOrderButton->setEnabled(hasOrderSelection && !selectedOrderIsReceived);
    m_addRecordButton->setEnabled(hasOrderSelection && !selectedOrderIsReceived);
    m_deleteRecordButton->setEnabled(hasRecordSelection && !selectedOrderIsReceived);
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

QString PurchaseView::selectedOrderStatus(QTableWidget *table) const
{
    const auto selected = table->selectedItems();
    if (selected.isEmpty()) {
        return {};
    }

    auto *statusItem = table->item(selected.first()->row(), 6);
    if (statusItem == nullptr) {
        return {};
    }

    return statusItem->data(Qt::UserRole).toString();
}

void PurchaseView::persistOrderRow(int row)
{
    auto *idItem = m_ordersTable->item(row, 0);
    auto *dateItem = m_ordersTable->item(row, 1);
    auto *userIdItem = m_ordersTable->item(row, 2);
    auto *supplierIdItem = m_ordersTable->item(row, 4);
    auto *statusItem = m_ordersTable->item(row, 6);
    if (idItem == nullptr || dateItem == nullptr || userIdItem == nullptr ||
        supplierIdItem == nullptr || statusItem == nullptr) {
        return;
    }

    const QString date = dateItem->text().trimmed();
    if (!isValidIsoDate(date)) {
        emit errorOccurred(tr("Date must use the YYYY-MM-DD format."));
        restoreItemText(m_ordersTable, dateItem, dateItem->data(Qt::UserRole).toString());
        return;
    }

    const QString userId = userIdItem->text().trimmed();
    const QString supplierId = supplierIdItem->text().trimmed();
    const QString status = statusItem->data(Qt::UserRole).toString().trimmed();
    if (userId.isEmpty() || supplierId.isEmpty() || status.isEmpty()) {
        emit errorOccurred(tr("Purchase order references must stay assigned."));
        return;
    }

    Common::Entities::PurchaseOrder order{.id = idItem->text().toStdString(),
                                          .date = date.toStdString(),
                                          .userId = userId.toStdString(),
                                          .supplierId = supplierId.toStdString(),
                                          .status = status.toStdString()};
    {
        const QSignalBlocker blocker(m_ordersTable);
        dateItem->setData(Qt::UserRole, date);
    }

    if (const auto *existingOrder = findOrderById(m_allOrders, idItem->text());
        existingOrder != nullptr &&
        matchesOrderSnapshot(*existingOrder, date, userId, supplierId, status)) {
        return;
    }

    m_viewModel.editOrder(order);
}

void PurchaseView::persistOrderRecordRow(int row)
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
    if (!parsePositiveInteger(quantityItem->text(), &normalizedQuantity)) {
        emit errorOccurred(tr("Quantity must be a positive integer."));
        restoreItemText(m_orderRecordsTable, quantityItem,
                        quantityItem->data(Qt::UserRole).toString());
        return;
    }
    if (!parseNonNegativeDecimal(priceItem->text(), &normalizedPrice)) {
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

    if (const auto *existingRecord = findOrderRecordById(m_currentRecords, idItem->text());
        existingRecord != nullptr &&
        matchesOrderRecordSnapshot(*existingRecord, productTypeId, normalizedQuantity,
                                   normalizedPrice)) {
        return;
    }

    Common::Entities::PurchaseOrderRecord record{.id = idItem->text().toStdString(),
                                                 .orderId = orderIdItem->text().toStdString(),
                                                 .productTypeId = productTypeId.toStdString(),
                                                 .quantity = normalizedQuantity.toStdString(),
                                                 .price = normalizedPrice.toStdString()};
    m_viewModel.editOrderRecord(record);
}
