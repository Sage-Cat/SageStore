#pragma once

#include <vector>

#include <QVariantMap>

#include "Ui/Models/BaseModel.hpp"
#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/SaleOrder.hpp"

class ApiManager;

class AnalyticsModel : public BaseModel {
    Q_OBJECT

public:
    explicit AnalyticsModel(ApiManager &apiManager, QObject *parent = nullptr);

    QVariantMap salesMetrics() const;
    QVariantMap inventoryMetrics() const;
    const std::vector<Common::Entities::SaleOrder> &salesOrders() const;
    const std::vector<Common::Entities::PurchaseOrder> &purchaseOrders() const;

signals:
    void salesMetricsChanged();
    void inventoryMetricsChanged();
    void salesOrdersChanged();
    void purchaseOrdersChanged();

public slots:
    void fetchAll();

private slots:
    void onSalesAnalyticsReady(const QVariantMap &metrics);
    void onInventoryAnalyticsReady(const QVariantMap &metrics);
    void onSalesOrdersList(const std::vector<Common::Entities::SaleOrder> &orders);
    void onPurchaseOrdersList(const std::vector<Common::Entities::PurchaseOrder> &orders);

private:
    ApiManager &m_apiManager;
    QVariantMap m_salesMetrics;
    QVariantMap m_inventoryMetrics;
    std::vector<Common::Entities::SaleOrder> m_salesOrders;
    std::vector<Common::Entities::PurchaseOrder> m_purchaseOrders;
};
