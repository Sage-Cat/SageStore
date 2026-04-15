#pragma once

#include <vector>

#include <QPair>
#include <QString>
#include <QVariantMap>
#include <QVector>

#include "Ui/ViewModels/BaseViewModel.hpp"
#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/SaleOrder.hpp"

class AnalyticsModel;

class AnalyticsViewModel : public BaseViewModel {
    Q_OBJECT

public:
    explicit AnalyticsViewModel(AnalyticsModel &model, QObject *parent = nullptr);

    QVector<QPair<QString, QString>> salesMetrics() const;
    QVector<QPair<QString, QString>> inventoryMetrics() const;
    QString salesMetricValue(const QString &key) const;
    QString inventoryMetricValue(const QString &key) const;
    double salesMetricNumber(const QString &key) const;
    double inventoryMetricNumber(const QString &key) const;
    const std::vector<Common::Entities::SaleOrder> &salesOrders() const;
    const std::vector<Common::Entities::PurchaseOrder> &purchaseOrders() const;

signals:
    void salesMetricsChanged();
    void inventoryMetricsChanged();
    void salesOrdersChanged();
    void purchaseOrdersChanged();
    void errorOccurred(const QString &errorMessage);

public slots:
    void fetchAll();

private:
    QVector<QPair<QString, QString>> normalizeMetrics(const QVariantMap &metrics,
                                                      const QVector<QString> &orderedKeys) const;
    QString labelForMetric(const QString &key) const;
    QString formatMetricValue(const QString &key, const QVariant &value) const;
    double metricNumber(const QVariantMap &metrics, const QString &key) const;

    AnalyticsModel &m_model;
};
