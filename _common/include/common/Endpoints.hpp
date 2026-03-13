#pragma once

namespace Endpoints {
namespace Segments {
inline constexpr int ROOT        = 0;
inline constexpr int MODULE      = 1;
inline constexpr int SUBMODULE   = 2;
inline constexpr int RESOURCE_ID = 3;
}; // namespace Segments

namespace Users {
inline constexpr char LOGIN[]        = "/api/users/login";
inline constexpr char USERS[]        = "/api/users/users";
inline constexpr char ROLES[]        = "/api/users/roles";
} // namespace Users

namespace Inventory {
inline constexpr char PRODUCT_TYPES[]    = "/api/inventory/product-types";
inline constexpr char STOCKS[]           = "/api/inventory/stocks";
inline constexpr char SUPPLIER_PRODUCTS[] = "/api/inventory/supplier-products";
} // namespace Inventory

namespace Management {
inline constexpr char CONTACTS[]  = "/api/management/contacts";
inline constexpr char SUPPLIERS[] = "/api/management/suppliers";
inline constexpr char EMPLOYEES[] = "/api/management/employees";
} // namespace Management

namespace Purchase {
inline constexpr char ORDERS[]       = "/api/purchase/orders";
inline constexpr char ORDER_RECORDS[] = "/api/purchase/order-records";
inline constexpr char RECEIPTS[]      = "/api/purchase/receipts";
} // namespace Purchase

namespace Sales {
inline constexpr char ORDERS[]       = "/api/sales/orders";
inline constexpr char ORDER_RECORDS[] = "/api/sales/order-records";
} // namespace Sales

namespace Analytics {
inline constexpr char SALES[]     = "/api/analytics/sales";
inline constexpr char INVENTORY[] = "/api/analytics/inventory";
} // namespace Analytics

namespace Logs {
inline constexpr char ENTRIES[] = "/api/logs/entries";
} // namespace Logs
} // namespace Endpoints
