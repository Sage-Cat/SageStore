#pragma once

#include <QLabel>
#include <QProgressBar>
#include <QTableWidget>
#include <QTabWidget>
#include <QVector>
#include <QWidget>

class AnalyticsViewModel;
class TimelineChartWidget;

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
    void refreshSalesTimeline();
    void refreshInventoryTimeline();
    void fillTable(QTableWidget *table, const QVector<QPair<QString, QString>> &metrics) const;
    void updateScaledBar(QProgressBar *bar, double value, double maxValue, const QString &text) const;
    void updateRatioBar(QProgressBar *bar, double ratio, const QString &text) const;

    AnalyticsViewModel &m_viewModel;
    QTabWidget *m_tabs{nullptr};
    QLabel *m_salesRevenueValue{nullptr};
    QLabel *m_salesProfitValue{nullptr};
    QLabel *m_salesMarginValue{nullptr};
    QLabel *m_salesCustomersValue{nullptr};
    QLabel *m_salesDraftValue{nullptr};
    QLabel *m_salesConfirmedValue{nullptr};
    QLabel *m_salesInvoicedValue{nullptr};
    QLabel *m_inventoryUnitsValue{nullptr};
    QLabel *m_inventoryValueValue{nullptr};
    QLabel *m_inventoryPurchaseSpendValue{nullptr};
    QLabel *m_inventoryProductTypesValue{nullptr};
    QLabel *m_inventoryPurchaseOrdersValue{nullptr};
    QLabel *m_inventoryReceivedOrdersValue{nullptr};
    QLabel *m_inventoryStockRecordsValue{nullptr};
    QLabel *m_inventoryZeroStockValue{nullptr};
    QProgressBar *m_salesRevenueBar{nullptr};
    QProgressBar *m_salesCostBar{nullptr};
    QProgressBar *m_salesProfitBar{nullptr};
    QProgressBar *m_salesDraftBar{nullptr};
    QProgressBar *m_salesConfirmedBar{nullptr};
    QProgressBar *m_salesInvoicedBar{nullptr};
    QProgressBar *m_inventoryOrdersFlowBar{nullptr};
    QProgressBar *m_inventoryReceiptsFlowBar{nullptr};
    QProgressBar *m_inventoryZeroStockFlowBar{nullptr};
    QProgressBar *m_inventoryReceivedBar{nullptr};
    QProgressBar *m_inventoryImportedBar{nullptr};
    QProgressBar *m_inventoryStockHealthBar{nullptr};
    TimelineChartWidget *m_salesTimelineChart{nullptr};
    TimelineChartWidget *m_inventoryTimelineChart{nullptr};
    QTableWidget *m_salesTable{nullptr};
    QTableWidget *m_inventoryTable{nullptr};
};
