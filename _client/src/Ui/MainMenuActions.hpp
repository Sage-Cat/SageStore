#pragma once

#include <QCoreApplication>

#include <map>
#include <string>

namespace MainMenuActions {

enum class Type {
    SETTINGS,
    EXIT,
    PURCHASE_ORDERS,
    SUPPLIER_MANAGEMENT,
    GOODS_RECEIPTS,
    SALES_ORDERS,
    CUSTOMER_MANAGEMENT,
    INVOICING,
    PRODUCT_MANAGEMENT,
    SUPPLIER_PRICELIST_UPLOAD,
    STOCK_TRACKING,
    SALES_ANALYTICS,
    INVENTORY_ANALYTICS,
    USERS,
    USER_ROLES,
    USER_LOGS,
    EMPLOYEES,
    CUSTOMERS,
    SUPPLIERS
};

inline const std::map<Type, const char *> SOURCE_TEXTS = {
    {Type::SETTINGS, "Settings"},
    {Type::EXIT, "Exit"},
    {Type::PURCHASE_ORDERS, "Purchase Orders"},
    {Type::SUPPLIER_MANAGEMENT, "Supplier Management"},
    {Type::GOODS_RECEIPTS, "Goods Receipts"},
    {Type::SALES_ORDERS, "Sales Orders"},
    {Type::CUSTOMER_MANAGEMENT, "Customer Management"},
    {Type::INVOICING, "Invoicing"},
    {Type::PRODUCT_MANAGEMENT, "Product Management"},
    {Type::SUPPLIER_PRICELIST_UPLOAD, "Supplier's Pricelist Upload"},
    {Type::STOCK_TRACKING, "Stock Tracking"},
    {Type::SALES_ANALYTICS, "Sales Analytics"},
    {Type::INVENTORY_ANALYTICS, "Inventory Analytics"},
    {Type::USERS, "Users Management"},
    {Type::USER_ROLES, "User Roles"},
    {Type::USER_LOGS, "User Logs"},
    {Type::EMPLOYEES, "Employees"},
    {Type::CUSTOMERS, "Customers"},
    {Type::SUPPLIERS, "Suppliers"}};

inline const std::map<Type, const char *> OBJECT_NAMES = {
    {Type::SETTINGS, "mainMenuActionSettings"},
    {Type::EXIT, "mainMenuActionExit"},
    {Type::PURCHASE_ORDERS, "mainMenuActionPurchaseOrders"},
    {Type::SUPPLIER_MANAGEMENT, "mainMenuActionSupplierManagement"},
    {Type::GOODS_RECEIPTS, "mainMenuActionGoodsReceipts"},
    {Type::SALES_ORDERS, "mainMenuActionSalesOrders"},
    {Type::CUSTOMER_MANAGEMENT, "mainMenuActionCustomerManagement"},
    {Type::INVOICING, "mainMenuActionInvoicing"},
    {Type::PRODUCT_MANAGEMENT, "mainMenuActionProductManagement"},
    {Type::SUPPLIER_PRICELIST_UPLOAD, "mainMenuActionSupplierPricelistUpload"},
    {Type::STOCK_TRACKING, "mainMenuActionStockTracking"},
    {Type::SALES_ANALYTICS, "mainMenuActionSalesAnalytics"},
    {Type::INVENTORY_ANALYTICS, "mainMenuActionInventoryAnalytics"},
    {Type::USERS, "mainMenuActionUsers"},
    {Type::USER_ROLES, "mainMenuActionUserRoles"},
    {Type::USER_LOGS, "mainMenuActionUserLogs"},
    {Type::EMPLOYEES, "mainMenuActionEmployees"},
    {Type::CUSTOMERS, "mainMenuActionCustomers"},
    {Type::SUPPLIERS, "mainMenuActionSuppliers"}};

inline QString displayName(Type type)
{
    return QCoreApplication::translate("MainMenuActions", SOURCE_TEXTS.at(type));
}

inline QString objectName(Type type) { return QString::fromLatin1(OBJECT_NAMES.at(type)); }
} // namespace MainMenuActions
