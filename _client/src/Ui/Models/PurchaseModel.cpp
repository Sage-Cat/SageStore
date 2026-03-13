#include "Ui/Models/PurchaseModel.hpp"

#include "Network/ApiManager.hpp"

PurchaseModel::PurchaseModel(ApiManager &apiManager, QObject *parent)
    : BaseModel(parent), m_apiManager(apiManager)
{
    connect(&m_apiManager, &ApiManager::errorOccurred, this, &PurchaseModel::errorOccurred);
    connect(&m_apiManager, &ApiManager::purchaseOrdersList, this, &PurchaseModel::onOrdersList);
    connect(&m_apiManager, &ApiManager::purchaseOrderRecordsList, this,
            &PurchaseModel::onOrderRecordsList);
    connect(&m_apiManager, &ApiManager::usersList, this, &PurchaseModel::onUsersList);
    connect(&m_apiManager, &ApiManager::suppliersList, this, &PurchaseModel::onSuppliersList);
    connect(&m_apiManager, &ApiManager::employeesList, this, &PurchaseModel::onEmployeesList);
    connect(&m_apiManager, &ApiManager::productTypesList, this, &PurchaseModel::onProductTypesList);
    connect(&m_apiManager, &ApiManager::purchaseOrderCreated, this, &PurchaseModel::onOrderCreated);
    connect(&m_apiManager, &ApiManager::purchaseOrderEdited, this, &PurchaseModel::onOrderEdited);
    connect(&m_apiManager, &ApiManager::purchaseOrderDeleted, this, &PurchaseModel::onOrderDeleted);
    connect(&m_apiManager, &ApiManager::purchaseOrderRecordCreated, this,
            &PurchaseModel::onOrderRecordCreated);
    connect(&m_apiManager, &ApiManager::purchaseOrderRecordEdited, this,
            &PurchaseModel::onOrderRecordEdited);
    connect(&m_apiManager, &ApiManager::purchaseOrderRecordDeleted, this,
            &PurchaseModel::onOrderRecordDeleted);
    connect(&m_apiManager, &ApiManager::purchaseReceiptPosted, this, &PurchaseModel::onReceiptPosted);
}

QVector<Common::Entities::PurchaseOrder> PurchaseModel::orders() const { return m_orders; }

QVector<Common::Entities::PurchaseOrderRecord> PurchaseModel::orderRecords() const
{
    return m_orderRecords;
}

QVector<Common::Entities::User> PurchaseModel::users() const { return m_users; }

QVector<Common::Entities::Supplier> PurchaseModel::suppliers() const { return m_suppliers; }

QVector<Common::Entities::Employee> PurchaseModel::employees() const { return m_employees; }

QVector<Common::Entities::ProductType> PurchaseModel::productTypes() const
{
    return m_productTypes;
}

void PurchaseModel::fetchAll()
{
    fetchReferenceData();
    fetchOrders();
}

void PurchaseModel::fetchOrders() { m_apiManager.getPurchaseOrders(); }

void PurchaseModel::fetchOrderRecords(const QString &orderId)
{
    m_lastOrderId = orderId;
    m_apiManager.getPurchaseOrderRecords(orderId);
}

void PurchaseModel::fetchReferenceData()
{
    m_apiManager.getUsers();
    m_apiManager.getSuppliers();
    m_apiManager.getEmployees();
    m_apiManager.getProductTypes();
}

void PurchaseModel::createOrder(const Common::Entities::PurchaseOrder &order)
{
    m_apiManager.createPurchaseOrder(order);
}

void PurchaseModel::editOrder(const Common::Entities::PurchaseOrder &order)
{
    m_apiManager.editPurchaseOrder(order);
}

void PurchaseModel::deleteOrder(const QString &id) { m_apiManager.deletePurchaseOrder(id); }

void PurchaseModel::createOrderRecord(const Common::Entities::PurchaseOrderRecord &record)
{
    m_apiManager.createPurchaseOrderRecord(record);
}

void PurchaseModel::editOrderRecord(const Common::Entities::PurchaseOrderRecord &record)
{
    m_apiManager.editPurchaseOrderRecord(record);
}

void PurchaseModel::deleteOrderRecord(const QString &id)
{
    m_apiManager.deletePurchaseOrderRecord(id);
}

void PurchaseModel::postReceipt(const QString &orderId, const QString &employeeId)
{
    m_apiManager.postPurchaseReceipt(orderId, employeeId);
}

void PurchaseModel::onOrdersList(const std::vector<Common::Entities::PurchaseOrder> &orders)
{
    m_orders = QVector<Common::Entities::PurchaseOrder>(orders.begin(), orders.end());
    emit ordersChanged();
}

void PurchaseModel::onOrderRecordsList(
    const std::vector<Common::Entities::PurchaseOrderRecord> &records)
{
    m_orderRecords = QVector<Common::Entities::PurchaseOrderRecord>(records.begin(), records.end());
    emit orderRecordsChanged();
}

void PurchaseModel::onUsersList(const std::vector<Common::Entities::User> &users)
{
    m_users = QVector<Common::Entities::User>(users.begin(), users.end());
    emit usersChanged();
}

void PurchaseModel::onSuppliersList(const std::vector<Common::Entities::Supplier> &suppliers)
{
    m_suppliers = QVector<Common::Entities::Supplier>(suppliers.begin(), suppliers.end());
    emit suppliersChanged();
}

void PurchaseModel::onEmployeesList(const std::vector<Common::Entities::Employee> &employees)
{
    m_employees = QVector<Common::Entities::Employee>(employees.begin(), employees.end());
    emit employeesChanged();
}

void PurchaseModel::onProductTypesList(const std::vector<Common::Entities::ProductType> &productTypes)
{
    m_productTypes = QVector<Common::Entities::ProductType>(productTypes.begin(), productTypes.end());
    emit productTypesChanged();
}

void PurchaseModel::onOrderCreated()
{
    fetchOrders();
    refreshCurrentRecords();
}

void PurchaseModel::onOrderEdited()
{
    fetchOrders();
    refreshCurrentRecords();
}

void PurchaseModel::onOrderDeleted()
{
    fetchOrders();
    refreshCurrentRecords();
}

void PurchaseModel::onOrderRecordCreated() { refreshCurrentRecords(); }

void PurchaseModel::onOrderRecordEdited() { refreshCurrentRecords(); }

void PurchaseModel::onOrderRecordDeleted() { refreshCurrentRecords(); }

void PurchaseModel::onReceiptPosted()
{
    fetchOrders();
    refreshCurrentRecords();
    emit purchaseReceiptPosted();
}

void PurchaseModel::refreshCurrentRecords()
{
    if (!m_lastOrderId.isEmpty()) {
        m_apiManager.getPurchaseOrderRecords(m_lastOrderId);
    }
}
