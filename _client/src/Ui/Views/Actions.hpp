#pragma once

#include <map>
#include <string>

namespace Actions {

enum class Types {
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
    USER_ROLES,
    USER_LOGS,
    EMPLOYEES,
    CUSTOMERS,
    SUPPLIERS
};

const std::map<Types, std::string> NAMES = {
    {Types::SETTINGS, "Settings"},
    {Types::EXIT, "Exit"},
    {Types::PURCHASE_ORDERS, "Purchase Orders"},
    {Types::SUPPLIER_MANAGEMENT, "Supplier Management"},
    {Types::GOODS_RECEIPTS, "Goods Receipts"},
    {Types::SALES_ORDERS, "Sales Orders"},
    {Types::CUSTOMER_MANAGEMENT, "Customer Management"},
    {Types::INVOICING, "Invoicing"},
    {Types::PRODUCT_MANAGEMENT, "Product Management"},
    {Types::SUPPLIER_PRICELIST_UPLOAD, "Supplier's Pricelist Upload"},
    {Types::STOCK_TRACKING, "Stock Tracking"},
    {Types::SALES_ANALYTICS, "Sales Analytics"},
    {Types::INVENTORY_ANALYTICS, "Inventory Analytics"},
    {Types::USER_ROLES, "User Roles"},
    {Types::USER_LOGS, "User Logs"},
    {Types::EMPLOYEES, "Employees"},
    {Types::CUSTOMERS, "Customers"},
    {Types::SUPPLIERS, "Suppliers"}
};

} 
