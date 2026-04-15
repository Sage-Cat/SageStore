#pragma once

namespace AnalyticsKeys {
namespace Sales {
inline constexpr char TOTAL_ORDERS[]       = "totalOrders";
inline constexpr char TOTAL_ORDER_LINES[]  = "totalOrderLines";
inline constexpr char TOTAL_REVENUE[]       = "totalRevenue";
inline constexpr char AVERAGE_ORDER_VALUE[] = "averageOrderValue";
inline constexpr char UNIQUE_CUSTOMERS[]    = "uniqueCustomers";
inline constexpr char TOTAL_PURCHASE_COST[] = "totalPurchaseCost";
inline constexpr char GROSS_PROFIT[]        = "grossProfit";
inline constexpr char GROSS_MARGIN_PERCENT[] = "grossMarginPercent";
inline constexpr char DRAFT_ORDERS[]        = "draftOrders";
inline constexpr char CONFIRMED_ORDERS[]    = "confirmedOrders";
inline constexpr char INVOICED_ORDERS[]     = "invoicedOrders";
} // namespace Sales

namespace Inventory {
inline constexpr char TOTAL_PRODUCT_TYPES[]   = "totalProductTypes";
inline constexpr char TOTAL_STOCK_RECORDS[]   = "totalStockRecords";
inline constexpr char TOTAL_UNITS_AVAILABLE[] = "totalUnitsAvailable";
inline constexpr char IMPORTED_PRODUCT_TYPES[] = "importedProductTypes";
inline constexpr char ZERO_STOCK_RECORDS[]    = "zeroStockRecords";
inline constexpr char TOTAL_PURCHASE_ORDERS[] = "totalPurchaseOrders";
inline constexpr char OPEN_PURCHASE_ORDERS[]  = "openPurchaseOrders";
inline constexpr char RECEIVED_PURCHASE_ORDERS[] = "receivedPurchaseOrders";
inline constexpr char TOTAL_PURCHASE_SPEND[]  = "totalPurchaseSpend";
inline constexpr char INVENTORY_VALUE_ESTIMATE[] = "inventoryValueEstimate";
} // namespace Inventory
} // namespace AnalyticsKeys
