#pragma once

#include <QVector>

#include "Ui/Models/BaseModel.hpp"

#include "common/Entities/Contact.hpp"
#include "common/Entities/Employee.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Entities/SaleOrder.hpp"
#include "common/Entities/SalesOrderRecord.hpp"
#include "common/Entities/User.hpp"

class ApiManager;

class SalesModel : public BaseModel {
    Q_OBJECT

public:
    explicit SalesModel(ApiManager &apiManager, QObject *parent = nullptr);

    QVector<Common::Entities::SaleOrder> orders() const;
    QVector<Common::Entities::SalesOrderRecord> orderRecords() const;
    QVector<Common::Entities::User> users() const;
    QVector<Common::Entities::Contact> contacts() const;
    QVector<Common::Entities::Employee> employees() const;
    QVector<Common::Entities::ProductType> productTypes() const;

signals:
    void ordersChanged();
    void orderRecordsChanged();
    void usersChanged();
    void contactsChanged();
    void employeesChanged();
    void productTypesChanged();

public slots:
    void fetchAll();
    void fetchOrders();
    void fetchOrderRecords(const QString &orderId);
    void fetchReferenceData();
    void createOrder(const Common::Entities::SaleOrder &order);
    void editOrder(const Common::Entities::SaleOrder &order);
    void deleteOrder(const QString &id);
    void createOrderRecord(const Common::Entities::SalesOrderRecord &record);
    void editOrderRecord(const Common::Entities::SalesOrderRecord &record);
    void deleteOrderRecord(const QString &id);

private slots:
    void onOrdersList(const std::vector<Common::Entities::SaleOrder> &orders);
    void onOrderRecordsList(const std::vector<Common::Entities::SalesOrderRecord> &records);
    void onUsersList(const std::vector<Common::Entities::User> &users);
    void onContactsList(const std::vector<Common::Entities::Contact> &contacts);
    void onEmployeesList(const std::vector<Common::Entities::Employee> &employees);
    void onProductTypesList(const std::vector<Common::Entities::ProductType> &productTypes);
    void onOrderCreated();
    void onOrderEdited();
    void onOrderDeleted();
    void onOrderRecordCreated();
    void onOrderRecordEdited();
    void onOrderRecordDeleted();

private:
    void refreshCurrentRecords();

    ApiManager &m_apiManager;
    QString m_lastOrderId;
    QVector<Common::Entities::SaleOrder> m_orders;
    QVector<Common::Entities::SalesOrderRecord> m_orderRecords;
    QVector<Common::Entities::User> m_users;
    QVector<Common::Entities::Contact> m_contacts;
    QVector<Common::Entities::Employee> m_employees;
    QVector<Common::Entities::ProductType> m_productTypes;
};
