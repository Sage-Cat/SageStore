#pragma once

#include <QVector>

#include "Ui/Models/BaseModel.hpp"

#include "common/Entities/Contact.hpp"
#include "common/Entities/Employee.hpp"
#include "common/Entities/Supplier.hpp"

class ApiManager;

class ManagementModel : public BaseModel {
    Q_OBJECT

public:
    explicit ManagementModel(ApiManager &apiManager, QObject *parent = nullptr);

    QVector<Common::Entities::Contact> contacts() const;
    QVector<Common::Entities::Supplier> suppliers() const;
    QVector<Common::Entities::Employee> employees() const;

signals:
    void contactsChanged();
    void suppliersChanged();
    void employeesChanged();
    void contactCreated();
    void contactEdited();
    void contactDeleted();
    void supplierCreated();
    void supplierEdited();
    void supplierDeleted();
    void employeeCreated();
    void employeeEdited();
    void employeeDeleted();

public slots:
    void fetchContacts();
    void createContact(const Common::Entities::Contact &contact);
    void editContact(const Common::Entities::Contact &contact);
    void deleteContact(const QString &id);

    void fetchSuppliers();
    void createSupplier(const Common::Entities::Supplier &supplier);
    void editSupplier(const Common::Entities::Supplier &supplier);
    void deleteSupplier(const QString &id);

    void fetchEmployees();
    void createEmployee(const Common::Entities::Employee &employee);
    void editEmployee(const Common::Entities::Employee &employee);
    void deleteEmployee(const QString &id);

private slots:
    void onContactsList(const std::vector<Common::Entities::Contact> &contacts);
    void onSuppliersList(const std::vector<Common::Entities::Supplier> &suppliers);
    void onEmployeesList(const std::vector<Common::Entities::Employee> &employees);
    void onContactCreated();
    void onContactEdited();
    void onContactDeleted();
    void onSupplierCreated();
    void onSupplierEdited();
    void onSupplierDeleted();
    void onEmployeeCreated();
    void onEmployeeEdited();
    void onEmployeeDeleted();

private:
    ApiManager &m_apiManager;
    QVector<Common::Entities::Contact> m_contacts;
    QVector<Common::Entities::Supplier> m_suppliers;
    QVector<Common::Entities::Employee> m_employees;
};
