#pragma once

#include <QPair>
#include <QVariantMap>
#include <QVector>

#include "Ui/ViewModels/BaseViewModel.hpp"

class AnalyticsModel;

class AnalyticsViewModel : public BaseViewModel {
    Q_OBJECT

public:
    explicit AnalyticsViewModel(AnalyticsModel &model, QObject *parent = nullptr);

    QVector<QPair<QString, QString>> salesMetrics() const;
    QVector<QPair<QString, QString>> inventoryMetrics() const;

signals:
    void salesMetricsChanged();
    void inventoryMetricsChanged();
    void errorOccurred(const QString &errorMessage);

public slots:
    void fetchAll();

private:
    QVector<QPair<QString, QString>> normalizeMetrics(const QVariantMap &metrics) const;

    AnalyticsModel &m_model;
};
