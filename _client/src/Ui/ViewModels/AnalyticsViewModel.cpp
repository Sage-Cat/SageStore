#include "Ui/ViewModels/AnalyticsViewModel.hpp"

#include <algorithm>
#include <QSet>

#include "common/AnalyticsKeys.hpp"

#include "Ui/Models/AnalyticsModel.hpp"

AnalyticsViewModel::AnalyticsViewModel(AnalyticsModel &model, QObject *parent)
    : BaseViewModel(model, parent), m_model(model)
{
    connect(&m_model, &AnalyticsModel::salesMetricsChanged, this,
            &AnalyticsViewModel::salesMetricsChanged);
    connect(&m_model, &AnalyticsModel::inventoryMetricsChanged, this,
            &AnalyticsViewModel::inventoryMetricsChanged);
    connect(&m_model, &AnalyticsModel::salesOrdersChanged, this, &AnalyticsViewModel::salesOrdersChanged);
    connect(&m_model, &AnalyticsModel::purchaseOrdersChanged, this,
            &AnalyticsViewModel::purchaseOrdersChanged);
    connect(&m_model, &AnalyticsModel::errorOccurred, this,
            &AnalyticsViewModel::errorOccurred);
}

QVector<QPair<QString, QString>> AnalyticsViewModel::salesMetrics() const
{
    return normalizeMetrics(
        m_model.salesMetrics(),
        {AnalyticsKeys::Sales::TOTAL_ORDERS, AnalyticsKeys::Sales::TOTAL_ORDER_LINES,
         AnalyticsKeys::Sales::UNIQUE_CUSTOMERS, AnalyticsKeys::Sales::TOTAL_REVENUE,
         AnalyticsKeys::Sales::TOTAL_PURCHASE_COST, AnalyticsKeys::Sales::GROSS_PROFIT,
         AnalyticsKeys::Sales::GROSS_MARGIN_PERCENT, AnalyticsKeys::Sales::AVERAGE_ORDER_VALUE,
         AnalyticsKeys::Sales::DRAFT_ORDERS, AnalyticsKeys::Sales::CONFIRMED_ORDERS,
         AnalyticsKeys::Sales::INVOICED_ORDERS});
}

QVector<QPair<QString, QString>> AnalyticsViewModel::inventoryMetrics() const
{
    return normalizeMetrics(
        m_model.inventoryMetrics(),
        {AnalyticsKeys::Inventory::TOTAL_PRODUCT_TYPES,
         AnalyticsKeys::Inventory::TOTAL_STOCK_RECORDS,
         AnalyticsKeys::Inventory::TOTAL_UNITS_AVAILABLE,
         AnalyticsKeys::Inventory::ZERO_STOCK_RECORDS,
         AnalyticsKeys::Inventory::IMPORTED_PRODUCT_TYPES,
         AnalyticsKeys::Inventory::TOTAL_PURCHASE_ORDERS,
         AnalyticsKeys::Inventory::OPEN_PURCHASE_ORDERS,
         AnalyticsKeys::Inventory::RECEIVED_PURCHASE_ORDERS,
         AnalyticsKeys::Inventory::TOTAL_PURCHASE_SPEND,
         AnalyticsKeys::Inventory::INVENTORY_VALUE_ESTIMATE});
}

void AnalyticsViewModel::fetchAll() { m_model.fetchAll(); }

QString AnalyticsViewModel::salesMetricValue(const QString &key) const
{
    return formatMetricValue(key, m_model.salesMetrics().value(key));
}

QString AnalyticsViewModel::inventoryMetricValue(const QString &key) const
{
    return formatMetricValue(key, m_model.inventoryMetrics().value(key));
}

double AnalyticsViewModel::salesMetricNumber(const QString &key) const
{
    return metricNumber(m_model.salesMetrics(), key);
}

double AnalyticsViewModel::inventoryMetricNumber(const QString &key) const
{
    return metricNumber(m_model.inventoryMetrics(), key);
}

const std::vector<Common::Entities::SaleOrder> &AnalyticsViewModel::salesOrders() const
{
    return m_model.salesOrders();
}

const std::vector<Common::Entities::PurchaseOrder> &AnalyticsViewModel::purchaseOrders() const
{
    return m_model.purchaseOrders();
}

QVector<QPair<QString, QString>> AnalyticsViewModel::normalizeMetrics(
    const QVariantMap &metrics, const QVector<QString> &orderedKeys) const
{
    QVector<QPair<QString, QString>> rows;
    QSet<QString> seenKeys;

    for (const auto &key : orderedKeys) {
        if (!metrics.contains(key)) {
            continue;
        }
        rows.push_back({labelForMetric(key), formatMetricValue(key, metrics.value(key))});
        seenKeys.insert(key);
    }

    QVector<QString> extraKeys;
    for (auto it = metrics.cbegin(); it != metrics.cend(); ++it) {
        if (!seenKeys.contains(it.key())) {
            extraKeys.push_back(it.key());
        }
    }

    std::sort(extraKeys.begin(), extraKeys.end());
    for (const auto &key : extraKeys) {
        rows.push_back({labelForMetric(key), formatMetricValue(key, metrics.value(key))});
    }

    return rows;
}

QString AnalyticsViewModel::labelForMetric(const QString &key) const
{
    if (key == AnalyticsKeys::Sales::TOTAL_ORDERS) {
        return tr("Total orders");
    }
    if (key == AnalyticsKeys::Sales::TOTAL_ORDER_LINES) {
        return tr("Total order lines");
    }
    if (key == AnalyticsKeys::Sales::UNIQUE_CUSTOMERS) {
        return tr("Unique customers");
    }
    if (key == AnalyticsKeys::Sales::TOTAL_REVENUE) {
        return tr("Revenue");
    }
    if (key == AnalyticsKeys::Sales::TOTAL_PURCHASE_COST) {
        return tr("Purchase cost");
    }
    if (key == AnalyticsKeys::Sales::GROSS_PROFIT) {
        return tr("Gross profit");
    }
    if (key == AnalyticsKeys::Sales::GROSS_MARGIN_PERCENT) {
        return tr("Gross margin");
    }
    if (key == AnalyticsKeys::Sales::AVERAGE_ORDER_VALUE) {
        return tr("Average order value");
    }
    if (key == AnalyticsKeys::Sales::DRAFT_ORDERS) {
        return tr("Draft orders");
    }
    if (key == AnalyticsKeys::Sales::CONFIRMED_ORDERS) {
        return tr("Confirmed orders");
    }
    if (key == AnalyticsKeys::Sales::INVOICED_ORDERS) {
        return tr("Invoiced orders");
    }
    if (key == AnalyticsKeys::Inventory::TOTAL_PRODUCT_TYPES) {
        return tr("Product types");
    }
    if (key == AnalyticsKeys::Inventory::TOTAL_STOCK_RECORDS) {
        return tr("Stock records");
    }
    if (key == AnalyticsKeys::Inventory::TOTAL_UNITS_AVAILABLE) {
        return tr("Units available");
    }
    if (key == AnalyticsKeys::Inventory::ZERO_STOCK_RECORDS) {
        return tr("Zero-stock records");
    }
    if (key == AnalyticsKeys::Inventory::IMPORTED_PRODUCT_TYPES) {
        return tr("Imported product types");
    }
    if (key == AnalyticsKeys::Inventory::TOTAL_PURCHASE_ORDERS) {
        return tr("Purchase orders");
    }
    if (key == AnalyticsKeys::Inventory::OPEN_PURCHASE_ORDERS) {
        return tr("Open purchase orders");
    }
    if (key == AnalyticsKeys::Inventory::RECEIVED_PURCHASE_ORDERS) {
        return tr("Received purchase orders");
    }
    if (key == AnalyticsKeys::Inventory::TOTAL_PURCHASE_SPEND) {
        return tr("Purchase spend");
    }
    if (key == AnalyticsKeys::Inventory::INVENTORY_VALUE_ESTIMATE) {
        return tr("Inventory value");
    }

    return key;
}

QString AnalyticsViewModel::formatMetricValue(const QString &key, const QVariant &value) const
{
    if (!value.isValid() || value.isNull()) {
        return QStringLiteral("0");
    }

    static const QSet<QString> moneyKeys = {
        AnalyticsKeys::Sales::TOTAL_REVENUE, AnalyticsKeys::Sales::TOTAL_PURCHASE_COST,
        AnalyticsKeys::Sales::GROSS_PROFIT, AnalyticsKeys::Sales::AVERAGE_ORDER_VALUE,
        AnalyticsKeys::Inventory::TOTAL_PURCHASE_SPEND,
        AnalyticsKeys::Inventory::INVENTORY_VALUE_ESTIMATE};
    if (moneyKeys.contains(key)) {
        return QString::number(value.toDouble(), 'f', 2);
    }

    if (key == AnalyticsKeys::Sales::GROSS_MARGIN_PERCENT) {
        return tr("%1%").arg(QString::number(value.toDouble(), 'f', 1));
    }

    bool integerOk = false;
    const auto integerValue = value.toLongLong(&integerOk);
    if (integerOk) {
        return QString::number(integerValue);
    }

    return value.toString();
}

double AnalyticsViewModel::metricNumber(const QVariantMap &metrics, const QString &key) const
{
    return metrics.value(key).toDouble();
}
