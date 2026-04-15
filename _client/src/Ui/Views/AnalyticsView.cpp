#include "Ui/Views/AnalyticsView.hpp"

#include <QAbstractItemView>
#include <QDate>
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QProgressBar>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <algorithm>
#include <cmath>

#include "Ui/ViewModels/AnalyticsViewModel.hpp"
#include "common/AnalyticsKeys.hpp"

class TimelineChartWidget : public QWidget {
public:
    struct Point {
        QString label;
        double primaryValue{0.0};
        double secondaryValue{0.0};
    };

    explicit TimelineChartWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        setMinimumHeight(196);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

    void setSeries(QString primaryLabel, QColor primaryColor, QString secondaryLabel,
                   QColor secondaryColor, QVector<Point> points)
    {
        m_primaryLabel = std::move(primaryLabel);
        m_primaryColor = std::move(primaryColor);
        m_secondaryLabel = std::move(secondaryLabel);
        m_secondaryColor = std::move(secondaryColor);
        m_points = std::move(points);

        double primaryPeak = 0.0;
        double secondaryPeak = 0.0;
        QStringList labels;
        labels.reserve(m_points.size());
        for (const auto &point : m_points) {
            primaryPeak = std::max(primaryPeak, point.primaryValue);
            secondaryPeak = std::max(secondaryPeak, point.secondaryValue);
            labels.push_back(point.label);
        }

        setProperty("pointCount", m_points.size());
        setProperty("primaryPeak", primaryPeak);
        setProperty("secondaryPeak", secondaryPeak);
        setProperty("dateLabels", labels.join(QStringLiteral("|")));
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.fillRect(rect(), Qt::transparent);

        const QRectF canvas = rect().adjusted(6, 10, -6, -10);
        const QRectF plotRect = canvas.adjusted(10, 8, -10, -28);

        painter.setPen(QPen(QColor("#e2e8f0"), 1.0));
        for (int grid = 0; grid < 4; ++grid) {
            const qreal y =
                plotRect.top() + (plotRect.height() / 3.0) * static_cast<qreal>(grid);
            painter.drawLine(QPointF(plotRect.left(), y), QPointF(plotRect.right(), y));
        }

        if (m_points.isEmpty()) {
            return;
        }

        const double maxValue = std::max(
            1.0, std::max(maxSeriesValue(true), maxSeriesValue(false)));

        auto pointAt = [&](int index, double value) {
            const qreal xStep = m_points.size() == 1
                                    ? 0.0
                                    : plotRect.width() / static_cast<qreal>(m_points.size() - 1);
            const qreal x = plotRect.left() + xStep * static_cast<qreal>(index);
            const qreal normalized = std::clamp(value / maxValue, 0.0, 1.0);
            const qreal y = plotRect.bottom() - normalized * plotRect.height();
            return QPointF(x, y);
        };

        QPainterPath primaryPath;
        primaryPath.moveTo(pointAt(0, m_points.front().primaryValue));
        for (int index = 1; index < m_points.size(); ++index) {
            primaryPath.lineTo(pointAt(index, m_points[index].primaryValue));
        }

        QPainterPath primaryArea = primaryPath;
        primaryArea.lineTo(QPointF(pointAt(m_points.size() - 1, 0.0).x(), plotRect.bottom()));
        primaryArea.lineTo(QPointF(pointAt(0, 0.0).x(), plotRect.bottom()));
        primaryArea.closeSubpath();

        QLinearGradient areaGradient(plotRect.topLeft(), plotRect.bottomLeft());
        QColor areaTop = m_primaryColor;
        areaTop.setAlpha(64);
        QColor areaBottom = m_primaryColor;
        areaBottom.setAlpha(8);
        areaGradient.setColorAt(0.0, areaTop);
        areaGradient.setColorAt(1.0, areaBottom);
        painter.fillPath(primaryArea, areaGradient);

        QPen primaryPen(m_primaryColor, 2.5);
        painter.setPen(primaryPen);
        painter.drawPath(primaryPath);

        QPainterPath secondaryPath;
        secondaryPath.moveTo(pointAt(0, m_points.front().secondaryValue));
        for (int index = 1; index < m_points.size(); ++index) {
            secondaryPath.lineTo(pointAt(index, m_points[index].secondaryValue));
        }
        QPen secondaryPen(m_secondaryColor, 1.8, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(secondaryPen);
        painter.drawPath(secondaryPath);

        for (int index = 0; index < m_points.size(); ++index) {
            const QPointF primaryPoint = pointAt(index, m_points[index].primaryValue);
            painter.setPen(Qt::NoPen);
            painter.setBrush(Qt::white);
            painter.drawEllipse(primaryPoint, 4.0, 4.0);
            painter.setBrush(m_primaryColor);
            painter.drawEllipse(primaryPoint, 2.5, 2.5);

            const QPointF secondaryPoint = pointAt(index, m_points[index].secondaryValue);
            painter.setBrush(Qt::white);
            painter.setPen(QPen(m_secondaryColor, 1.4));
            painter.drawEllipse(secondaryPoint, 3.2, 3.2);

            painter.setPen(QColor("#64748b"));
            const QRectF labelRect(primaryPoint.x() - 24.0, plotRect.bottom() + 6.0, 48.0, 18.0);
            painter.drawText(labelRect, Qt::AlignCenter, m_points[index].label);
        }
    }

private:
    double maxSeriesValue(bool primary) const
    {
        double peak = 0.0;
        for (const auto &point : m_points) {
            peak = std::max(peak, primary ? point.primaryValue : point.secondaryValue);
        }
        return peak;
    }

    QString m_primaryLabel;
    QColor m_primaryColor{"#16a34a"};
    QString m_secondaryLabel;
    QColor m_secondaryColor{"#0f766e"};
    QVector<Point> m_points;
};

namespace {
QFrame *createMetricCard(const QString &title, QLabel **valueLabel, QWidget *parent)
{
    auto *card = new QFrame(parent);
    card->setObjectName("analyticsMetricCard");
    card->setProperty("metricCard", true);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    card->setMinimumHeight(92);
    card->setMaximumHeight(112);
    auto *layout = new QVBoxLayout(card);
    auto *titleLabel = new QLabel(title, card);
    auto *value = new QLabel(QStringLiteral("0"), card);
    titleLabel->setObjectName("analyticsCardTitle");
    titleLabel->setProperty("metricCaption", true);
    value->setObjectName("analyticsCardValue");
    layout->addWidget(titleLabel);
    layout->addWidget(value);
    layout->setSpacing(4);
    layout->setContentsMargins(14, 12, 14, 12);
    if (valueLabel != nullptr) {
        *valueLabel = value;
    }
    return card;
}

QWidget *createProgressRow(const QString &title, QProgressBar **bar, QWidget *parent)
{
    auto *container = new QWidget(parent);
    auto *layout = new QVBoxLayout(container);
    auto *label = new QLabel(title, container);
    label->setProperty("metricCaption", true);
    auto *progress = new QProgressBar(container);
    progress->setTextVisible(true);
    progress->setMinimumHeight(20);
    progress->setRange(0, 100);
    layout->addWidget(label);
    layout->addWidget(progress);
    layout->setSpacing(6);
    layout->setContentsMargins(0, 0, 0, 0);
    if (bar != nullptr) {
        *bar = progress;
    }
    return container;
}

QLabel *createLegendBadge(const QString &text, const QString &accentColor, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setProperty("metricCaption", true);
    label->setStyleSheet(QStringLiteral(
                             "QLabel { border-radius: 999px; padding: 4px 10px; "
                             "background-color: rgba(255,255,255,0.92); border: 1px solid #dbe3ee; }")
                             + QStringLiteral("QLabel { color: %1; }").arg(accentColor));
    return label;
}

QFrame *createTimelineCard(const QString &title, const QString &subtitle, TimelineChartWidget **chart,
                           const QString &primaryLegend, const QString &primaryColor,
                           const QString &secondaryLegend, const QString &secondaryColor,
                           const QString &chartObjectName, QWidget *parent)
{
    auto *card = new QFrame(parent);
    card->setObjectName("analyticsSectionCard");
    card->setProperty("card", true);
    auto *layout = new QVBoxLayout(card);
    layout->setSpacing(10);
    layout->setContentsMargins(16, 16, 16, 16);

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("analyticsSectionLabel");
    auto *subtitleLabel = new QLabel(subtitle, card);
    subtitleLabel->setProperty("muted", true);
    subtitleLabel->setWordWrap(true);

    auto *legendRow = new QHBoxLayout;
    legendRow->setSpacing(8);
    legendRow->addWidget(createLegendBadge(primaryLegend, primaryColor, card));
    legendRow->addWidget(createLegendBadge(secondaryLegend, secondaryColor, card));
    legendRow->addStretch();

    auto *timelineChart = new TimelineChartWidget(card);
    timelineChart->setObjectName(chartObjectName);

    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addLayout(legendRow);
    layout->addWidget(timelineChart);

    if (chart != nullptr) {
        *chart = timelineChart;
    }
    return card;
}

QDate parseIsoDate(const std::string &dateText)
{
    return QDate::fromString(QString::fromStdString(dateText).trimmed(), QStringLiteral("yyyy-MM-dd"));
}

QString canonicalSalesStatus(const std::string &status)
{
    const QString normalized = QString::fromStdString(status).trimmed();
    if (normalized.compare(QStringLiteral("Invoiced"), Qt::CaseInsensitive) == 0 ||
        normalized == QStringLiteral("Виставлено рахунок")) {
        return QStringLiteral("Invoiced");
    }
    if (normalized.compare(QStringLiteral("Confirmed"), Qt::CaseInsensitive) == 0 ||
        normalized == QStringLiteral("Підтверджено")) {
        return QStringLiteral("Confirmed");
    }
    if (normalized.compare(QStringLiteral("Draft"), Qt::CaseInsensitive) == 0 ||
        normalized == QStringLiteral("Чернетка")) {
        return QStringLiteral("Draft");
    }
    return normalized;
}

QString canonicalPurchaseStatus(const std::string &status)
{
    const QString normalized = QString::fromStdString(status).trimmed();
    if (normalized.compare(QStringLiteral("Received"), Qt::CaseInsensitive) == 0 ||
        normalized == QStringLiteral("Отримано")) {
        return QStringLiteral("Received");
    }
    if (normalized.compare(QStringLiteral("Ordered"), Qt::CaseInsensitive) == 0 ||
        normalized.compare(QStringLiteral("Pending"), Qt::CaseInsensitive) == 0 ||
        normalized == QStringLiteral("Замовлено")) {
        return QStringLiteral("Ordered");
    }
    if (normalized.compare(QStringLiteral("Draft"), Qt::CaseInsensitive) == 0 ||
        normalized == QStringLiteral("Чернетка")) {
        return QStringLiteral("Draft");
    }
    return normalized;
}

template <typename Order, typename SecondaryPredicate>
QVector<TimelineChartWidget::Point> buildOrderTimeline(const std::vector<Order> &orders,
                                                       SecondaryPredicate secondaryPredicate)
{
    QDate anchorDate;
    for (const auto &order : orders) {
        const QDate date = parseIsoDate(order.date);
        if (date.isValid() && (!anchorDate.isValid() || date > anchorDate)) {
            anchorDate = date;
        }
    }

    if (!anchorDate.isValid()) {
        anchorDate = QDate::currentDate();
    }

    const QDate startDate = anchorDate.addDays(-6);
    QVector<TimelineChartWidget::Point> points;
    points.reserve(7);
    for (int offset = 0; offset < 7; ++offset) {
        const QDate date = startDate.addDays(offset);
        points.push_back({date.toString(QStringLiteral("dd MMM")), 0.0, 0.0});
    }

    for (const auto &order : orders) {
        const QDate date = parseIsoDate(order.date);
        if (!date.isValid() || date < startDate || date > anchorDate) {
            continue;
        }

        const int index = startDate.daysTo(date);
        if (index < 0 || index >= points.size()) {
            continue;
        }

        points[index].primaryValue += 1.0;
        if (secondaryPredicate(order)) {
            points[index].secondaryValue += 1.0;
        }
    }

    return points;
}
} // namespace

AnalyticsView::AnalyticsView(AnalyticsViewModel &viewModel, QWidget *parent)
    : QWidget(parent), m_viewModel(viewModel)
{
    setupUi();

    connect(&m_viewModel, &AnalyticsViewModel::salesMetricsChanged, this,
            &AnalyticsView::fillSalesMetrics);
    connect(&m_viewModel, &AnalyticsViewModel::inventoryMetricsChanged, this,
            &AnalyticsView::fillInventoryMetrics);
    connect(&m_viewModel, &AnalyticsViewModel::salesOrdersChanged, this,
            &AnalyticsView::refreshSalesTimeline);
    connect(&m_viewModel, &AnalyticsViewModel::purchaseOrdersChanged, this,
            &AnalyticsView::refreshInventoryTimeline);
}

void AnalyticsView::showSection(Section section)
{
    m_tabs->setCurrentIndex(section == Section::Sales ? 0 : 1);
}

void AnalyticsView::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(14);
    layout->setContentsMargins(18, 18, 18, 18);
    auto *titleLabel = new QLabel(tr("Analytics Dashboard"), this);
    auto *summaryLabel = new QLabel(
        tr("Track the goods lifecycle from purchasing to stock and sales, with revenue, cost, "
           "margin, and stock-health snapshots grounded in the current baseline data."),
        this);
    titleLabel->setObjectName("analyticsTitleLabel");
    summaryLabel->setObjectName("analyticsSummaryLabel");
    summaryLabel->setProperty("muted", true);
    summaryLabel->setWordWrap(true);

    m_tabs = new QTabWidget(this);
    auto *salesPage = new QWidget(this);
    auto *salesLayout = new QVBoxLayout(salesPage);
    salesLayout->setSpacing(14);
    auto *salesCardsGrid = new QGridLayout;
    salesCardsGrid->setHorizontalSpacing(12);
    salesCardsGrid->setVerticalSpacing(12);
    salesCardsGrid->addWidget(createMetricCard(tr("Revenue"), &m_salesRevenueValue, salesPage), 0,
                              0);
    salesCardsGrid->addWidget(createMetricCard(tr("Gross Profit"), &m_salesProfitValue, salesPage),
                              0, 1);
    salesCardsGrid->addWidget(createMetricCard(tr("Gross Margin"), &m_salesMarginValue, salesPage),
                              1, 0);
    salesCardsGrid->addWidget(createMetricCard(tr("Customers"), &m_salesCustomersValue, salesPage),
                              1, 1);
    m_salesRevenueValue->setObjectName("analyticsSalesRevenueValue");
    m_salesProfitValue->setObjectName("analyticsSalesProfitValue");
    m_salesMarginValue->setObjectName("analyticsSalesMarginValue");
    m_salesCustomersValue->setObjectName("analyticsSalesCustomersValue");

    auto *salesBarsFrame = new QFrame(salesPage);
    salesBarsFrame->setObjectName("analyticsSectionCard");
    salesBarsFrame->setProperty("card", true);
    auto *salesBarsLayout = new QVBoxLayout(salesBarsFrame);
    auto *salesBarsLabel = new QLabel(tr("Income Balance"), salesBarsFrame);
    salesBarsLabel->setObjectName("analyticsSectionLabel");
    salesBarsLayout->addWidget(salesBarsLabel);
    salesBarsLayout->addWidget(createProgressRow(tr("Revenue"), &m_salesRevenueBar, salesBarsFrame));
    salesBarsLayout->addWidget(
        createProgressRow(tr("Purchase Cost"), &m_salesCostBar, salesBarsFrame));
    salesBarsLayout->addWidget(
        createProgressRow(tr("Gross Profit"), &m_salesProfitBar, salesBarsFrame));
    m_salesRevenueBar->setObjectName("analyticsSalesRevenueBar");
    m_salesCostBar->setObjectName("analyticsSalesCostBar");
    m_salesProfitBar->setObjectName("analyticsSalesProfitBar");
    m_salesRevenueBar->setProperty("tone", "revenue");
    m_salesCostBar->setProperty("tone", "cost");
    m_salesProfitBar->setProperty("tone", "profit");

    auto *salesFlowFrame = new QFrame(salesPage);
    salesFlowFrame->setObjectName("analyticsSectionCard");
    salesFlowFrame->setProperty("card", true);
    auto *salesFlowLayout = new QVBoxLayout(salesFlowFrame);
    auto *salesFlowLabel = new QLabel(tr("Pipeline Distribution"), salesFlowFrame);
    salesFlowLabel->setObjectName("analyticsSectionLabel");
    salesFlowLayout->addWidget(salesFlowLabel);
    salesFlowLayout->addWidget(createProgressRow(tr("Draft"), &m_salesDraftBar, salesFlowFrame));
    salesFlowLayout->addWidget(
        createProgressRow(tr("Confirmed"), &m_salesConfirmedBar, salesFlowFrame));
    salesFlowLayout->addWidget(
        createProgressRow(tr("Invoiced"), &m_salesInvoicedBar, salesFlowFrame));
    m_salesDraftBar->setObjectName("analyticsSalesDraftBar");
    m_salesConfirmedBar->setObjectName("analyticsSalesConfirmedBar");
    m_salesInvoicedBar->setObjectName("analyticsSalesInvoicedBar");
    m_salesDraftBar->setProperty("tone", "neutral");
    m_salesConfirmedBar->setProperty("tone", "revenue");
    m_salesInvoicedBar->setProperty("tone", "profit");

    auto *salesVisualsRow = new QHBoxLayout;
    salesVisualsRow->setSpacing(12);
    salesVisualsRow->addWidget(salesBarsFrame);
    salesVisualsRow->addWidget(salesFlowFrame);

    auto *salesTimelineCard = createTimelineCard(
        tr("Sales Over Time"), tr("Seven-day order activity by sales order date."),
        &m_salesTimelineChart, tr("Orders"), QStringLiteral("#16a34a"), tr("Invoiced"),
        QStringLiteral("#0f766e"), QStringLiteral("analyticsSalesTimelineChart"), salesPage);

    auto *salesCountsGrid = new QGridLayout;
    salesCountsGrid->setHorizontalSpacing(12);
    salesCountsGrid->setVerticalSpacing(12);
    salesCountsGrid->addWidget(createMetricCard(tr("Draft"), &m_salesDraftValue, salesPage), 0, 0);
    salesCountsGrid->addWidget(createMetricCard(tr("Confirmed"), &m_salesConfirmedValue, salesPage),
                               0, 1);
    salesCountsGrid->addWidget(createMetricCard(tr("Invoiced"), &m_salesInvoicedValue, salesPage),
                               0, 2);
    m_salesDraftValue->setObjectName("analyticsSalesDraftValue");
    m_salesConfirmedValue->setObjectName("analyticsSalesConfirmedValue");
    m_salesInvoicedValue->setObjectName("analyticsSalesInvoicedValue");

    m_salesTable = new QTableWidget(salesPage);
    m_salesTable->setObjectName("analyticsSalesTable");
    auto *salesTableLabel = new QLabel(tr("Sales Metrics"), salesPage);
    salesTableLabel->setObjectName("analyticsSectionLabel");
    salesLayout->addLayout(salesCardsGrid);
    salesLayout->addLayout(salesVisualsRow);
    salesLayout->addWidget(salesTimelineCard);
    salesLayout->addLayout(salesCountsGrid);
    salesLayout->addWidget(salesTableLabel);
    salesLayout->addWidget(m_salesTable);

    auto *inventoryPage = new QWidget(this);
    auto *inventoryLayout = new QVBoxLayout(inventoryPage);
    inventoryLayout->setSpacing(14);
    auto *inventoryCardsGrid = new QGridLayout;
    inventoryCardsGrid->setHorizontalSpacing(12);
    inventoryCardsGrid->setVerticalSpacing(12);
    inventoryCardsGrid->addWidget(
        createMetricCard(tr("Units Available"), &m_inventoryUnitsValue, inventoryPage), 0, 0);
    inventoryCardsGrid->addWidget(
        createMetricCard(tr("Inventory Value"), &m_inventoryValueValue, inventoryPage), 0, 1);
    inventoryCardsGrid->addWidget(
        createMetricCard(tr("Purchase Spend"), &m_inventoryPurchaseSpendValue, inventoryPage), 1,
        0);
    inventoryCardsGrid->addWidget(
        createMetricCard(tr("Product Types"), &m_inventoryProductTypesValue, inventoryPage), 1,
        1);
    m_inventoryUnitsValue->setObjectName("analyticsInventoryUnitsValue");
    m_inventoryValueValue->setObjectName("analyticsInventoryValueValue");
    m_inventoryPurchaseSpendValue->setObjectName("analyticsInventoryPurchaseSpendValue");
    m_inventoryProductTypesValue->setObjectName("analyticsInventoryProductTypesValue");

    auto *inventoryCountsGrid = new QGridLayout;
    inventoryCountsGrid->setHorizontalSpacing(12);
    inventoryCountsGrid->setVerticalSpacing(12);
    inventoryCountsGrid->addWidget(
        createMetricCard(tr("Purchase Orders"), &m_inventoryPurchaseOrdersValue, inventoryPage), 0,
        0);
    inventoryCountsGrid->addWidget(
        createMetricCard(tr("Received Orders"), &m_inventoryReceivedOrdersValue, inventoryPage), 0,
        1);
    inventoryCountsGrid->addWidget(
        createMetricCard(tr("Stock Records"), &m_inventoryStockRecordsValue, inventoryPage), 1,
        0);
    inventoryCountsGrid->addWidget(
        createMetricCard(tr("Zero Stock"), &m_inventoryZeroStockValue, inventoryPage), 1, 1);
    m_inventoryPurchaseOrdersValue->setObjectName("analyticsInventoryPurchaseOrdersValue");
    m_inventoryReceivedOrdersValue->setObjectName("analyticsInventoryReceivedOrdersValue");
    m_inventoryStockRecordsValue->setObjectName("analyticsInventoryStockRecordsValue");
    m_inventoryZeroStockValue->setObjectName("analyticsInventoryZeroStockValue");

    auto *inventoryBarsFrame = new QFrame(inventoryPage);
    inventoryBarsFrame->setObjectName("analyticsSectionCard");
    inventoryBarsFrame->setProperty("card", true);
    auto *inventoryBarsLayout = new QVBoxLayout(inventoryBarsFrame);
    auto *inventoryBarsLabel = new QLabel(tr("Goods Flow Health"), inventoryBarsFrame);
    inventoryBarsLabel->setObjectName("analyticsSectionLabel");
    inventoryBarsLayout->addWidget(inventoryBarsLabel);
    inventoryBarsLayout->addWidget(
        createProgressRow(tr("Receipt Completion"), &m_inventoryReceivedBar, inventoryBarsFrame));
    inventoryBarsLayout->addWidget(
        createProgressRow(tr("Imported Mix"), &m_inventoryImportedBar, inventoryBarsFrame));
    inventoryBarsLayout->addWidget(
        createProgressRow(tr("Stock Health"), &m_inventoryStockHealthBar, inventoryBarsFrame));
    m_inventoryReceivedBar->setObjectName("analyticsInventoryReceivedBar");
    m_inventoryImportedBar->setObjectName("analyticsInventoryImportedBar");
    m_inventoryStockHealthBar->setObjectName("analyticsInventoryStockHealthBar");
    m_inventoryReceivedBar->setProperty("tone", "profit");
    m_inventoryImportedBar->setProperty("tone", "revenue");
    m_inventoryStockHealthBar->setProperty("tone", "inventory");

    auto *inventoryFlowFrame = new QFrame(inventoryPage);
    inventoryFlowFrame->setObjectName("analyticsSectionCard");
    inventoryFlowFrame->setProperty("card", true);
    auto *inventoryFlowLayout = new QVBoxLayout(inventoryFlowFrame);
    auto *inventoryFlowLabel = new QLabel(tr("Operational Snapshot"), inventoryFlowFrame);
    inventoryFlowLabel->setObjectName("analyticsSectionLabel");
    inventoryFlowLayout->addWidget(inventoryFlowLabel);
    inventoryFlowLayout->addWidget(
        createProgressRow(tr("Purchase Orders"), &m_inventoryOrdersFlowBar, inventoryFlowFrame));
    inventoryFlowLayout->addWidget(
        createProgressRow(tr("Received Orders"), &m_inventoryReceiptsFlowBar, inventoryFlowFrame));
    inventoryFlowLayout->addWidget(
        createProgressRow(tr("Zero Stock Alerts"), &m_inventoryZeroStockFlowBar,
                          inventoryFlowFrame));
    m_inventoryOrdersFlowBar->setObjectName("analyticsInventoryOrdersFlowBar");
    m_inventoryReceiptsFlowBar->setObjectName("analyticsInventoryReceiptsFlowBar");
    m_inventoryZeroStockFlowBar->setObjectName("analyticsInventoryZeroStockFlowBar");
    m_inventoryOrdersFlowBar->setProperty("tone", "revenue");
    m_inventoryReceiptsFlowBar->setProperty("tone", "profit");
    m_inventoryZeroStockFlowBar->setProperty("tone", "danger");

    auto *inventoryVisualsRow = new QHBoxLayout;
    inventoryVisualsRow->setSpacing(12);
    inventoryVisualsRow->addWidget(inventoryBarsFrame);
    inventoryVisualsRow->addWidget(inventoryFlowFrame);

    auto *inventoryTimelineCard = createTimelineCard(
        tr("Purchasing Over Time"), tr("Seven-day order activity by purchase order date."),
        &m_inventoryTimelineChart, tr("Orders"), QStringLiteral("#16a34a"), tr("Received"),
        QStringLiteral("#0f766e"), QStringLiteral("analyticsInventoryTimelineChart"),
        inventoryPage);

    m_inventoryTable = new QTableWidget(inventoryPage);
    m_inventoryTable->setObjectName("analyticsInventoryTable");
    auto *inventoryTableLabel = new QLabel(tr("Inventory Metrics"), inventoryPage);
    inventoryTableLabel->setObjectName("analyticsSectionLabel");
    inventoryLayout->addLayout(inventoryCardsGrid);
    inventoryLayout->addLayout(inventoryVisualsRow);
    inventoryLayout->addWidget(inventoryTimelineCard);
    inventoryLayout->addLayout(inventoryCountsGrid);
    inventoryLayout->addWidget(inventoryTableLabel);
    inventoryLayout->addWidget(m_inventoryTable);

    for (auto *table : {m_salesTable, m_inventoryTable}) {
        table->setSelectionMode(QAbstractItemView::NoSelection);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setFocusPolicy(Qt::NoFocus);
        table->verticalHeader()->setVisible(false);
        table->verticalHeader()->setDefaultSectionSize(28);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setMaximumHeight(260);
    }

    m_tabs->addTab(salesPage, tr("Sales"));
    m_tabs->addTab(inventoryPage, tr("Inventory"));

    layout->addWidget(titleLabel);
    layout->addWidget(summaryLabel);
    layout->addWidget(m_tabs);

    setObjectName("analyticsView");
    setStyleSheet(
        "#analyticsView { background-color: transparent; }"
        "#analyticsTitleLabel { color: #0f172a; font-size: 24px; font-weight: 700; }"
        "#analyticsSummaryLabel { color: #64748b; }"
        "#analyticsMetricCard, #analyticsSectionCard { background-color: #ffffff; border: 1px solid #dbe3ee; border-radius: 16px; }"
        "#analyticsCardTitle, #analyticsSectionLabel { color: #334155; font-weight: 700; }"
        "#analyticsCardValue { color: #0f172a; font-size: 20px; font-weight: 700; }"
        "QProgressBar[tone='revenue']::chunk { background-color: #2563eb; }"
        "QProgressBar[tone='cost']::chunk { background-color: #f59e0b; }"
        "QProgressBar[tone='profit']::chunk { background-color: #16a34a; }"
        "QProgressBar[tone='inventory']::chunk { background-color: #0f766e; }"
        "QProgressBar[tone='neutral']::chunk { background-color: #64748b; }"
        "QProgressBar[tone='danger']::chunk { background-color: #e11d48; }");
}

void AnalyticsView::fillSalesMetrics()
{
    const double revenue = m_viewModel.salesMetricNumber(AnalyticsKeys::Sales::TOTAL_REVENUE);
    const double purchaseCost =
        m_viewModel.salesMetricNumber(AnalyticsKeys::Sales::TOTAL_PURCHASE_COST);
    const double grossProfit = m_viewModel.salesMetricNumber(AnalyticsKeys::Sales::GROSS_PROFIT);
    const double draftOrders =
        m_viewModel.salesMetricNumber(AnalyticsKeys::Sales::DRAFT_ORDERS);
    const double confirmedOrders =
        m_viewModel.salesMetricNumber(AnalyticsKeys::Sales::CONFIRMED_ORDERS);
    const double invoicedOrders =
        m_viewModel.salesMetricNumber(AnalyticsKeys::Sales::INVOICED_ORDERS);
    const double maxValue = std::max({1.0, revenue, purchaseCost, std::fabs(grossProfit)});
    const double pipelineMax =
        std::max({1.0, draftOrders, confirmedOrders, invoicedOrders});

    m_salesRevenueValue->setText(
        m_viewModel.salesMetricValue(AnalyticsKeys::Sales::TOTAL_REVENUE));
    m_salesProfitValue->setText(
        m_viewModel.salesMetricValue(AnalyticsKeys::Sales::GROSS_PROFIT));
    m_salesMarginValue->setText(
        m_viewModel.salesMetricValue(AnalyticsKeys::Sales::GROSS_MARGIN_PERCENT));
    m_salesCustomersValue->setText(
        m_viewModel.salesMetricValue(AnalyticsKeys::Sales::UNIQUE_CUSTOMERS));
    m_salesDraftValue->setText(
        m_viewModel.salesMetricValue(AnalyticsKeys::Sales::DRAFT_ORDERS));
    m_salesConfirmedValue->setText(
        m_viewModel.salesMetricValue(AnalyticsKeys::Sales::CONFIRMED_ORDERS));
    m_salesInvoicedValue->setText(
        m_viewModel.salesMetricValue(AnalyticsKeys::Sales::INVOICED_ORDERS));

    updateScaledBar(m_salesRevenueBar, revenue, maxValue,
                    m_viewModel.salesMetricValue(AnalyticsKeys::Sales::TOTAL_REVENUE));
    updateScaledBar(m_salesCostBar, purchaseCost, maxValue,
                    m_viewModel.salesMetricValue(AnalyticsKeys::Sales::TOTAL_PURCHASE_COST));
    updateScaledBar(m_salesProfitBar, grossProfit, maxValue,
                    m_viewModel.salesMetricValue(AnalyticsKeys::Sales::GROSS_PROFIT));
    updateScaledBar(m_salesDraftBar, draftOrders, pipelineMax,
                    m_viewModel.salesMetricValue(AnalyticsKeys::Sales::DRAFT_ORDERS));
    updateScaledBar(m_salesConfirmedBar, confirmedOrders, pipelineMax,
                    m_viewModel.salesMetricValue(AnalyticsKeys::Sales::CONFIRMED_ORDERS));
    updateScaledBar(m_salesInvoicedBar, invoicedOrders, pipelineMax,
                    m_viewModel.salesMetricValue(AnalyticsKeys::Sales::INVOICED_ORDERS));
    fillTable(m_salesTable, m_viewModel.salesMetrics());
}

void AnalyticsView::fillInventoryMetrics()
{
    const double totalPurchaseOrders =
        m_viewModel.inventoryMetricNumber(AnalyticsKeys::Inventory::TOTAL_PURCHASE_ORDERS);
    const double receivedPurchaseOrders =
        m_viewModel.inventoryMetricNumber(AnalyticsKeys::Inventory::RECEIVED_PURCHASE_ORDERS);
    const double totalProductTypes =
        m_viewModel.inventoryMetricNumber(AnalyticsKeys::Inventory::TOTAL_PRODUCT_TYPES);
    const double importedProductTypes =
        m_viewModel.inventoryMetricNumber(AnalyticsKeys::Inventory::IMPORTED_PRODUCT_TYPES);
    const double totalStockRecords =
        m_viewModel.inventoryMetricNumber(AnalyticsKeys::Inventory::TOTAL_STOCK_RECORDS);
    const double zeroStockRecords =
        m_viewModel.inventoryMetricNumber(AnalyticsKeys::Inventory::ZERO_STOCK_RECORDS);
    const double flowMax =
        std::max({1.0, totalPurchaseOrders, receivedPurchaseOrders, zeroStockRecords});

    m_inventoryUnitsValue->setText(
        m_viewModel.inventoryMetricValue(AnalyticsKeys::Inventory::TOTAL_UNITS_AVAILABLE));
    m_inventoryValueValue->setText(
        m_viewModel.inventoryMetricValue(AnalyticsKeys::Inventory::INVENTORY_VALUE_ESTIMATE));
    m_inventoryPurchaseSpendValue->setText(
        m_viewModel.inventoryMetricValue(AnalyticsKeys::Inventory::TOTAL_PURCHASE_SPEND));
    m_inventoryProductTypesValue->setText(
        m_viewModel.inventoryMetricValue(AnalyticsKeys::Inventory::TOTAL_PRODUCT_TYPES));
    m_inventoryPurchaseOrdersValue->setText(
        m_viewModel.inventoryMetricValue(AnalyticsKeys::Inventory::TOTAL_PURCHASE_ORDERS));
    m_inventoryReceivedOrdersValue->setText(
        m_viewModel.inventoryMetricValue(AnalyticsKeys::Inventory::RECEIVED_PURCHASE_ORDERS));
    m_inventoryStockRecordsValue->setText(
        m_viewModel.inventoryMetricValue(AnalyticsKeys::Inventory::TOTAL_STOCK_RECORDS));
    m_inventoryZeroStockValue->setText(
        m_viewModel.inventoryMetricValue(AnalyticsKeys::Inventory::ZERO_STOCK_RECORDS));

    const double receiptRatio =
        totalPurchaseOrders <= 0.0 ? 0.0 : (receivedPurchaseOrders / totalPurchaseOrders) * 100.0;
    const double importedRatio =
        totalProductTypes <= 0.0 ? 0.0 : (importedProductTypes / totalProductTypes) * 100.0;
    const double stockHealth =
        totalStockRecords <= 0.0 ? 0.0
                                 : ((totalStockRecords - zeroStockRecords) / totalStockRecords) *
                                       100.0;

    updateRatioBar(m_inventoryReceivedBar, receiptRatio,
                   tr("%1% complete").arg(QString::number(receiptRatio, 'f', 1)));
    updateRatioBar(m_inventoryImportedBar, importedRatio,
                   tr("%1% imported").arg(QString::number(importedRatio, 'f', 1)));
    updateRatioBar(m_inventoryStockHealthBar, stockHealth,
                   tr("%1% healthy").arg(QString::number(stockHealth, 'f', 1)));
    updateScaledBar(m_inventoryOrdersFlowBar, totalPurchaseOrders, flowMax,
                    m_viewModel.inventoryMetricValue(AnalyticsKeys::Inventory::TOTAL_PURCHASE_ORDERS));
    updateScaledBar(
        m_inventoryReceiptsFlowBar, receivedPurchaseOrders, flowMax,
        m_viewModel.inventoryMetricValue(AnalyticsKeys::Inventory::RECEIVED_PURCHASE_ORDERS));
    updateScaledBar(m_inventoryZeroStockFlowBar, zeroStockRecords, flowMax,
                    m_viewModel.inventoryMetricValue(AnalyticsKeys::Inventory::ZERO_STOCK_RECORDS));
    fillTable(m_inventoryTable, m_viewModel.inventoryMetrics());
}

void AnalyticsView::refreshSalesTimeline()
{
    if (m_salesTimelineChart == nullptr) {
        return;
    }

    m_salesTimelineChart->setSeries(
        tr("Orders"), QColor("#16a34a"), tr("Invoiced"), QColor("#0f766e"),
        buildOrderTimeline(m_viewModel.salesOrders(), [](const Common::Entities::SaleOrder &order) {
            return canonicalSalesStatus(order.status) == QStringLiteral("Invoiced");
        }));
}

void AnalyticsView::refreshInventoryTimeline()
{
    if (m_inventoryTimelineChart == nullptr) {
        return;
    }

    m_inventoryTimelineChart->setSeries(
        tr("Orders"), QColor("#16a34a"), tr("Received"), QColor("#0f766e"),
        buildOrderTimeline(
            m_viewModel.purchaseOrders(), [](const Common::Entities::PurchaseOrder &order) {
                return canonicalPurchaseStatus(order.status) == QStringLiteral("Received");
            }));
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

void AnalyticsView::updateScaledBar(QProgressBar *bar, double value, double maxValue,
                                    const QString &text) const
{
    const int scaledValue = maxValue <= 0.0
                                ? 0
                                : static_cast<int>(std::round(std::max(0.0, value) / maxValue * 100.0));
    bar->setRange(0, 100);
    bar->setValue(scaledValue);
    bar->setFormat(text);
}

void AnalyticsView::updateRatioBar(QProgressBar *bar, double ratio, const QString &text) const
{
    const auto clampedRatio =
        std::clamp(static_cast<int>(std::round(ratio)), 0, 100);
    bar->setRange(0, 100);
    bar->setValue(clampedRatio);
    bar->setFormat(text);
}
