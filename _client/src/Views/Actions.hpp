#pragma once

#include <map>
#include <string>

enum class ActionTypes {
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

const std::map<ActionTypes, std::string> ACTION_NAMES = {
    {ActionTypes::SETTINGS, "Settings"},
    {ActionTypes::EXIT, "Exit"},
    {ActionTypes::PURCHASE_ORDERS, "Purchase Orders"},
    {ActionTypes::SUPPLIER_MANAGEMENT, "Supplier Management"},
    {ActionTypes::GOODS_RECEIPTS, "Goods Receipts"},
    {ActionTypes::SALES_ORDERS, "Sales Orders"},
    {ActionTypes::CUSTOMER_MANAGEMENT, "Customer Management"},
    {ActionTypes::INVOICING, "Invoicing"},
    {ActionTypes::PRODUCT_MANAGEMENT, "Product Management"},
    {ActionTypes::SUPPLIER_PRICELIST_UPLOAD, "Supplier's Pricelist Upload"},
    {ActionTypes::STOCK_TRACKING, "Stock Tracking"},
    {ActionTypes::SALES_ANALYTICS, "Sales Analytics"},
    {ActionTypes::INVENTORY_ANALYTICS, "Inventory Analytics"},
    {ActionTypes::USER_ROLES, "User Roles"},
    {ActionTypes::USER_LOGS, "User Logs"},
    {ActionTypes::EMPLOYEES, "Employees"},
    {ActionTypes::CUSTOMERS, "Customers"},
    {ActionTypes::SUPPLIERS, "Suppliers"}
};