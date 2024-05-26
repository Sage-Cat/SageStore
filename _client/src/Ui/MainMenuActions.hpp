#pragma once

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

const std::map<Type, QString> NAMES = {
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
} // namespace MainMenuActions
