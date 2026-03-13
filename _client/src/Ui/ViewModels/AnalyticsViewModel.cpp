#include "Ui/ViewModels/AnalyticsViewModel.hpp"

#include <algorithm>

#include "Ui/Models/AnalyticsModel.hpp"

AnalyticsViewModel::AnalyticsViewModel(AnalyticsModel &model, QObject *parent)
    : BaseViewModel(model, parent), m_model(model)
{
    connect(&m_model, &AnalyticsModel::salesMetricsChanged, this,
            &AnalyticsViewModel::salesMetricsChanged);
    connect(&m_model, &AnalyticsModel::inventoryMetricsChanged, this,
            &AnalyticsViewModel::inventoryMetricsChanged);
    connect(&m_model, &AnalyticsModel::errorOccurred, this,
            &AnalyticsViewModel::errorOccurred);
}

QVector<QPair<QString, QString>> AnalyticsViewModel::salesMetrics() const
{
    return normalizeMetrics(m_model.salesMetrics());
}

QVector<QPair<QString, QString>> AnalyticsViewModel::inventoryMetrics() const
{
    return normalizeMetrics(m_model.inventoryMetrics());
}

void AnalyticsViewModel::fetchAll() { m_model.fetchAll(); }

QVector<QPair<QString, QString>> AnalyticsViewModel::normalizeMetrics(const QVariantMap &metrics) const
{
    QVector<QPair<QString, QString>> rows;
    for (auto it = metrics.cbegin(); it != metrics.cend(); ++it) {
        rows.push_back({it.key(), it.value().toString()});
    }
    std::sort(rows.begin(), rows.end(), [](const auto &lhs, const auto &rhs) {
        return lhs.first < rhs.first;
    });
    return rows;
}
