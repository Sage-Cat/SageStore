#include "Ui/ViewModels/ManagementViewModel.hpp"

#include "Ui/Models/ManagementModel.hpp"

ManagementViewModel::ManagementViewModel(ManagementModel &model, QObject *parent)
    : BaseViewModel(model, parent), m_model(model)
{
    connect(&m_model, &ManagementModel::contactsChanged, this, &ManagementViewModel::contactsChanged);
    connect(&m_model, &ManagementModel::suppliersChanged, this,
            &ManagementViewModel::suppliersChanged);
    connect(&m_model, &ManagementModel::employeesChanged, this,
            &ManagementViewModel::employeesChanged);
    connect(&m_model, &ManagementModel::errorOccurred, this,
            &ManagementViewModel::errorOccurred);
}

QVector<Common::Entities::Contact> ManagementViewModel::contacts() const
{
    return m_model.contacts();
}

QVector<Common::Entities::Supplier> ManagementViewModel::suppliers() const
{
    return m_model.suppliers();
}

QVector<Common::Entities::Employee> ManagementViewModel::employees() const
{
    return m_model.employees();
}

void ManagementViewModel::fetchAll()
{
    m_model.fetchContacts();
    m_model.fetchSuppliers();
    m_model.fetchEmployees();
}

void ManagementViewModel::fetchContacts() { m_model.fetchContacts(); }

void ManagementViewModel::createContact(const Common::Entities::Contact &contact)
{
    m_model.createContact(contact);
}

void ManagementViewModel::editContact(const Common::Entities::Contact &contact)
{
    m_model.editContact(contact);
}

void ManagementViewModel::deleteContact(const QString &id) { m_model.deleteContact(id); }

void ManagementViewModel::fetchSuppliers() { m_model.fetchSuppliers(); }

void ManagementViewModel::createSupplier(const Common::Entities::Supplier &supplier)
{
    m_model.createSupplier(supplier);
}

void ManagementViewModel::editSupplier(const Common::Entities::Supplier &supplier)
{
    m_model.editSupplier(supplier);
}

void ManagementViewModel::deleteSupplier(const QString &id) { m_model.deleteSupplier(id); }

void ManagementViewModel::fetchEmployees() { m_model.fetchEmployees(); }

void ManagementViewModel::createEmployee(const Common::Entities::Employee &employee)
{
    m_model.createEmployee(employee);
}

void ManagementViewModel::editEmployee(const Common::Entities::Employee &employee)
{
    m_model.editEmployee(employee);
}

void ManagementViewModel::deleteEmployee(const QString &id) { m_model.deleteEmployee(id); }
