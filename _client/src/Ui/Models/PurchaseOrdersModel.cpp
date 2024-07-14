#include "PurchaseOrdersModel.hpp"

#include "Network/ApiManager.hpp"

#include "common/SpdlogConfig.hpp"

PurchaseOrdersModel::PurchaseOrdersModel(ApiManager &apiManager, QObject *parent)
    : BaseModel(parent), m_apiManager(apiManager)
{
    SPDLOG_TRACE("PurchaseOrdersModel::PurchaseOrdersModel");

    connect(&m_apiManager, &ApiManager::purchaseOrderAdded, this, &PurchaseOrdersModel::onPurchaseOrderAdded);
    connect(&m_apiManager, &ApiManager::purchaseOrderEdited, this, &PurchaseOrdersModel::onPurchaseOrderUpdated);
    connect(&m_apiManager, &ApiManager::purchaseOrderDeleted, this, &PurchaseOrdersModel::onPurchaseOrderDeleted);
    connect(&m_apiManager, &ApiManager::purchaseOrdersList, this, &PurchaseOrdersModel::onPurchaseOrdersList);

    connect(&m_apiManager, &ApiManager::purchaseOrderRecordCreated, this, &PurchaseOrdersModel::onPurchaseOrderRecordCreated);
    connect(&m_apiManager, &ApiManager::purchaseOrderRecordEdited, this, &PurchaseOrdersModel::onPurchaseOrderRecordEdited);
    connect(&m_apiManager, &ApiManager::purchaseOrderRecordDeleted, this, &PurchaseOrdersModel::onPurchaseOrderRecordDeleted);
    connect(&m_apiManager, &ApiManager::purchaseOrderRecordsList, this, &PurchaseOrdersModel::onPurchaseOrderRecordsList);

    // Fetch data on startup
    SPDLOG_TRACE("PurchaseOrdersModel | On startup fetch data");
    fetchOrders();
    fetchOrderRecords();
}


QVector<Common::Entities::PurchaseOrder> PurchaseOrdersModel::orders() const { return m_orders; }
QVector<Common::Entities::PurchaseOrderRecord> PurchaseOrdersModel::ordersRecord() const { return m_ordersRecord; }

void PurchaseOrdersModel::fetchOrders() 
{ 
    m_apiManager.getPurchaseOrders(); 
}

void PurchaseOrdersModel::addOrder(const Common::Entities::PurchaseOrder &purchaseOrder) 
{ 
    m_apiManager.createPurchaseOrder(purchaseOrder); 
}

void PurchaseOrdersModel::editOrder(const Common::Entities::PurchaseOrder &purchaseOrder) 
{ 
    m_apiManager.editPurchaseOrder(purchaseOrder); 
}

void PurchaseOrdersModel::deleteOrder(const QString &id) 
{ 
    m_apiManager.deletePurchaseOrder(id); 
}


void PurchaseOrdersModel::fetchOrderRecords() 
{
    m_apiManager.getPurchaseOrderRecords(); 
}

void PurchaseOrdersModel::createOrderRecord(const Common::Entities::PurchaseOrderRecord &purchaseOrderRecord) 
{
    m_apiManager.createPurchaseOrderRecord(purchaseOrderRecord); 
}

void PurchaseOrdersModel::editOrderRecord(const Common::Entities::PurchaseOrderRecord &purchaseOrderRecord) 
{
    m_apiManager.editPurchaseOrderRecord(purchaseOrderRecord); 
}

void PurchaseOrdersModel::deleteOrderRecord(const QString &id) 
{
    m_apiManager.deletePurchaseOrderRecord(id); 
}

void PurchaseOrdersModel::onPurchaseOrdersList(const std::vector<Common::Entities::PurchaseOrder> &orders)
{
    m_orders = QVector<Common::Entities::PurchaseOrder>(orders.begin(), orders.end());
    emit ordersChanged();
}

void PurchaseOrdersModel::onPurchaseOrderAdded()
{
    fetchOrders();
    emit orderAdded();
}

void PurchaseOrdersModel::onPurchaseOrderUpdated()
{
    fetchOrders();
    emit orderEdited();
}

void PurchaseOrdersModel::onPurchaseOrderDeleted()
{
    fetchOrders();
    emit orderDeleted();
}

void PurchaseOrdersModel::onPurchaseOrderRecordsList(const std::vector<Common::Entities::PurchaseOrderRecord> &ordersRecord)
{
    m_ordersRecord = QVector<Common::Entities::PurchaseOrderRecord>(ordersRecord.begin(), ordersRecord.end());
    emit orderRecordsChanged();
}

void PurchaseOrdersModel::onPurchaseOrderRecordCreated()
{
    fetchOrderRecords();
    emit orderRecordCreated();
}

void PurchaseOrdersModel::onPurchaseOrderRecordEdited()
{
    fetchOrderRecords();
    emit orderRecordEdited();
}

void PurchaseOrdersModel::onPurchaseOrderRecordDeleted()
{
    fetchOrderRecords();
    emit orderRecordDeleted();
}

