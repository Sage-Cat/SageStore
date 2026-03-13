#pragma once

#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QVector>
#include <QWidget>

#include "common/Entities/Contact.hpp"
#include "common/Entities/Employee.hpp"
#include "common/Entities/Supplier.hpp"

class ManagementViewModel;

class ManagementView : public QWidget {
    Q_OBJECT

public:
    enum class Section { Contacts, Suppliers, Employees };

    explicit ManagementView(ManagementViewModel &viewModel, QWidget *parent = nullptr);

    void showSection(Section section);

signals:
    void errorOccurred(const QString &message);

private slots:
    void onContactsChanged();
    void onSuppliersChanged();
    void onEmployeesChanged();

    void onContactAdd();
    void onContactEdit();
    void onContactDelete();
    void onSupplierAdd();
    void onSupplierEdit();
    void onSupplierDelete();
    void onEmployeeAdd();
    void onEmployeeEdit();
    void onEmployeeDelete();

    void refreshContacts();
    void refreshSuppliers();
    void refreshEmployees();

private:
    struct CrudUi {
        QWidget *page{nullptr};
        QLineEdit *filterField{nullptr};
        QPushButton *addButton{nullptr};
        QPushButton *editButton{nullptr};
        QPushButton *deleteButton{nullptr};
        QTableWidget *table{nullptr};
        QLabel *statusLabel{nullptr};
    };

    CrudUi createCrudSection(const QString &objectPrefix, const QString &placeholder) const;
    void setupUi();
    void connectSignals();
    void styleSection(CrudUi &ui) const;
    void updateActionButtons(CrudUi &ui) const;

    void applyContactsFilter();
    void applySuppliersFilter();
    void applyEmployeesFilter();
    void fillContactsTable(const QVector<Common::Entities::Contact> &contacts);
    void fillSuppliersTable(const QVector<Common::Entities::Supplier> &suppliers);
    void fillEmployeesTable(const QVector<Common::Entities::Employee> &employees);

    bool showContactDialog(Common::Entities::Contact &contact, const QString &title);
    bool showSupplierDialog(Common::Entities::Supplier &supplier, const QString &title);
    bool showEmployeeDialog(Common::Entities::Employee &employee, const QString &title);

    ManagementViewModel &m_viewModel;
    QTabWidget *m_sections{nullptr};
    CrudUi m_contactsUi;
    CrudUi m_suppliersUi;
    CrudUi m_employeesUi;
    QVector<Common::Entities::Contact> m_allContacts;
    QVector<Common::Entities::Supplier> m_allSuppliers;
    QVector<Common::Entities::Employee> m_allEmployees;
};
