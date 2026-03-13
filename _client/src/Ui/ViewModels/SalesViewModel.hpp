#pragma once

#include <QVector>

#include "Ui/ViewModels/BaseViewModel.hpp"

#include "common/Entities/Contact.hpp"
#include "common/Entities/Employee.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Entities/SaleOrder.hpp"
#include "common/Entities/SalesOrderRecord.hpp"
#include "common/Entities/User.hpp"

class SalesModel;

class SalesViewModel : public BaseViewModel {
    Q_OBJECT

public:
    explicit SalesViewModel(SalesModel &model, QObject *parent = nullptr);

    QVector<Common::Entities::SaleOrder> orders() const;
    QVector<Common::Entities::SalesOrderRecord> orderRecords() const;
    QVector<Common::Entities::User> users() const;
    QVector<Common::Entities::Contact> contacts() const;
    QVector<Common::Entities::Employee> employees() const;
    QVector<Common::Entities::ProductType> productTypes() const;

    QString userLabel(const QString &userId) const;
    QString contactLabel(const QString &contactId) const;
    QString employeeLabel(const QString &employeeId) const;
    QString productTypeLabel(const QString &productTypeId) const;
    QString buildInvoicePreview(const QString &orderId) const;

signals:
    void ordersChanged();
    void orderRecordsChanged();
    void referenceDataChanged();
    void errorOccurred(const QString &errorMessage);

public slots:
    void fetchAll();
    void fetchOrders();
    void fetchOrderRecords(const QString &orderId);
    void createOrder(const Common::Entities::SaleOrder &order);
    void editOrder(const Common::Entities::SaleOrder &order);
    void deleteOrder(const QString &id);
    void createOrderRecord(const Common::Entities::SalesOrderRecord &record);
    void editOrderRecord(const Common::Entities::SalesOrderRecord &record);
    void deleteOrderRecord(const QString &id);

private:
    SalesModel &m_model;
};
