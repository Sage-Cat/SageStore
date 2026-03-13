#pragma once

#include <QVector>

#include "Ui/ViewModels/BaseViewModel.hpp"

#include "common/Entities/Employee.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"
#include "common/Entities/Supplier.hpp"
#include "common/Entities/User.hpp"

class PurchaseModel;

class PurchaseViewModel : public BaseViewModel {
    Q_OBJECT

public:
    explicit PurchaseViewModel(PurchaseModel &model, QObject *parent = nullptr);

    QVector<Common::Entities::PurchaseOrder> orders() const;
    QVector<Common::Entities::PurchaseOrderRecord> orderRecords() const;
    QVector<Common::Entities::User> users() const;
    QVector<Common::Entities::Supplier> suppliers() const;
    QVector<Common::Entities::Employee> employees() const;
    QVector<Common::Entities::ProductType> productTypes() const;

    QString userLabel(const QString &userId) const;
    QString supplierLabel(const QString &supplierId) const;
    QString employeeLabel(const QString &employeeId) const;
    QString productTypeLabel(const QString &productTypeId) const;

signals:
    void ordersChanged();
    void orderRecordsChanged();
    void referenceDataChanged();
    void purchaseReceiptPosted();
    void errorOccurred(const QString &errorMessage);

public slots:
    void fetchAll();
    void fetchOrders();
    void fetchOrderRecords(const QString &orderId);
    void createOrder(const Common::Entities::PurchaseOrder &order);
    void editOrder(const Common::Entities::PurchaseOrder &order);
    void deleteOrder(const QString &id);
    void createOrderRecord(const Common::Entities::PurchaseOrderRecord &record);
    void editOrderRecord(const Common::Entities::PurchaseOrderRecord &record);
    void deleteOrderRecord(const QString &id);
    void postReceipt(const QString &orderId, const QString &employeeId);

private:
    PurchaseModel &m_model;
};
