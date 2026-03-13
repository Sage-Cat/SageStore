#pragma once

namespace AnalyticsKeys {
namespace Sales {
inline constexpr char TOTAL_ORDERS[]       = "totalOrders";
inline constexpr char TOTAL_ORDER_LINES[]  = "totalOrderLines";
inline constexpr char TOTAL_REVENUE[]      = "totalRevenue";
inline constexpr char AVERAGE_ORDER_VALUE[] = "averageOrderValue";
inline constexpr char UNIQUE_CUSTOMERS[]   = "uniqueCustomers";
} // namespace Sales

namespace Inventory {
inline constexpr char TOTAL_PRODUCT_TYPES[] = "totalProductTypes";
inline constexpr char TOTAL_STOCK_RECORDS[] = "totalStockRecords";
inline constexpr char TOTAL_UNITS_AVAILABLE[] = "totalUnitsAvailable";
inline constexpr char IMPORTED_PRODUCT_TYPES[] = "importedProductTypes";
inline constexpr char ZERO_STOCK_RECORDS[] = "zeroStockRecords";
} // namespace Inventory
} // namespace AnalyticsKeys
