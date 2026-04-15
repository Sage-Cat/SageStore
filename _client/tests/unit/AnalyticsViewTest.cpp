#include <QLabel>
#include <QProgressBar>
#include <QTableWidget>
#include <QtTest>

#include "Ui/Models/AnalyticsModel.hpp"
#include "Ui/ViewModels/AnalyticsViewModel.hpp"
#include "Ui/Views/AnalyticsView.hpp"

#include "mocks/ApiManagerMock.hpp"

class AnalyticsViewTest : public QObject {
    Q_OBJECT

private slots:
    void init()
    {
        apiManagerMock = new ApiManagerMock();
        analyticsModel = new AnalyticsModel(*apiManagerMock);
        viewModel = new AnalyticsViewModel(*analyticsModel);
        view = new AnalyticsView(*viewModel);
        view->show();
        viewModel->fetchAll();
        QTRY_VERIFY(salesTable()->rowCount() > 0);
        QTRY_VERIFY(inventoryTable()->rowCount() > 0);
    }

    void cleanup()
    {
        delete view;
        delete viewModel;
        delete analyticsModel;
        delete apiManagerMock;
    }

    void testDashboardShowsExpandedMetrics()
    {
        QCOMPARE(metricValue(salesTable(), "Gross profit"), QString("21.00"));
        QCOMPARE(metricValue(salesTable(), "Revenue"), QString("31.50"));
        QCOMPARE(metricValue(inventoryTable(), "Inventory value"), QString("105.00"));
        QCOMPARE(metricValue(inventoryTable(), "Purchase spend"), QString("52.50"));

        auto *salesRevenueValue = view->findChild<QLabel *>("analyticsSalesRevenueValue");
        auto *inventoryUnitsValue = view->findChild<QLabel *>("analyticsInventoryUnitsValue");
        QVERIFY(salesRevenueValue != nullptr);
        QVERIFY(inventoryUnitsValue != nullptr);
        QCOMPARE(salesRevenueValue->text(), QString("31.50"));
        QCOMPARE(inventoryUnitsValue->text(), QString("10"));
    }

    void testDashboardShowsCompactVisualPanels()
    {
        auto *salesDraftBar = view->findChild<QProgressBar *>("analyticsSalesDraftBar");
        auto *inventoryFlowBar =
            view->findChild<QProgressBar *>("analyticsInventoryOrdersFlowBar");

        QVERIFY(salesDraftBar != nullptr);
        QVERIFY(inventoryFlowBar != nullptr);
        QVERIFY(salesDraftBar->value() >= 0);
        QVERIFY(inventoryFlowBar->value() >= 0);
        QVERIFY(!salesDraftBar->format().isEmpty());
        QVERIFY(!inventoryFlowBar->format().isEmpty());
        QCOMPARE(salesTable()->maximumHeight(), 260);
        QCOMPARE(inventoryTable()->maximumHeight(), 260);
    }

    void testDashboardShowsTimeCharts()
    {
        auto *salesTimelineChart = view->findChild<QWidget *>("analyticsSalesTimelineChart");
        auto *inventoryTimelineChart =
            view->findChild<QWidget *>("analyticsInventoryTimelineChart");

        QVERIFY(salesTimelineChart != nullptr);
        QVERIFY(inventoryTimelineChart != nullptr);
        QCOMPARE(apiManagerMock->salesOrdersRequestCount(), 1);
        QCOMPARE(apiManagerMock->purchaseOrdersRequestCount(), 1);
        QCOMPARE(salesTimelineChart->property("pointCount").toInt(), 7);
        QCOMPARE(inventoryTimelineChart->property("pointCount").toInt(), 7);
        QVERIFY(salesTimelineChart->property("primaryPeak").toDouble() > 0.0);
        QVERIFY(inventoryTimelineChart->property("primaryPeak").toDouble() > 0.0);
        QVERIFY(!salesTimelineChart->property("dateLabels").toString().isEmpty());
        QVERIFY(!inventoryTimelineChart->property("dateLabels").toString().isEmpty());
    }

private:
    QTableWidget *salesTable() const
    {
        auto *table = view->findChild<QTableWidget *>("analyticsSalesTable");
        Q_ASSERT(table != nullptr);
        return table;
    }

    QTableWidget *inventoryTable() const
    {
        auto *table = view->findChild<QTableWidget *>("analyticsInventoryTable");
        Q_ASSERT(table != nullptr);
        return table;
    }

    QString metricValue(QTableWidget *table, const QString &metricLabel) const
    {
        for (int row = 0; row < table->rowCount(); ++row) {
            auto *labelItem = table->item(row, 0);
            auto *valueItem = table->item(row, 1);
            if (labelItem != nullptr && valueItem != nullptr && labelItem->text() == metricLabel) {
                return valueItem->text();
            }
        }

        return {};
    }

    ApiManagerMock *apiManagerMock{nullptr};
    AnalyticsModel *analyticsModel{nullptr};
    AnalyticsViewModel *viewModel{nullptr};
    AnalyticsView *view{nullptr};
};

QTEST_MAIN(AnalyticsViewTest)
#include "AnalyticsViewTest.moc"
