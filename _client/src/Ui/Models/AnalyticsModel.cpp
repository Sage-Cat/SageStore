#include "Ui/Models/AnalyticsModel.hpp"

#include "Network/ApiManager.hpp"

AnalyticsModel::AnalyticsModel(ApiManager &apiManager, QObject *parent)
    : BaseModel(parent), m_apiManager(apiManager)
{
    connect(&m_apiManager, &ApiManager::errorOccurred, this, &AnalyticsModel::errorOccurred);
    connect(&m_apiManager, &ApiManager::salesAnalyticsReady, this,
            &AnalyticsModel::onSalesAnalyticsReady);
    connect(&m_apiManager, &ApiManager::inventoryAnalyticsReady, this,
            &AnalyticsModel::onInventoryAnalyticsReady);
    connect(&m_apiManager, &ApiManager::salesOrdersList, this, &AnalyticsModel::onSalesOrdersList);
    connect(&m_apiManager, &ApiManager::purchaseOrdersList, this,
            &AnalyticsModel::onPurchaseOrdersList);
}

QVariantMap AnalyticsModel::salesMetrics() const { return m_salesMetrics; }

QVariantMap AnalyticsModel::inventoryMetrics() const { return m_inventoryMetrics; }

const std::vector<Common::Entities::SaleOrder> &AnalyticsModel::salesOrders() const
{
    return m_salesOrders;
}

const std::vector<Common::Entities::PurchaseOrder> &AnalyticsModel::purchaseOrders() const
{
    return m_purchaseOrders;
}

void AnalyticsModel::fetchAll()
{
    m_apiManager.getSalesAnalytics();
    m_apiManager.getInventoryAnalytics();
    m_apiManager.getSalesOrders();
    m_apiManager.getPurchaseOrders();
}

void AnalyticsModel::onSalesAnalyticsReady(const QVariantMap &metrics)
{
    m_salesMetrics = metrics;
    emit salesMetricsChanged();
}

void AnalyticsModel::onInventoryAnalyticsReady(const QVariantMap &metrics)
{
    m_inventoryMetrics = metrics;
    emit inventoryMetricsChanged();
}

void AnalyticsModel::onSalesOrdersList(const std::vector<Common::Entities::SaleOrder> &orders)
{
    m_salesOrders = orders;
    emit salesOrdersChanged();
}

void AnalyticsModel::onPurchaseOrdersList(
    const std::vector<Common::Entities::PurchaseOrder> &orders)
{
    m_purchaseOrders = orders;
    emit purchaseOrdersChanged();
}
