#pragma once

#include <QTableWidget>
#include <QTabWidget>
#include <QVector>
#include <QWidget>

class AnalyticsViewModel;

class AnalyticsView : public QWidget {
    Q_OBJECT

public:
    enum class Section { Sales, Inventory };

    explicit AnalyticsView(AnalyticsViewModel &viewModel, QWidget *parent = nullptr);

    void showSection(Section section);

private slots:
    void fillSalesMetrics();
    void fillInventoryMetrics();

private:
    void setupUi();
    void fillTable(QTableWidget *table, const QVector<QPair<QString, QString>> &metrics) const;

    AnalyticsViewModel &m_viewModel;
    QTabWidget *m_tabs{nullptr};
    QTableWidget *m_salesTable{nullptr};
    QTableWidget *m_inventoryTable{nullptr};
};
