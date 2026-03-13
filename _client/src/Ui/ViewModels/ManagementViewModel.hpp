#pragma once

#include <QVector>

#include "Ui/ViewModels/BaseViewModel.hpp"

#include "common/Entities/Contact.hpp"
#include "common/Entities/Employee.hpp"
#include "common/Entities/Supplier.hpp"

class ManagementModel;

class ManagementViewModel : public BaseViewModel {
    Q_OBJECT

public:
    explicit ManagementViewModel(ManagementModel &model, QObject *parent = nullptr);

    QVector<Common::Entities::Contact> contacts() const;
    QVector<Common::Entities::Supplier> suppliers() const;
    QVector<Common::Entities::Employee> employees() const;

signals:
    void contactsChanged();
    void suppliersChanged();
    void employeesChanged();
    void errorOccurred(const QString &errorMessage);

public slots:
    void fetchAll();
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

private:
    ManagementModel &m_model;
};
