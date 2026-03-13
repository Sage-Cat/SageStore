#include "AnalyticsModule.hpp"

#include <unordered_set>

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
} // namespace

AnalyticsModule::AnalyticsModule(RepositoryManager &repositoryManager)
{
    m_salesOrdersRepository       = std::move(repositoryManager.getSaleOrderRepository());
    m_salesOrderRecordsRepository = std::move(repositoryManager.getSalesOrderRecordRepository());
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
    const auto orders  = m_salesOrdersRepository->getAll();
    const auto records = m_salesOrderRecordsRepository->getAll();

    double totalRevenue = 0.0;
    for (const auto &record : records) {
        totalRevenue += parseDouble(record.price) * static_cast<double>(parseInteger(record.quantity));
    }

    std::unordered_set<std::string> uniqueCustomers;
    for (const auto &order : orders) {
        if (!order.contactId.empty()) {
            uniqueCustomers.insert(order.contactId);
        }
    }

    const double averageOrderValue =
        orders.empty() ? 0.0 : totalRevenue / static_cast<double>(orders.size());

    response.dataset[AnalyticsKeys::Sales::TOTAL_ORDERS] = {
        std::to_string(static_cast<long long>(orders.size()))};
    response.dataset[AnalyticsKeys::Sales::TOTAL_ORDER_LINES] = {
        std::to_string(static_cast<long long>(records.size()))};
    response.dataset[AnalyticsKeys::Sales::TOTAL_REVENUE] = {std::to_string(totalRevenue)};
    response.dataset[AnalyticsKeys::Sales::AVERAGE_ORDER_VALUE] = {std::to_string(averageOrderValue)};
    response.dataset[AnalyticsKeys::Sales::UNIQUE_CUSTOMERS] = {
        std::to_string(static_cast<long long>(uniqueCustomers.size()))};
    return response;
}

ResponseData AnalyticsModule::getInventoryAnalytics() const
{
    ResponseData response;
    const auto inventoryRecords = m_inventoryRepository->getAll();
    const auto productTypes     = m_productTypesRepository->getAll();

    long long totalUnits           = 0;
    long long zeroStockRecords     = 0;
    long long importedProductTypes = 0;

    for (const auto &stock : inventoryRecords) {
        const auto quantity = parseInteger(stock.quantityAvailable);
        totalUnits += quantity;
        if (quantity == 0) {
            ++zeroStockRecords;
        }
    }

    for (const auto &productType : productTypes) {
        if (productType.isImported == "1" || productType.isImported == "true" ||
            productType.isImported == "True") {
            ++importedProductTypes;
        }
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
    return response;
}
