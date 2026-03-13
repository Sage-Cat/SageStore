#pragma once

#include <QVector>

#include "Ui/Models/BaseModel.hpp"

#include "common/Entities/Employee.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"
#include "common/Entities/Supplier.hpp"
#include "common/Entities/User.hpp"

class ApiManager;

class PurchaseModel : public BaseModel {
    Q_OBJECT

public:
    explicit PurchaseModel(ApiManager &apiManager, QObject *parent = nullptr);

    QVector<Common::Entities::PurchaseOrder> orders() const;
    QVector<Common::Entities::PurchaseOrderRecord> orderRecords() const;
    QVector<Common::Entities::User> users() const;
    QVector<Common::Entities::Supplier> suppliers() const;
    QVector<Common::Entities::Employee> employees() const;
    QVector<Common::Entities::ProductType> productTypes() const;

signals:
    void ordersChanged();
    void orderRecordsChanged();
    void usersChanged();
    void suppliersChanged();
    void employeesChanged();
    void productTypesChanged();
    void purchaseReceiptPosted();

public slots:
    void fetchAll();
    void fetchOrders();
    void fetchOrderRecords(const QString &orderId);
    void fetchReferenceData();
    void createOrder(const Common::Entities::PurchaseOrder &order);
    void editOrder(const Common::Entities::PurchaseOrder &order);
    void deleteOrder(const QString &id);
    void createOrderRecord(const Common::Entities::PurchaseOrderRecord &record);
    void editOrderRecord(const Common::Entities::PurchaseOrderRecord &record);
    void deleteOrderRecord(const QString &id);
    void postReceipt(const QString &orderId, const QString &employeeId);

private slots:
    void onOrdersList(const std::vector<Common::Entities::PurchaseOrder> &orders);
    void onOrderRecordsList(const std::vector<Common::Entities::PurchaseOrderRecord> &records);
    void onUsersList(const std::vector<Common::Entities::User> &users);
    void onSuppliersList(const std::vector<Common::Entities::Supplier> &suppliers);
    void onEmployeesList(const std::vector<Common::Entities::Employee> &employees);
    void onProductTypesList(const std::vector<Common::Entities::ProductType> &productTypes);
    void onOrderCreated();
    void onOrderEdited();
    void onOrderDeleted();
    void onOrderRecordCreated();
    void onOrderRecordEdited();
    void onOrderRecordDeleted();
    void onReceiptPosted();

private:
    void refreshCurrentRecords();

    ApiManager &m_apiManager;
    QString m_lastOrderId;
    QVector<Common::Entities::PurchaseOrder> m_orders;
    QVector<Common::Entities::PurchaseOrderRecord> m_orderRecords;
    QVector<Common::Entities::User> m_users;
    QVector<Common::Entities::Supplier> m_suppliers;
    QVector<Common::Entities::Employee> m_employees;
    QVector<Common::Entities::ProductType> m_productTypes;
};
