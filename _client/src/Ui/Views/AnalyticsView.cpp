#include "Ui/Views/AnalyticsView.hpp"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include "Ui/ViewModels/AnalyticsViewModel.hpp"

AnalyticsView::AnalyticsView(AnalyticsViewModel &viewModel, QWidget *parent)
    : QWidget(parent), m_viewModel(viewModel)
{
    setupUi();

    connect(&m_viewModel, &AnalyticsViewModel::salesMetricsChanged, this,
            &AnalyticsView::fillSalesMetrics);
    connect(&m_viewModel, &AnalyticsViewModel::inventoryMetricsChanged, this,
            &AnalyticsView::fillInventoryMetrics);
}

void AnalyticsView::showSection(Section section)
{
    m_tabs->setCurrentIndex(section == Section::Sales ? 0 : 1);
}

void AnalyticsView::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    auto *titleLabel = new QLabel(tr("Analytics Dashboard"), this);
    titleLabel->setObjectName("analyticsTitleLabel");

    m_tabs = new QTabWidget(this);
    auto *salesPage = new QWidget(this);
    auto *salesLayout = new QVBoxLayout(salesPage);
    m_salesTable = new QTableWidget(salesPage);
    salesLayout->addWidget(m_salesTable);

    auto *inventoryPage = new QWidget(this);
    auto *inventoryLayout = new QVBoxLayout(inventoryPage);
    m_inventoryTable = new QTableWidget(inventoryPage);
    inventoryLayout->addWidget(m_inventoryTable);

    for (auto *table : {m_salesTable, m_inventoryTable}) {
        table->setSelectionMode(QAbstractItemView::NoSelection);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->verticalHeader()->setVisible(false);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }

    m_tabs->addTab(salesPage, tr("Sales"));
    m_tabs->addTab(inventoryPage, tr("Inventory"));

    layout->addWidget(titleLabel);
    layout->addWidget(m_tabs);

    setObjectName("analyticsView");
    setStyleSheet(
        "#analyticsView { background-color: #fffaf2; }"
        "#analyticsTitleLabel { color: #233130; font-size: 22px; font-weight: 700; }"
        "QTableWidget { background-color: #fbfbfd; border: 1px solid #d7dde7; border-radius: 8px; }"
        "QHeaderView::section { background-color: #e8eef6; color: #0f172a; padding: 6px; border: 0; border-bottom: 1px solid #d7dde7; font-weight: 600; }");
}

void AnalyticsView::fillSalesMetrics() { fillTable(m_salesTable, m_viewModel.salesMetrics()); }

void AnalyticsView::fillInventoryMetrics()
{
    fillTable(m_inventoryTable, m_viewModel.inventoryMetrics());
}

void AnalyticsView::fillTable(QTableWidget *table,
                              const QVector<QPair<QString, QString>> &metrics) const
{
    table->clearContents();
    table->setRowCount(metrics.size());
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels({tr("Metric"), tr("Value")});
    for (int row = 0; row < metrics.size(); ++row) {
        table->setItem(row, 0, new QTableWidgetItem(metrics[row].first));
        table->setItem(row, 1, new QTableWidgetItem(metrics[row].second));
    }
}
