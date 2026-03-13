#include "Ui/Views/ManagementView.hpp"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
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
} // namespace

ManagementView::ManagementView(ManagementViewModel &viewModel, QWidget *parent)
    : QWidget(parent), m_viewModel(viewModel)
{
    setupUi();
    connectSignals();
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
    ui.editButton = new QPushButton(tr("Edit"), ui.page);
    ui.editButton->setObjectName(objectPrefix + "EditButton");
    ui.deleteButton = new QPushButton(tr("Delete"), ui.page);
    ui.deleteButton->setObjectName(objectPrefix + "DeleteButton");

    filterRow->addWidget(new QLabel(tr("Filter"), ui.page));
    filterRow->addWidget(ui.filterField);

    buttonsRow->addWidget(ui.addButton);
    buttonsRow->addWidget(ui.editButton);
    buttonsRow->addWidget(ui.deleteButton);
    buttonsRow->addStretch();

    ui.table = new QTableWidget(ui.page);
    ui.table->setObjectName(objectPrefix + "Table");
    ui.table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.table->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.table->setAlternatingRowColors(true);
    ui.table->setSortingEnabled(true);
    ui.table->verticalHeader()->setVisible(false);
    ui.table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui.statusLabel = new QLabel(tr("Status: ready"), ui.page);
    ui.statusLabel->setObjectName(objectPrefix + "StatusLabel");

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

    titleLabel->setObjectName("managementTitleLabel");
    summaryLabel->setObjectName("managementSummaryLabel");
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
        "#managementView { background-color: #fffaf2; }"
        "QFrame { background-color: #f6efe4; border: 1px solid #d8cdbd; border-radius: 12px; }"
        "#managementTitleLabel { color: #233130; font-size: 24px; font-weight: 700; }"
        "#managementSummaryLabel { color: #4a544d; }"
        "QLineEdit, QComboBox { background-color: white; border: 1px solid #cbd5e1; border-radius: 6px; padding: 6px; }"
        "QPushButton { background-color: #1d4ed8; color: white; border: 0; border-radius: 6px; padding: 6px 12px; }"
        "QPushButton:hover { background-color: #1e40af; }"
        "QPushButton[destructive='true'] { background-color: #b91c1c; }"
        "QPushButton[destructive='true']:hover { background-color: #991b1b; }"
        "QTableWidget { background-color: #fbfbfd; alternate-background-color: #f1f4f8; border: 1px solid #d7dde7; border-radius: 8px; }"
        "QHeaderView::section { background-color: #e8eef6; color: #0f172a; padding: 6px; border: 0; border-bottom: 1px solid #d7dde7; font-weight: 600; }");
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
    connect(m_contactsUi.editButton, &QPushButton::clicked, this, &ManagementView::onContactEdit);
    connect(m_contactsUi.deleteButton, &QPushButton::clicked, this,
            &ManagementView::onContactDelete);
    connect(m_contactsUi.filterField, &QLineEdit::textChanged, this,
            &ManagementView::refreshContacts);
    connect(m_contactsUi.table->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &ManagementView::refreshContacts);

    connect(m_suppliersUi.addButton, &QPushButton::clicked, this, &ManagementView::onSupplierAdd);
    connect(m_suppliersUi.editButton, &QPushButton::clicked, this,
            &ManagementView::onSupplierEdit);
    connect(m_suppliersUi.deleteButton, &QPushButton::clicked, this,
            &ManagementView::onSupplierDelete);
    connect(m_suppliersUi.filterField, &QLineEdit::textChanged, this,
            &ManagementView::refreshSuppliers);
    connect(m_suppliersUi.table->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &ManagementView::refreshSuppliers);

    connect(m_employeesUi.addButton, &QPushButton::clicked, this, &ManagementView::onEmployeeAdd);
    connect(m_employeesUi.editButton, &QPushButton::clicked, this,
            &ManagementView::onEmployeeEdit);
    connect(m_employeesUi.deleteButton, &QPushButton::clicked, this,
            &ManagementView::onEmployeeDelete);
    connect(m_employeesUi.filterField, &QLineEdit::textChanged, this,
            &ManagementView::refreshEmployees);
    connect(m_employeesUi.table->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &ManagementView::refreshEmployees);
}

void ManagementView::styleSection(CrudUi &ui) const
{
    ui.deleteButton->setProperty("destructive", true);
    updateActionButtons(ui);
}

void ManagementView::updateActionButtons(CrudUi &ui) const
{
    const bool hasSelection = !ui.table->selectedItems().isEmpty();
    ui.editButton->setEnabled(hasSelection);
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
        const QStringList haystack = {QString::fromStdString(contact.name),
                                      QString::fromStdString(contact.type),
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
    m_contactsUi.table->setSortingEnabled(false);
    m_contactsUi.table->clearContents();
    m_contactsUi.table->setRowCount(contacts.size());
    m_contactsUi.table->setColumnCount(5);
    m_contactsUi.table->setHorizontalHeaderLabels(
        {tr("ID"), tr("Name"), tr("Type"), tr("Email"), tr("Phone")});

    for (int row = 0; row < contacts.size(); ++row) {
        const auto &contact = contacts[row];
        m_contactsUi.table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(contact.id)));
        m_contactsUi.table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(contact.name)));
        m_contactsUi.table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(contact.type)));
        m_contactsUi.table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(contact.email)));
        m_contactsUi.table->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(contact.phone)));
    }

    m_contactsUi.table->setColumnHidden(0, true);
    m_contactsUi.table->setSortingEnabled(true);
    updateActionButtons(m_contactsUi);
}

void ManagementView::fillSuppliersTable(const QVector<Common::Entities::Supplier> &suppliers)
{
    m_suppliersUi.table->setSortingEnabled(false);
    m_suppliersUi.table->clearContents();
    m_suppliersUi.table->setRowCount(suppliers.size());
    m_suppliersUi.table->setColumnCount(5);
    m_suppliersUi.table->setHorizontalHeaderLabels(
        {tr("ID"), tr("Name"), tr("Number"), tr("Email"), tr("Address")});

    for (int row = 0; row < suppliers.size(); ++row) {
        const auto &supplier = suppliers[row];
        m_suppliersUi.table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(supplier.id)));
        m_suppliersUi.table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(supplier.name)));
        m_suppliersUi.table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(supplier.number)));
        m_suppliersUi.table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(supplier.email)));
        m_suppliersUi.table->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(supplier.address)));
    }

    m_suppliersUi.table->setColumnHidden(0, true);
    m_suppliersUi.table->setSortingEnabled(true);
    updateActionButtons(m_suppliersUi);
}

void ManagementView::fillEmployeesTable(const QVector<Common::Entities::Employee> &employees)
{
    m_employeesUi.table->setSortingEnabled(false);
    m_employeesUi.table->clearContents();
    m_employeesUi.table->setRowCount(employees.size());
    m_employeesUi.table->setColumnCount(5);
    m_employeesUi.table->setHorizontalHeaderLabels(
        {tr("ID"), tr("Name"), tr("Number"), tr("Email"), tr("Address")});

    for (int row = 0; row < employees.size(); ++row) {
        const auto &employee = employees[row];
        m_employeesUi.table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(employee.id)));
        m_employeesUi.table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(employee.name)));
        m_employeesUi.table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(employee.number)));
        m_employeesUi.table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(employee.email)));
        m_employeesUi.table->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(employee.address)));
    }

    m_employeesUi.table->setColumnHidden(0, true);
    m_employeesUi.table->setSortingEnabled(true);
    updateActionButtons(m_employeesUi);
}

bool ManagementView::showContactDialog(Common::Entities::Contact &contact, const QString &title)
{
    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setMinimumWidth(420);

    auto *layout = new QFormLayout(&dialog);
    auto *nameField = new QLineEdit(QString::fromStdString(contact.name), &dialog);
    auto *typeBox = new QComboBox(&dialog);
    auto *emailField = new QLineEdit(QString::fromStdString(contact.email), &dialog);
    auto *phoneField = new QLineEdit(QString::fromStdString(contact.phone), &dialog);

    typeBox->setEditable(true);
    typeBox->addItems({tr("Customer"), tr("Client"), tr("Partner")});
    typeBox->setCurrentText(contact.type.empty() ? tr("Customer")
                                                 : QString::fromStdString(contact.type));

    layout->addRow(tr("Name"), nameField);
    layout->addRow(tr("Type"), typeBox);
    layout->addRow(tr("Email"), emailField);
    layout->addRow(tr("Phone"), phoneField);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, [this, &dialog, nameField]() {
        if (nameField->text().trimmed().isEmpty()) {
            emit errorOccurred(tr("Name is required."));
            return;
        }
        dialog.accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    contact.name  = nameField->text().trimmed().toStdString();
    contact.type  = typeBox->currentText().trimmed().toStdString();
    contact.email = emailField->text().trimmed().toStdString();
    contact.phone = phoneField->text().trimmed().toStdString();
    return true;
}

bool ManagementView::showSupplierDialog(Common::Entities::Supplier &supplier, const QString &title)
{
    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setMinimumWidth(440);

    auto *layout = new QFormLayout(&dialog);
    auto *nameField = new QLineEdit(QString::fromStdString(supplier.name), &dialog);
    auto *numberField = new QLineEdit(QString::fromStdString(supplier.number), &dialog);
    auto *emailField = new QLineEdit(QString::fromStdString(supplier.email), &dialog);
    auto *addressField = new QLineEdit(QString::fromStdString(supplier.address), &dialog);

    layout->addRow(tr("Name"), nameField);
    layout->addRow(tr("Number"), numberField);
    layout->addRow(tr("Email"), emailField);
    layout->addRow(tr("Address"), addressField);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, [this, &dialog, nameField]() {
        if (nameField->text().trimmed().isEmpty()) {
            emit errorOccurred(tr("Name is required."));
            return;
        }
        dialog.accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    supplier.name    = nameField->text().trimmed().toStdString();
    supplier.number  = numberField->text().trimmed().toStdString();
    supplier.email   = emailField->text().trimmed().toStdString();
    supplier.address = addressField->text().trimmed().toStdString();
    return true;
}

bool ManagementView::showEmployeeDialog(Common::Entities::Employee &employee, const QString &title)
{
    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setMinimumWidth(440);

    auto *layout = new QFormLayout(&dialog);
    auto *nameField = new QLineEdit(QString::fromStdString(employee.name), &dialog);
    auto *numberField = new QLineEdit(QString::fromStdString(employee.number), &dialog);
    auto *emailField = new QLineEdit(QString::fromStdString(employee.email), &dialog);
    auto *addressField = new QLineEdit(QString::fromStdString(employee.address), &dialog);

    layout->addRow(tr("Name"), nameField);
    layout->addRow(tr("Number"), numberField);
    layout->addRow(tr("Email"), emailField);
    layout->addRow(tr("Address"), addressField);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, [this, &dialog, nameField]() {
        if (nameField->text().trimmed().isEmpty()) {
            emit errorOccurred(tr("Name is required."));
            return;
        }
        dialog.accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    employee.name    = nameField->text().trimmed().toStdString();
    employee.number  = numberField->text().trimmed().toStdString();
    employee.email   = emailField->text().trimmed().toStdString();
    employee.address = addressField->text().trimmed().toStdString();
    return true;
}

void ManagementView::onContactAdd()
{
    Common::Entities::Contact contact;
    contact.type = "Customer";
    if (showContactDialog(contact, tr("Create customer"))) {
        m_viewModel.createContact(contact);
    }
}

void ManagementView::onContactEdit()
{
    const auto selected = m_contactsUi.table->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    const int row = selected.first()->row();
    Common::Entities::Contact contact{
        .id = m_contactsUi.table->item(row, 0)->text().toStdString(),
        .name = m_contactsUi.table->item(row, 1)->text().toStdString(),
        .type = m_contactsUi.table->item(row, 2)->text().toStdString(),
        .email = m_contactsUi.table->item(row, 3)->text().toStdString(),
        .phone = m_contactsUi.table->item(row, 4)->text().toStdString()};
    if (showContactDialog(contact, tr("Edit customer"))) {
        m_viewModel.editContact(contact);
    }
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
    if (showSupplierDialog(supplier, tr("Create supplier"))) {
        m_viewModel.createSupplier(supplier);
    }
}

void ManagementView::onSupplierEdit()
{
    const auto selected = m_suppliersUi.table->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    const int row = selected.first()->row();
    Common::Entities::Supplier supplier{
        .id = m_suppliersUi.table->item(row, 0)->text().toStdString(),
        .name = m_suppliersUi.table->item(row, 1)->text().toStdString(),
        .number = m_suppliersUi.table->item(row, 2)->text().toStdString(),
        .email = m_suppliersUi.table->item(row, 3)->text().toStdString(),
        .address = m_suppliersUi.table->item(row, 4)->text().toStdString()};
    if (showSupplierDialog(supplier, tr("Edit supplier"))) {
        m_viewModel.editSupplier(supplier);
    }
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
    if (showEmployeeDialog(employee, tr("Create employee"))) {
        m_viewModel.createEmployee(employee);
    }
}

void ManagementView::onEmployeeEdit()
{
    const auto selected = m_employeesUi.table->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    const int row = selected.first()->row();
    Common::Entities::Employee employee{
        .id = m_employeesUi.table->item(row, 0)->text().toStdString(),
        .name = m_employeesUi.table->item(row, 1)->text().toStdString(),
        .number = m_employeesUi.table->item(row, 2)->text().toStdString(),
        .email = m_employeesUi.table->item(row, 3)->text().toStdString(),
        .address = m_employeesUi.table->item(row, 4)->text().toStdString()};
    if (showEmployeeDialog(employee, tr("Edit employee"))) {
        m_viewModel.editEmployee(employee);
    }
}

void ManagementView::onEmployeeDelete()
{
    const auto selected = m_employeesUi.table->selectedItems();
    if (!selected.isEmpty()) {
        m_viewModel.deleteEmployee(m_employeesUi.table->item(selected.first()->row(), 0)->text());
    }
}
