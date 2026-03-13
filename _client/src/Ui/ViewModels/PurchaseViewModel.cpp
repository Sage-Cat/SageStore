#include "Ui/ViewModels/PurchaseViewModel.hpp"

#include "Ui/Models/PurchaseModel.hpp"

PurchaseViewModel::PurchaseViewModel(PurchaseModel &model, QObject *parent)
    : BaseViewModel(model, parent), m_model(model)
{
    connect(&m_model, &PurchaseModel::ordersChanged, this, &PurchaseViewModel::ordersChanged);
    connect(&m_model, &PurchaseModel::orderRecordsChanged, this,
            &PurchaseViewModel::orderRecordsChanged);
    connect(&m_model, &PurchaseModel::usersChanged, this, &PurchaseViewModel::referenceDataChanged);
    connect(&m_model, &PurchaseModel::suppliersChanged, this,
            &PurchaseViewModel::referenceDataChanged);
    connect(&m_model, &PurchaseModel::employeesChanged, this,
            &PurchaseViewModel::referenceDataChanged);
    connect(&m_model, &PurchaseModel::productTypesChanged, this,
            &PurchaseViewModel::referenceDataChanged);
    connect(&m_model, &PurchaseModel::purchaseReceiptPosted, this,
            &PurchaseViewModel::purchaseReceiptPosted);
    connect(&m_model, &PurchaseModel::errorOccurred, this, &PurchaseViewModel::errorOccurred);
}

QVector<Common::Entities::PurchaseOrder> PurchaseViewModel::orders() const { return m_model.orders(); }

QVector<Common::Entities::PurchaseOrderRecord> PurchaseViewModel::orderRecords() const
{
    return m_model.orderRecords();
}

QVector<Common::Entities::User> PurchaseViewModel::users() const { return m_model.users(); }

QVector<Common::Entities::Supplier> PurchaseViewModel::suppliers() const
{
    return m_model.suppliers();
}

QVector<Common::Entities::Employee> PurchaseViewModel::employees() const
{
    return m_model.employees();
}

QVector<Common::Entities::ProductType> PurchaseViewModel::productTypes() const
{
    return m_model.productTypes();
}

QString PurchaseViewModel::userLabel(const QString &userId) const
{
    for (const auto &user : m_model.users()) {
        if (QString::fromStdString(user.id) == userId) {
            return QString::fromStdString(user.username);
        }
    }
    return userId;
}

QString PurchaseViewModel::supplierLabel(const QString &supplierId) const
{
    for (const auto &supplier : m_model.suppliers()) {
        if (QString::fromStdString(supplier.id) == supplierId) {
            return QString::fromStdString(supplier.name);
        }
    }
    return supplierId;
}

QString PurchaseViewModel::employeeLabel(const QString &employeeId) const
{
    for (const auto &employee : m_model.employees()) {
        if (QString::fromStdString(employee.id) == employeeId) {
            return QString::fromStdString(employee.name);
        }
    }
    return employeeId;
}

QString PurchaseViewModel::productTypeLabel(const QString &productTypeId) const
{
    for (const auto &productType : m_model.productTypes()) {
        if (QString::fromStdString(productType.id) == productTypeId) {
            return QString::fromStdString(productType.code) + " | " +
                   QString::fromStdString(productType.name);
        }
    }
    return productTypeId;
}

void PurchaseViewModel::fetchAll() { m_model.fetchAll(); }

void PurchaseViewModel::fetchOrders() { m_model.fetchOrders(); }

void PurchaseViewModel::fetchOrderRecords(const QString &orderId)
{
    m_model.fetchOrderRecords(orderId);
}

void PurchaseViewModel::createOrder(const Common::Entities::PurchaseOrder &order)
{
    m_model.createOrder(order);
}

void PurchaseViewModel::editOrder(const Common::Entities::PurchaseOrder &order)
{
    m_model.editOrder(order);
}

void PurchaseViewModel::deleteOrder(const QString &id) { m_model.deleteOrder(id); }

void PurchaseViewModel::createOrderRecord(const Common::Entities::PurchaseOrderRecord &record)
{
    m_model.createOrderRecord(record);
}

void PurchaseViewModel::editOrderRecord(const Common::Entities::PurchaseOrderRecord &record)
{
    m_model.editOrderRecord(record);
}

void PurchaseViewModel::deleteOrderRecord(const QString &id) { m_model.deleteOrderRecord(id); }

void PurchaseViewModel::postReceipt(const QString &orderId, const QString &employeeId)
{
    m_model.postReceipt(orderId, employeeId);
}
