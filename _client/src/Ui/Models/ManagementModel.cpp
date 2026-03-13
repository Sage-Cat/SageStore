#include "Ui/Models/ManagementModel.hpp"

#include "Network/ApiManager.hpp"

ManagementModel::ManagementModel(ApiManager &apiManager, QObject *parent)
    : BaseModel(parent), m_apiManager(apiManager)
{
    connect(&m_apiManager, &ApiManager::errorOccurred, this, &ManagementModel::errorOccurred);

    connect(&m_apiManager, &ApiManager::contactsList, this, &ManagementModel::onContactsList);
    connect(&m_apiManager, &ApiManager::contactCreated, this, &ManagementModel::onContactCreated);
    connect(&m_apiManager, &ApiManager::contactEdited, this, &ManagementModel::onContactEdited);
    connect(&m_apiManager, &ApiManager::contactDeleted, this, &ManagementModel::onContactDeleted);

    connect(&m_apiManager, &ApiManager::suppliersList, this, &ManagementModel::onSuppliersList);
    connect(&m_apiManager, &ApiManager::supplierCreated, this,
            &ManagementModel::onSupplierCreated);
    connect(&m_apiManager, &ApiManager::supplierEdited, this, &ManagementModel::onSupplierEdited);
    connect(&m_apiManager, &ApiManager::supplierDeleted, this,
            &ManagementModel::onSupplierDeleted);

    connect(&m_apiManager, &ApiManager::employeesList, this, &ManagementModel::onEmployeesList);
    connect(&m_apiManager, &ApiManager::employeeCreated, this,
            &ManagementModel::onEmployeeCreated);
    connect(&m_apiManager, &ApiManager::employeeEdited, this, &ManagementModel::onEmployeeEdited);
    connect(&m_apiManager, &ApiManager::employeeDeleted, this,
            &ManagementModel::onEmployeeDeleted);
}

QVector<Common::Entities::Contact> ManagementModel::contacts() const { return m_contacts; }

QVector<Common::Entities::Supplier> ManagementModel::suppliers() const { return m_suppliers; }

QVector<Common::Entities::Employee> ManagementModel::employees() const { return m_employees; }

void ManagementModel::fetchContacts() { m_apiManager.getContacts(); }

void ManagementModel::createContact(const Common::Entities::Contact &contact)
{
    m_apiManager.createContact(contact);
}

void ManagementModel::editContact(const Common::Entities::Contact &contact)
{
    m_apiManager.editContact(contact);
}

void ManagementModel::deleteContact(const QString &id) { m_apiManager.deleteContact(id); }

void ManagementModel::fetchSuppliers() { m_apiManager.getSuppliers(); }

void ManagementModel::createSupplier(const Common::Entities::Supplier &supplier)
{
    m_apiManager.createSupplier(supplier);
}

void ManagementModel::editSupplier(const Common::Entities::Supplier &supplier)
{
    m_apiManager.editSupplier(supplier);
}

void ManagementModel::deleteSupplier(const QString &id) { m_apiManager.deleteSupplier(id); }

void ManagementModel::fetchEmployees() { m_apiManager.getEmployees(); }

void ManagementModel::createEmployee(const Common::Entities::Employee &employee)
{
    m_apiManager.createEmployee(employee);
}

void ManagementModel::editEmployee(const Common::Entities::Employee &employee)
{
    m_apiManager.editEmployee(employee);
}

void ManagementModel::deleteEmployee(const QString &id) { m_apiManager.deleteEmployee(id); }

void ManagementModel::onContactsList(const std::vector<Common::Entities::Contact> &contacts)
{
    m_contacts = QVector<Common::Entities::Contact>(contacts.begin(), contacts.end());
    emit contactsChanged();
}

void ManagementModel::onSuppliersList(const std::vector<Common::Entities::Supplier> &suppliers)
{
    m_suppliers = QVector<Common::Entities::Supplier>(suppliers.begin(), suppliers.end());
    emit suppliersChanged();
}

void ManagementModel::onEmployeesList(const std::vector<Common::Entities::Employee> &employees)
{
    m_employees = QVector<Common::Entities::Employee>(employees.begin(), employees.end());
    emit employeesChanged();
}

void ManagementModel::onContactCreated()
{
    fetchContacts();
    emit contactCreated();
}

void ManagementModel::onContactEdited()
{
    fetchContacts();
    emit contactEdited();
}

void ManagementModel::onContactDeleted()
{
    fetchContacts();
    emit contactDeleted();
}

void ManagementModel::onSupplierCreated()
{
    fetchSuppliers();
    emit supplierCreated();
}

void ManagementModel::onSupplierEdited()
{
    fetchSuppliers();
    emit supplierEdited();
}

void ManagementModel::onSupplierDeleted()
{
    fetchSuppliers();
    emit supplierDeleted();
}

void ManagementModel::onEmployeeCreated()
{
    fetchEmployees();
    emit employeeCreated();
}

void ManagementModel::onEmployeeEdited()
{
    fetchEmployees();
    emit employeeEdited();
}

void ManagementModel::onEmployeeDeleted()
{
    fetchEmployees();
    emit employeeDeleted();
}
