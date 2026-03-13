#pragma once

#include <QVariantMap>

#include "Ui/Models/BaseModel.hpp"

class ApiManager;

class AnalyticsModel : public BaseModel {
    Q_OBJECT

public:
    explicit AnalyticsModel(ApiManager &apiManager, QObject *parent = nullptr);

    QVariantMap salesMetrics() const;
    QVariantMap inventoryMetrics() const;

signals:
    void salesMetricsChanged();
    void inventoryMetricsChanged();

public slots:
    void fetchAll();

private slots:
    void onSalesAnalyticsReady(const QVariantMap &metrics);
    void onInventoryAnalyticsReady(const QVariantMap &metrics);

private:
    ApiManager &m_apiManager;
    QVariantMap m_salesMetrics;
    QVariantMap m_inventoryMetrics;
};
