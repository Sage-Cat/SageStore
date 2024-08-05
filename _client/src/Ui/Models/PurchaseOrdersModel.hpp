#pragma once

#include "BaseModel.hpp"

#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"

#include <QObject>
#include <QVector>

class ApiManager;

class PurchaseOrdersModel : public BaseModel {
    Q_OBJECT
public:
    explicit PurchaseOrdersModel(ApiManager &apiManager, QObject *parent = nullptr);

    QVector<Common::Entities::PurchaseOrder> orders() const;
    QVector<Common::Entities::PurchaseOrderRecord> ordersRecord() const;

signals:
    // Orders
    void ordersChanged();
    void orderAdded();
    void orderEdited();
    void orderDeleted();

    // Order Records
    void orderRecordsChanged();
    void orderRecordCreated();
    void orderRecordEdited();
    void orderRecordDeleted();

public slots:
    // Orders
    void fetchOrders();
    void addOrder(const Common::Entities::PurchaseOrder &purchaseOrder);
    void editOrder(const Common::Entities::PurchaseOrder &purchaseOrder);
    void deleteOrder(const QString &id);

    // Order Records
    void fetchOrderRecords();
    void createOrderRecord(const Common::Entities::PurchaseOrderRecord &purchaseOrderRecord);
    void editOrderRecord(const Common::Entities::PurchaseOrderRecord &purchaseOrderRecord);
    void deleteOrderRecord(const QString &id);

private slots: // -- ApiManager signal handlers ---
    void onPurchaseOrderAdded();
    void onPurchaseOrderUpdated();
    void onPurchaseOrderDeleted();
    void onPurchaseOrdersList(const std::vector<Common::Entities::PurchaseOrder> &orders);

    void onPurchaseOrderRecordCreated();
    void onPurchaseOrderRecordEdited();
    void onPurchaseOrderRecordDeleted();
    void onPurchaseOrderRecordsList(const std::vector<Common::Entities::PurchaseOrderRecord> &ordersRecord);

private:
    ApiManager &m_apiManager;

    QVector<Common::Entities::PurchaseOrder> m_orders;
    QVector<Common::Entities::PurchaseOrderRecord> m_ordersRecord;
};