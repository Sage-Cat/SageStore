#include "Ui/Views/ManagementView.hpp"

#include "Ui/Views/TableInteractionHelpers.hpp"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDateTime>
#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <algorithm>

#include "Ui/ViewModels/ManagementViewModel.hpp"

namespace {
bool containsInsensitive(const QString &haystack, const QString &needle)
{
    return haystack.contains(needle, Qt::CaseInsensitive);
}

QString canonicalContactType(const QString &type)
{
    const QString normalized = type.trimmed();
    if (normalized.compare(QStringLiteral("Customer"), Qt::CaseInsensitive) == 0 ||
        normalized == QStringLiteral("Клієнт")) {
        return QStringLiteral("Customer");
    }
    if (normalized.compare(QStringLiteral("Client"), Qt::CaseInsensitive) == 0) {
        return QStringLiteral("Client");
    }
    if (normalized.compare(QStringLiteral("Partner"), Qt::CaseInsensitive) == 0 ||
        normalized == QStringLiteral("Партнер")) {
        return QStringLiteral("Partner");
    }

    return normalized;
}

QString displayContactType(const std::string &type)
{
    const QString canonical = canonicalContactType(QString::fromStdString(type));
    if (canonical == QStringLiteral("Customer")) {
        return QObject::tr("Customer");
    }
    if (canonical == QStringLiteral("Client")) {
        return QObject::tr("Client");
    }
    if (canonical == QStringLiteral("Partner")) {
        return QObject::tr("Partner");
    }

    return canonical;
}

QString defaultEntitySuffix()
{
    return QDateTime::currentDateTimeUtc().toString(QStringLiteral("hhmmsszzz"));
}
} // namespace

ManagementView::ManagementView(ManagementViewModel &viewModel, QWidget *parent)
    : QWidget(parent), m_viewModel(viewModel)
{
    setupUi();
    connectSignals();
    installComboDelegate(
        m_contactsUi.table, 2, QStringLiteral("managementContactTypeCombo"),
        [](const QModelIndex &) {
            return QVector<TableComboChoice>{
                {QObject::tr("Customer"), QStringLiteral("Customer")},
                {QObject::tr("Client"), QStringLiteral("Client")},
                {QObject::tr("Partner"), QStringLiteral("Partner")}};
        },
        [this](const QModelIndex &index, const TableComboChoice &) {
            if (m_isSyncingTable) {
                return;
            }
            persistContactRow(index.row());
        });
}

void ManagementView::showSection(Section section)
{
    switch (section) {
    case Section::Contacts:
        m_sections->setCurrentIndex(0);
        break;
    case Section::Suppliers:
        m_sections->setCurrentIndex(1);
        break;
    case Section::Employees:
        m_sections->setCurrentIndex(2);
        break;
    }
}

ManagementView::CrudUi ManagementView::createCrudSection(const QString &objectPrefix,
                                                         const QString &placeholder) const
{
    CrudUi ui;
    ui.page = new QWidget;

    auto *layout = new QVBoxLayout(ui.page);
    auto *filterRow = new QHBoxLayout;
    auto *buttonsRow = new QHBoxLayout;

    ui.filterField = new QLineEdit(ui.page);
    ui.filterField->setObjectName(objectPrefix + "FilterField");
    ui.filterField->setPlaceholderText(placeholder);
    ui.filterField->setClearButtonEnabled(true);

    ui.addButton = new QPushButton(tr("Add"), ui.page);
    ui.addButton->setObjectName(objectPrefix + "AddButton");
    ui.deleteButton = new QPushButton(tr("Delete"), ui.page);
    ui.deleteButton->setObjectName(objectPrefix + "DeleteButton");

    filterRow->addWidget(new QLabel(tr("Filter"), ui.page));
    filterRow->addWidget(ui.filterField);

    buttonsRow->addWidget(ui.addButton);
    buttonsRow->addWidget(ui.deleteButton);
    buttonsRow->addStretch();

    ui.table = new QTableWidget(ui.page);
    ui.table->setObjectName(objectPrefix + "Table");
    ui.table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.table->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.table->setAlternatingRowColors(true);
    ui.table->setSortingEnabled(false);
    ui.table->verticalHeader()->setVisible(false);
    ui.table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    configureEditableTable(ui.table, objectPrefix + QStringLiteral("InlineEditor"));

    ui.statusLabel = new QLabel(tr("Status: ready"), ui.page);
    ui.statusLabel->setObjectName(objectPrefix + "StatusLabel");
    ui.statusLabel->setProperty("muted", true);

    layout->addLayout(filterRow);
    layout->addLayout(buttonsRow);
    layout->addWidget(ui.table);
    layout->addWidget(ui.statusLabel);

    return ui;
}

void ManagementView::setupUi()
{
    auto *rootLayout = new QVBoxLayout(this);
    auto *headerCard = new QFrame(this);
    auto *headerLayout = new QVBoxLayout(headerCard);
    auto *titleLabel = new QLabel(tr("Master Data"), headerCard);
    auto *summaryLabel = new QLabel(
        tr("Maintain shared business directories for customers, suppliers, and employees. These "
           "records are reused by purchasing, sales, stock, and reporting workflows."),
        headerCard);

    headerCard->setProperty("card", true);
    titleLabel->setObjectName("managementTitleLabel");
    summaryLabel->setObjectName("managementSummaryLabel");
    summaryLabel->setProperty("muted", true);
    summaryLabel->setWordWrap(true);

    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(summaryLabel);

    m_sections = new QTabWidget(this);
    m_sections->setObjectName("managementSections");
    m_contactsUi =
        createCrudSection("managementContacts", tr("Search by name, type, email, or phone"));
    m_suppliersUi =
        createCrudSection("managementSuppliers", tr("Search by name, number, email, or address"));
    m_employeesUi =
        createCrudSection("managementEmployees", tr("Search by name, number, email, or address"));

    styleSection(m_contactsUi);
    styleSection(m_suppliersUi);
    styleSection(m_employeesUi);

    m_sections->addTab(m_contactsUi.page, tr("Customers"));
    m_sections->addTab(m_suppliersUi.page, tr("Suppliers"));
    m_sections->addTab(m_employeesUi.page, tr("Employees"));

    rootLayout->addWidget(headerCard);
    rootLayout->addWidget(m_sections);

    setObjectName("managementView");
    setStyleSheet(
        "#managementView { background-color: transparent; }"
        "#managementTitleLabel { color: #0f172a; font-size: 24px; font-weight: 700; }");
}

void ManagementView::connectSignals()
{
    connect(this, &ManagementView::errorOccurred, &m_viewModel, &ManagementViewModel::errorOccurred);
    connect(&m_viewModel, &ManagementViewModel::contactsChanged, this,
            &ManagementView::onContactsChanged);
    connect(&m_viewModel, &ManagementViewModel::suppliersChanged, this,
            &ManagementView::onSuppliersChanged);
    connect(&m_viewModel, &ManagementViewModel::employeesChanged, this,
            &ManagementView::onEmployeesChanged);

    connect(m_contactsUi.addButton, &QPushButton::clicked, this, &ManagementView::onContactAdd);
    connect(m_contactsUi.deleteButton, &QPushButton::clicked, this,
            &ManagementView::onContactDelete);
    connect(m_contactsUi.filterField, &QLineEdit::textChanged, this,
            &ManagementView::refreshContacts);
    connect(m_contactsUi.table->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            [this](const QItemSelection &, const QItemSelection &) {
                updateActionButtons(m_contactsUi);
            });
    connect(m_contactsUi.table, &QTableWidget::itemChanged, this,
            &ManagementView::onContactItemChanged);

    connect(m_suppliersUi.addButton, &QPushButton::clicked, this, &ManagementView::onSupplierAdd);
    connect(m_suppliersUi.deleteButton, &QPushButton::clicked, this,
            &ManagementView::onSupplierDelete);
    connect(m_suppliersUi.filterField, &QLineEdit::textChanged, this,
            &ManagementView::refreshSuppliers);
    connect(m_suppliersUi.table->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            [this](const QItemSelection &, const QItemSelection &) {
                updateActionButtons(m_suppliersUi);
            });
    connect(m_suppliersUi.table, &QTableWidget::itemChanged, this,
            &ManagementView::onSupplierItemChanged);

    connect(m_employeesUi.addButton, &QPushButton::clicked, this, &ManagementView::onEmployeeAdd);
    connect(m_employeesUi.deleteButton, &QPushButton::clicked, this,
            &ManagementView::onEmployeeDelete);
    connect(m_employeesUi.filterField, &QLineEdit::textChanged, this,
            &ManagementView::refreshEmployees);
    connect(m_employeesUi.table->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            [this](const QItemSelection &, const QItemSelection &) {
                updateActionButtons(m_employeesUi);
            });
    connect(m_employeesUi.table, &QTableWidget::itemChanged, this,
            &ManagementView::onEmployeeItemChanged);
}

void ManagementView::styleSection(CrudUi &ui) const
{
    ui.deleteButton->setProperty("destructive", true);
    updateActionButtons(ui);
}

void ManagementView::updateActionButtons(CrudUi &ui) const
{
    const bool hasSelection = !ui.table->selectedItems().isEmpty();
    ui.deleteButton->setEnabled(hasSelection);
}

void ManagementView::onContactsChanged()
{
    m_allContacts = m_viewModel.contacts();
    applyContactsFilter();
}

void ManagementView::onSuppliersChanged()
{
    m_allSuppliers = m_viewModel.suppliers();
    applySuppliersFilter();
}

void ManagementView::onEmployeesChanged()
{
    m_allEmployees = m_viewModel.employees();
    applyEmployeesFilter();
}

void ManagementView::refreshContacts() { applyContactsFilter(); }

void ManagementView::refreshSuppliers() { applySuppliersFilter(); }

void ManagementView::refreshEmployees() { applyEmployeesFilter(); }

void ManagementView::applyContactsFilter()
{
    const QString filter = m_contactsUi.filterField->text().trimmed();
    QVector<Common::Entities::Contact> filtered;
    for (const auto &contact : m_allContacts) {
        const QString canonicalType = canonicalContactType(QString::fromStdString(contact.type));
        const QStringList haystack = {QString::fromStdString(contact.name), canonicalType,
                                      displayContactType(contact.type),
                                      QString::fromStdString(contact.email),
                                      QString::fromStdString(contact.phone)};
        if (filter.isEmpty() || std::any_of(haystack.begin(), haystack.end(), [&](const QString &value) {
                return containsInsensitive(value, filter);
            })) {
            filtered.push_back(contact);
        }
    }

    fillContactsTable(filtered);
    m_contactsUi.statusLabel->setText(
        tr("Status: showing %1 of %2 customer records")
            .arg(QString::number(filtered.size()), QString::number(m_allContacts.size())));
}

void ManagementView::applySuppliersFilter()
{
    const QString filter = m_suppliersUi.filterField->text().trimmed();
    QVector<Common::Entities::Supplier> filtered;
    for (const auto &supplier : m_allSuppliers) {
        const QStringList haystack = {QString::fromStdString(supplier.name),
                                      QString::fromStdString(supplier.number),
                                      QString::fromStdString(supplier.email),
                                      QString::fromStdString(supplier.address)};
        if (filter.isEmpty() || std::any_of(haystack.begin(), haystack.end(), [&](const QString &value) {
                return containsInsensitive(value, filter);
            })) {
            filtered.push_back(supplier);
        }
    }

    fillSuppliersTable(filtered);
    m_suppliersUi.statusLabel->setText(
        tr("Status: showing %1 of %2 supplier records")
            .arg(QString::number(filtered.size()), QString::number(m_allSuppliers.size())));
}

void ManagementView::applyEmployeesFilter()
{
    const QString filter = m_employeesUi.filterField->text().trimmed();
    QVector<Common::Entities::Employee> filtered;
    for (const auto &employee : m_allEmployees) {
        const QStringList haystack = {QString::fromStdString(employee.name),
                                      QString::fromStdString(employee.number),
                                      QString::fromStdString(employee.email),
                                      QString::fromStdString(employee.address)};
        if (filter.isEmpty() || std::any_of(haystack.begin(), haystack.end(), [&](const QString &value) {
                return containsInsensitive(value, filter);
            })) {
            filtered.push_back(employee);
        }
    }

    fillEmployeesTable(filtered);
    m_employeesUi.statusLabel->setText(
        tr("Status: showing %1 of %2 employee records")
            .arg(QString::number(filtered.size()), QString::number(m_allEmployees.size())));
}

void ManagementView::fillContactsTable(const QVector<Common::Entities::Contact> &contacts)
{
    m_isSyncingTable = true;
    m_contactsUi.table->clearContents();
    m_contactsUi.table->setRowCount(contacts.size());
    m_contactsUi.table->setColumnCount(5);
    m_contactsUi.table->setHorizontalHeaderLabels(
        {tr("ID"), tr("Name"), tr("Type"), tr("Email"), tr("Phone")});

    for (int row = 0; row < contacts.size(); ++row) {
        const auto &contact = contacts[row];
        const QString contactType = canonicalContactType(QString::fromStdString(contact.type));
        m_contactsUi.table->setItem(row, 0,
                                    createReadOnlyTableItem(QString::fromStdString(contact.id)));
        m_contactsUi.table->setItem(row, 1,
                                    createEditableTableItem(QString::fromStdString(contact.name)));
        m_contactsUi.table->setItem(
            row, 2, createComboTableItem(displayContactType(contact.type), contactType));
        m_contactsUi.table->setItem(row, 3,
                                    createEditableTableItem(QString::fromStdString(contact.email)));
        m_contactsUi.table->setItem(row, 4,
                                    createEditableTableItem(QString::fromStdString(contact.phone)));
    }

    m_contactsUi.table->setColumnHidden(0, true);
    refreshPersistentComboEditors(m_contactsUi.table);
    m_isSyncingTable = false;
    updateActionButtons(m_contactsUi);
}

void ManagementView::fillSuppliersTable(const QVector<Common::Entities::Supplier> &suppliers)
{
    m_isSyncingTable = true;
    m_suppliersUi.table->clearContents();
    m_suppliersUi.table->setRowCount(suppliers.size());
    m_suppliersUi.table->setColumnCount(5);
    m_suppliersUi.table->setHorizontalHeaderLabels(
        {tr("ID"), tr("Name"), tr("Number"), tr("Email"), tr("Address")});

    for (int row = 0; row < suppliers.size(); ++row) {
        const auto &supplier = suppliers[row];
        m_suppliersUi.table->setItem(row, 0,
                                     createReadOnlyTableItem(QString::fromStdString(supplier.id)));
        m_suppliersUi.table->setItem(row, 1,
                                     createEditableTableItem(QString::fromStdString(supplier.name)));
        m_suppliersUi.table->setItem(
            row, 2, createEditableTableItem(QString::fromStdString(supplier.number)));
        m_suppliersUi.table->setItem(row, 3,
                                     createEditableTableItem(QString::fromStdString(supplier.email)));
        m_suppliersUi.table->setItem(
            row, 4, createEditableTableItem(QString::fromStdString(supplier.address)));

    }

    m_suppliersUi.table->setColumnHidden(0, true);
    m_isSyncingTable = false;
    updateActionButtons(m_suppliersUi);
}

void ManagementView::fillEmployeesTable(const QVector<Common::Entities::Employee> &employees)
{
    m_isSyncingTable = true;
    m_employeesUi.table->clearContents();
    m_employeesUi.table->setRowCount(employees.size());
    m_employeesUi.table->setColumnCount(5);
    m_employeesUi.table->setHorizontalHeaderLabels(
        {tr("ID"), tr("Name"), tr("Number"), tr("Email"), tr("Address")});

    for (int row = 0; row < employees.size(); ++row) {
        const auto &employee = employees[row];
        m_employeesUi.table->setItem(row, 0,
                                     createReadOnlyTableItem(QString::fromStdString(employee.id)));
        m_employeesUi.table->setItem(row, 1,
                                     createEditableTableItem(QString::fromStdString(employee.name)));
        m_employeesUi.table->setItem(
            row, 2, createEditableTableItem(QString::fromStdString(employee.number)));
        m_employeesUi.table->setItem(row, 3,
                                     createEditableTableItem(QString::fromStdString(employee.email)));
        m_employeesUi.table->setItem(
            row, 4, createEditableTableItem(QString::fromStdString(employee.address)));

    }

    m_employeesUi.table->setColumnHidden(0, true);
    m_isSyncingTable = false;
    updateActionButtons(m_employeesUi);
}

void ManagementView::onContactItemChanged(QTableWidgetItem *item)
{
    if (m_isSyncingTable || item == nullptr) {
        return;
    }

    const QString currentValue = item->text().trimmed();

    if (item->column() == 1 && currentValue.isEmpty()) {
        emit errorOccurred(tr("Name is required."));
        restoreItemText(m_contactsUi.table, item, item->data(Qt::UserRole).toString());
        return;
    }

    if (item->column() == 1 || item->column() == 3 || item->column() == 4) {
        if (restoreIfUnchanged(m_contactsUi.table, item, currentValue)) {
            return;
        }
        persistContactRow(item->row());
    }
}

void ManagementView::onSupplierItemChanged(QTableWidgetItem *item)
{
    if (m_isSyncingTable || item == nullptr) {
        return;
    }

    const QString currentValue = item->text().trimmed();

    if (item->column() == 1 && currentValue.isEmpty()) {
        emit errorOccurred(tr("Name is required."));
        restoreItemText(m_suppliersUi.table, item, item->data(Qt::UserRole).toString());
        return;
    }

    if (item->column() >= 1 && item->column() <= 4) {
        if (restoreIfUnchanged(m_suppliersUi.table, item, currentValue)) {
            return;
        }
        persistSupplierRow(item->row());
    }
}

void ManagementView::onEmployeeItemChanged(QTableWidgetItem *item)
{
    if (m_isSyncingTable || item == nullptr) {
        return;
    }

    const QString currentValue = item->text().trimmed();

    if (item->column() == 1 && currentValue.isEmpty()) {
        emit errorOccurred(tr("Name is required."));
        restoreItemText(m_employeesUi.table, item, item->data(Qt::UserRole).toString());
        return;
    }

    if (item->column() >= 1 && item->column() <= 4) {
        if (restoreIfUnchanged(m_employeesUi.table, item, currentValue)) {
            return;
        }
        persistEmployeeRow(item->row());
    }
}

void ManagementView::onContactAdd()
{
    Common::Entities::Contact contact;
    contact.name = tr("New customer %1").arg(defaultEntitySuffix()).toStdString();
    contact.type = "Customer";
    m_viewModel.createContact(contact);
}

void ManagementView::onContactDelete()
{
    const auto selected = m_contactsUi.table->selectedItems();
    if (!selected.isEmpty()) {
        m_viewModel.deleteContact(m_contactsUi.table->item(selected.first()->row(), 0)->text());
    }
}

void ManagementView::onSupplierAdd()
{
    Common::Entities::Supplier supplier;
    supplier.name = tr("New supplier %1").arg(defaultEntitySuffix()).toStdString();
    m_viewModel.createSupplier(supplier);
}

void ManagementView::onSupplierDelete()
{
    const auto selected = m_suppliersUi.table->selectedItems();
    if (!selected.isEmpty()) {
        m_viewModel.deleteSupplier(m_suppliersUi.table->item(selected.first()->row(), 0)->text());
    }
}

void ManagementView::onEmployeeAdd()
{
    Common::Entities::Employee employee;
    employee.name = tr("New employee %1").arg(defaultEntitySuffix()).toStdString();
    m_viewModel.createEmployee(employee);
}

void ManagementView::onEmployeeDelete()
{
    const auto selected = m_employeesUi.table->selectedItems();
    if (!selected.isEmpty()) {
        m_viewModel.deleteEmployee(m_employeesUi.table->item(selected.first()->row(), 0)->text());
    }
}

void ManagementView::persistContactRow(int row)
{
    auto *idItem = m_contactsUi.table->item(row, 0);
    auto *nameItem = m_contactsUi.table->item(row, 1);
    auto *typeItem = m_contactsUi.table->item(row, 2);
    auto *emailItem = m_contactsUi.table->item(row, 3);
    auto *phoneItem = m_contactsUi.table->item(row, 4);
    if (idItem == nullptr || nameItem == nullptr || typeItem == nullptr || emailItem == nullptr ||
        phoneItem == nullptr) {
        return;
    }

    const QString name = nameItem->text().trimmed();
    if (name.isEmpty()) {
        emit errorOccurred(tr("Name is required."));
        restoreItemText(m_contactsUi.table, nameItem, nameItem->data(Qt::UserRole).toString());
        return;
    }

    Common::Entities::Contact contact{.id = idItem->text().toStdString(),
                                      .name = name.toStdString(),
                                      .type = typeItem->data(Qt::UserRole).toString().toStdString(),
                                      .email = emailItem->text().trimmed().toStdString(),
                                      .phone = phoneItem->text().trimmed().toStdString()};
    {
        const QSignalBlocker blocker(m_contactsUi.table);
        nameItem->setData(Qt::UserRole, name);
        emailItem->setData(Qt::UserRole, emailItem->text().trimmed());
        phoneItem->setData(Qt::UserRole, phoneItem->text().trimmed());
    }
    m_viewModel.editContact(contact);
}

void ManagementView::persistSupplierRow(int row)
{
    auto *idItem = m_suppliersUi.table->item(row, 0);
    auto *nameItem = m_suppliersUi.table->item(row, 1);
    auto *numberItem = m_suppliersUi.table->item(row, 2);
    auto *emailItem = m_suppliersUi.table->item(row, 3);
    auto *addressItem = m_suppliersUi.table->item(row, 4);
    if (idItem == nullptr || nameItem == nullptr || numberItem == nullptr ||
        emailItem == nullptr || addressItem == nullptr) {
        return;
    }

    const QString name = nameItem->text().trimmed();
    if (name.isEmpty()) {
        emit errorOccurred(tr("Name is required."));
        restoreItemText(m_suppliersUi.table, nameItem, nameItem->data(Qt::UserRole).toString());
        return;
    }

    Common::Entities::Supplier supplier{.id = idItem->text().toStdString(),
                                        .name = name.toStdString(),
                                        .number = numberItem->text().trimmed().toStdString(),
                                        .email = emailItem->text().trimmed().toStdString(),
                                        .address = addressItem->text().trimmed().toStdString()};
    {
        const QSignalBlocker blocker(m_suppliersUi.table);
        nameItem->setData(Qt::UserRole, name);
        numberItem->setData(Qt::UserRole, numberItem->text().trimmed());
        emailItem->setData(Qt::UserRole, emailItem->text().trimmed());
        addressItem->setData(Qt::UserRole, addressItem->text().trimmed());
    }
    m_viewModel.editSupplier(supplier);
}

void ManagementView::persistEmployeeRow(int row)
{
    auto *idItem = m_employeesUi.table->item(row, 0);
    auto *nameItem = m_employeesUi.table->item(row, 1);
    auto *numberItem = m_employeesUi.table->item(row, 2);
    auto *emailItem = m_employeesUi.table->item(row, 3);
    auto *addressItem = m_employeesUi.table->item(row, 4);
    if (idItem == nullptr || nameItem == nullptr || numberItem == nullptr ||
        emailItem == nullptr || addressItem == nullptr) {
        return;
    }

    const QString name = nameItem->text().trimmed();
    if (name.isEmpty()) {
        emit errorOccurred(tr("Name is required."));
        restoreItemText(m_employeesUi.table, nameItem, nameItem->data(Qt::UserRole).toString());
        return;
    }

    Common::Entities::Employee employee{.id = idItem->text().toStdString(),
                                        .name = name.toStdString(),
                                        .number = numberItem->text().trimmed().toStdString(),
                                        .email = emailItem->text().trimmed().toStdString(),
                                        .address = addressItem->text().trimmed().toStdString()};
    {
        const QSignalBlocker blocker(m_employeesUi.table);
        nameItem->setData(Qt::UserRole, name);
        numberItem->setData(Qt::UserRole, numberItem->text().trimmed());
        emailItem->setData(Qt::UserRole, emailItem->text().trimmed());
        addressItem->setData(Qt::UserRole, addressItem->text().trimmed());
    }
    m_viewModel.editEmployee(employee);
}
