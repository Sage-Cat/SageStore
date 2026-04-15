#include "AnalyticsModule.hpp"

#include <cmath>
#include <unordered_set>
#include <unordered_map>

#include "Database/RepositoryManager.hpp"
#include "ServerException.hpp"

#include "common/AnalyticsKeys.hpp"

#define _M "AnalyticsModule"

namespace {
double parseDouble(const std::string &value)
{
    try {
        return std::stod(value);
    } catch (...) {
        return 0.0;
    }
}

long long parseInteger(const std::string &value)
{
    try {
        return std::stoll(value);
    } catch (...) {
        return 0;
    }
}

std::string trimValue(std::string value)
{
    const auto first = value.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos) {
        return {};
    }

    const auto last = value.find_last_not_of(" \t\n\r\f\v");
    return value.substr(first, last - first + 1);
}

std::string normalizeSalesStatus(const std::string &status)
{
    const auto normalized = trimValue(status);
    if (normalized == "Draft" || normalized == "Чернетка") {
        return "Draft";
    }
    if (normalized == "Confirmed" || normalized == "Підтверджено") {
        return "Confirmed";
    }
    if (normalized == "Invoiced" || normalized == "Issued" ||
        normalized == "Виставлено рахунок") {
        return "Invoiced";
    }

    return normalized;
}

std::string normalizePurchaseStatus(const std::string &status)
{
    const auto normalized = trimValue(status);
    if (normalized == "Received" || normalized == "Отримано") {
        return "Received";
    }
    if (normalized == "Ordered" || normalized == "Pending" || normalized == "Замовлено") {
        return "Ordered";
    }
    if (normalized == "Draft" || normalized == "Чернетка") {
        return "Draft";
    }

    return normalized;
}
} // namespace

AnalyticsModule::AnalyticsModule(RepositoryManager &repositoryManager)
{
    m_salesOrdersRepository       = std::move(repositoryManager.getSaleOrderRepository());
    m_salesOrderRecordsRepository = std::move(repositoryManager.getSalesOrderRecordRepository());
    m_purchaseOrdersRepository    = std::move(repositoryManager.getPurchaseOrderRepository());
    m_purchaseOrderRecordsRepository =
        std::move(repositoryManager.getPurchaseOrderRecordRepository());
    m_inventoryRepository         = std::move(repositoryManager.getInventoryRepository());
    m_productTypesRepository      = std::move(repositoryManager.getProductTypeRepository());
}

AnalyticsModule::~AnalyticsModule() = default;

ResponseData AnalyticsModule::executeTask(const RequestData &requestData)
{
    if (requestData.method != "GET") {
        throw ServerException(_M, "Analytics endpoints are read-only");
    }

    if (requestData.submodule == "sales") {
        return getSalesAnalytics();
    }

    if (requestData.submodule == "inventory") {
        return getInventoryAnalytics();
    }

    throw ServerException(_M, "Unrecognized analytics task");
}

ResponseData AnalyticsModule::getSalesAnalytics() const
{
    ResponseData response;
    const auto orders           = m_salesOrdersRepository->getAll();
    const auto salesRecords     = m_salesOrderRecordsRepository->getAll();
    const auto purchaseRecords  = m_purchaseOrderRecordsRepository->getAll();

    double totalRevenue = 0.0;
    for (const auto &record : salesRecords) {
        totalRevenue += parseDouble(record.price) * static_cast<double>(parseInteger(record.quantity));
    }

    double totalPurchaseCost = 0.0;
    for (const auto &record : purchaseRecords) {
        totalPurchaseCost +=
            parseDouble(record.price) * static_cast<double>(parseInteger(record.quantity));
    }

    std::unordered_set<std::string> uniqueCustomers;
    long long draftOrders = 0;
    long long confirmedOrders = 0;
    long long invoicedOrders = 0;
    for (const auto &order : orders) {
        if (!order.contactId.empty()) {
            uniqueCustomers.insert(order.contactId);
        }

        const auto normalizedStatus = normalizeSalesStatus(order.status);
        if (normalizedStatus == "Draft") {
            ++draftOrders;
        } else if (normalizedStatus == "Confirmed") {
            ++confirmedOrders;
        } else if (normalizedStatus == "Invoiced") {
            ++invoicedOrders;
        }
    }

    const double averageOrderValue =
        orders.empty() ? 0.0 : totalRevenue / static_cast<double>(orders.size());
    const double grossProfit = totalRevenue - totalPurchaseCost;
    const double grossMarginPercent =
        totalRevenue <= 0.0 ? 0.0 : (grossProfit / totalRevenue) * 100.0;

    response.dataset[AnalyticsKeys::Sales::TOTAL_ORDERS] = {
        std::to_string(static_cast<long long>(orders.size()))};
    response.dataset[AnalyticsKeys::Sales::TOTAL_ORDER_LINES] = {
        std::to_string(static_cast<long long>(salesRecords.size()))};
    response.dataset[AnalyticsKeys::Sales::TOTAL_REVENUE] = {std::to_string(totalRevenue)};
    response.dataset[AnalyticsKeys::Sales::AVERAGE_ORDER_VALUE] = {std::to_string(averageOrderValue)};
    response.dataset[AnalyticsKeys::Sales::UNIQUE_CUSTOMERS] = {
        std::to_string(static_cast<long long>(uniqueCustomers.size()))};
    response.dataset[AnalyticsKeys::Sales::TOTAL_PURCHASE_COST] = {
        std::to_string(totalPurchaseCost)};
    response.dataset[AnalyticsKeys::Sales::GROSS_PROFIT] = {std::to_string(grossProfit)};
    response.dataset[AnalyticsKeys::Sales::GROSS_MARGIN_PERCENT] = {
        std::to_string(grossMarginPercent)};
    response.dataset[AnalyticsKeys::Sales::DRAFT_ORDERS] = {std::to_string(draftOrders)};
    response.dataset[AnalyticsKeys::Sales::CONFIRMED_ORDERS] = {std::to_string(confirmedOrders)};
    response.dataset[AnalyticsKeys::Sales::INVOICED_ORDERS] = {std::to_string(invoicedOrders)};
    return response;
}

ResponseData AnalyticsModule::getInventoryAnalytics() const
{
    ResponseData response;
    const auto inventoryRecords = m_inventoryRepository->getAll();
    const auto productTypes     = m_productTypesRepository->getAll();
    const auto purchaseOrders   = m_purchaseOrdersRepository->getAll();
    const auto purchaseRecords  = m_purchaseOrderRecordsRepository->getAll();

    std::unordered_map<std::string, double> productCosts;
    for (const auto &productType : productTypes) {
        productCosts.emplace(productType.id, parseDouble(productType.lastPrice));
    }

    long long totalUnits            = 0;
    long long zeroStockRecords      = 0;
    long long importedProductTypes  = 0;
    double inventoryValueEstimate   = 0.0;

    for (const auto &stock : inventoryRecords) {
        const auto quantity = parseInteger(stock.quantityAvailable);
        totalUnits += quantity;
        if (quantity == 0) {
            ++zeroStockRecords;
        }

        const auto costIt = productCosts.find(stock.productTypeId);
        if (costIt != productCosts.end()) {
            inventoryValueEstimate += static_cast<double>(quantity) * costIt->second;
        }
    }

    for (const auto &productType : productTypes) {
        if (productType.isImported == "1" || productType.isImported == "true" ||
            productType.isImported == "True") {
            ++importedProductTypes;
        }
    }

    long long receivedPurchaseOrders = 0;
    for (const auto &order : purchaseOrders) {
        if (normalizePurchaseStatus(order.status) == "Received") {
            ++receivedPurchaseOrders;
        }
    }

    double totalPurchaseSpend = 0.0;
    for (const auto &record : purchaseRecords) {
        totalPurchaseSpend +=
            parseDouble(record.price) * static_cast<double>(parseInteger(record.quantity));
    }

    response.dataset[AnalyticsKeys::Inventory::TOTAL_PRODUCT_TYPES] = {
        std::to_string(static_cast<long long>(productTypes.size()))};
    response.dataset[AnalyticsKeys::Inventory::TOTAL_STOCK_RECORDS] = {
        std::to_string(static_cast<long long>(inventoryRecords.size()))};
    response.dataset[AnalyticsKeys::Inventory::TOTAL_UNITS_AVAILABLE] = {std::to_string(totalUnits)};
    response.dataset[AnalyticsKeys::Inventory::IMPORTED_PRODUCT_TYPES] = {
        std::to_string(importedProductTypes)};
    response.dataset[AnalyticsKeys::Inventory::ZERO_STOCK_RECORDS] = {
        std::to_string(zeroStockRecords)};
    response.dataset[AnalyticsKeys::Inventory::TOTAL_PURCHASE_ORDERS] = {
        std::to_string(static_cast<long long>(purchaseOrders.size()))};
    response.dataset[AnalyticsKeys::Inventory::OPEN_PURCHASE_ORDERS] = {
        std::to_string(static_cast<long long>(purchaseOrders.size()) - receivedPurchaseOrders)};
    response.dataset[AnalyticsKeys::Inventory::RECEIVED_PURCHASE_ORDERS] = {
        std::to_string(receivedPurchaseOrders)};
    response.dataset[AnalyticsKeys::Inventory::TOTAL_PURCHASE_SPEND] = {
        std::to_string(totalPurchaseSpend)};
    response.dataset[AnalyticsKeys::Inventory::INVENTORY_VALUE_ESTIMATE] = {
        std::to_string(inventoryValueEstimate)};
    return response;
}
