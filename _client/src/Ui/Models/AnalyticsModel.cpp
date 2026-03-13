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
}

QVariantMap AnalyticsModel::salesMetrics() const { return m_salesMetrics; }

QVariantMap AnalyticsModel::inventoryMetrics() const { return m_inventoryMetrics; }

void AnalyticsModel::fetchAll()
{
    m_apiManager.getSalesAnalytics();
    m_apiManager.getInventoryAnalytics();
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
