#pragma once

#include "Network/NetworkService.hpp"
#include "common/AnalyticsKeys.hpp"
#include "common/Endpoints.hpp"
#include "common/Keys.hpp"

class NetworkServiceMock : public NetworkService {
public:
    NetworkServiceMock() : NetworkService(NetworkService::ServerConfig(), nullptr) {}

    void sendRequest(std::string endpoint, Method method = Method::GET,
                     const Dataset &dataset         = Dataset(),
                     const std::string &resource_id = "") override
    {
        Dataset serverResponse{{Keys::_ERROR, {"Error message"}}};

        if (endpoint == Endpoints::Users::LOGIN && method == Method::POST) {
            serverResponse = {{Common::Entities::User::ID_KEY, {"0"}},
                              {Common::Entities::User::ROLE_ID_KEY, {"0"}}};
        } else if (endpoint == Endpoints::Users::USERS && method == Method::GET) {
            serverResponse = {{Common::Entities::User::ID_KEY, {"1", "2"}},
                              {Common::Entities::User::USERNAME_KEY, {"user1", "user2"}},
                              {Common::Entities::User::ROLE_ID_KEY, {"1", "2"}}};
        } else if (endpoint == Endpoints::Users::USERS && method == Method::POST) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Users::USERS && method == Method::PUT) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Users::USERS && method == Method::DEL) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Users::ROLES && method == Method::GET) {
            serverResponse = {{Common::Entities::Role::ID_KEY, {"1", "2", "3"}},
                              {Common::Entities::Role::NAME_KEY, {"adm", "asd", "Asdf"}}};
        } else if (endpoint == Endpoints::Users::ROLES && method == Method::POST) {
            serverResponse = {{Common::Entities::Role::ID_KEY, {"0"}}};
        } else if (endpoint == Endpoints::Users::ROLES && method == Method::PUT) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Users::ROLES && method == Method::DEL) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Inventory::PRODUCT_TYPES && method == Method::GET) {
            serverResponse = {{Common::Entities::ProductType::ID_KEY, {"1", "2"}},
                              {Common::Entities::ProductType::CODE_KEY, {"PT-001", "PT-002"}},
                              {Common::Entities::ProductType::BARCODE_KEY, {"123", "456"}},
                              {Common::Entities::ProductType::NAME_KEY, {"Oil", "Brake fluid"}},
                              {Common::Entities::ProductType::DESCRIPTION_KEY,
                               {"Synthetic oil", "DOT-4"}},
                              {Common::Entities::ProductType::LAST_PRICE_KEY, {"10.50", "12.00"}},
                              {Common::Entities::ProductType::UNIT_KEY, {"pcs", "bottle"}},
                              {Common::Entities::ProductType::IS_IMPORTED_KEY, {"0", "1"}}};
        } else if (endpoint == Endpoints::Inventory::PRODUCT_TYPES && method == Method::POST) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Inventory::PRODUCT_TYPES && method == Method::PUT) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Inventory::PRODUCT_TYPES && method == Method::DEL) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Inventory::STOCKS && method == Method::GET) {
            serverResponse = {{Common::Entities::Inventory::ID_KEY, {"1", "2"}},
                              {Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, {"1", "2"}},
                              {Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY, {"10", "4"}},
                              {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"1", "1"}}};
        } else if (endpoint == Endpoints::Inventory::STOCKS && method == Method::POST) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Inventory::STOCKS && method == Method::PUT) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Inventory::STOCKS && method == Method::DEL) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Inventory::SUPPLIER_PRODUCTS && method == Method::GET) {
            serverResponse = {{Common::Entities::SuppliersProductInfo::ID_KEY, {"1"}},
                              {Common::Entities::SuppliersProductInfo::SUPPLIER_ID_KEY, {"1"}},
                              {Common::Entities::SuppliersProductInfo::PRODUCT_TYPE_ID_KEY, {"1"}},
                              {Common::Entities::SuppliersProductInfo::CODE_KEY, {"SUP-001"}}};
        } else if (endpoint == Endpoints::Inventory::SUPPLIER_PRODUCTS &&
                   (method == Method::POST || method == Method::PUT || method == Method::DEL)) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Management::CONTACTS && method == Method::GET) {
            serverResponse = {{Common::Entities::Contact::ID_KEY, {"1"}},
                              {Common::Entities::Contact::NAME_KEY, {"Default Contact"}},
                              {Common::Entities::Contact::TYPE_KEY, {"Customer"}},
                              {Common::Entities::Contact::EMAIL_KEY, {"contact@example.com"}},
                              {Common::Entities::Contact::PHONE_KEY, {"123-456-7890"}}};
        } else if (endpoint == Endpoints::Management::SUPPLIERS && method == Method::GET) {
            serverResponse = {{Common::Entities::Supplier::ID_KEY, {"1"}},
                              {Common::Entities::Supplier::NAME_KEY, {"Default Supplier"}},
                              {Common::Entities::Supplier::NUMBER_KEY, {"987654321"}},
                              {Common::Entities::Supplier::EMAIL_KEY, {"supplier@example.com"}},
                              {Common::Entities::Supplier::ADDRESS_KEY, {"456 Elm St"}}};
        } else if (endpoint == Endpoints::Management::EMPLOYEES && method == Method::GET) {
            serverResponse = {{Common::Entities::Employee::ID_KEY, {"1"}},
                              {Common::Entities::Employee::NAME_KEY, {"John Doe"}},
                              {Common::Entities::Employee::NUMBER_KEY, {"123456789"}},
                              {Common::Entities::Employee::EMAIL_KEY, {"john@example.com"}},
                              {Common::Entities::Employee::ADDRESS_KEY, {"123 Main St"}}};
        } else if ((endpoint == Endpoints::Management::CONTACTS ||
                    endpoint == Endpoints::Management::SUPPLIERS ||
                    endpoint == Endpoints::Management::EMPLOYEES) &&
                   (method == Method::POST || method == Method::PUT || method == Method::DEL)) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Purchase::ORDERS && method == Method::GET) {
            serverResponse = {{Common::Entities::PurchaseOrder::ID_KEY, {"1"}},
                              {Common::Entities::PurchaseOrder::DATE_KEY, {"2026-03-13"}},
                              {Common::Entities::PurchaseOrder::USER_ID_KEY, {"1"}},
                              {Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY, {"1"}},
                              {Common::Entities::PurchaseOrder::STATUS_KEY, {"Ordered"}}};
        } else if (endpoint == Endpoints::Purchase::ORDER_RECORDS && method == Method::GET) {
            serverResponse = {{Common::Entities::PurchaseOrderRecord::ID_KEY, {"1"}},
                              {Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, {"1"}},
                              {Common::Entities::PurchaseOrderRecord::PRODUCT_TYPE_ID_KEY, {"1"}},
                              {Common::Entities::PurchaseOrderRecord::QUANTITY_KEY, {"5"}},
                              {Common::Entities::PurchaseOrderRecord::PRICE_KEY, {"10.50"}}};
        } else if ((endpoint == Endpoints::Purchase::ORDERS ||
                    endpoint == Endpoints::Purchase::ORDER_RECORDS) &&
                   (method == Method::POST || method == Method::PUT || method == Method::DEL)) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Purchase::RECEIPTS && method == Method::POST) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Sales::ORDERS && method == Method::GET) {
            serverResponse = {{Common::Entities::SaleOrder::ID_KEY, {"1"}},
                              {Common::Entities::SaleOrder::DATE_KEY, {"2026-03-13"}},
                              {Common::Entities::SaleOrder::USER_ID_KEY, {"1"}},
                              {Common::Entities::SaleOrder::CONTACT_ID_KEY, {"1"}},
                              {Common::Entities::SaleOrder::EMPLOYEE_ID_KEY, {"1"}},
                              {Common::Entities::SaleOrder::STATUS_KEY, {"Confirmed"}}};
        } else if (endpoint == Endpoints::Sales::ORDER_RECORDS && method == Method::GET) {
            serverResponse = {{Common::Entities::SalesOrderRecord::ID_KEY, {"1"}},
                              {Common::Entities::SalesOrderRecord::ORDER_ID_KEY, {"1"}},
                              {Common::Entities::SalesOrderRecord::PRODUCT_TYPE_ID_KEY, {"1"}},
                              {Common::Entities::SalesOrderRecord::QUANTITY_KEY, {"2"}},
                              {Common::Entities::SalesOrderRecord::PRICE_KEY, {"15.75"}}};
        } else if ((endpoint == Endpoints::Sales::ORDERS ||
                    endpoint == Endpoints::Sales::ORDER_RECORDS) &&
                   (method == Method::POST || method == Method::PUT || method == Method::DEL)) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Logs::ENTRIES && method == Method::GET) {
            serverResponse = {{Common::Entities::Log::ID_KEY, {"1"}},
                              {Common::Entities::Log::USER_ID_KEY, {"1"}},
                              {Common::Entities::Log::ACTION_KEY, {"sales/orders POST"}},
                              {Common::Entities::Log::TIMESTAMP_KEY, {"2026-03-13 12:00:00"}}};
        } else if (endpoint == Endpoints::Analytics::SALES && method == Method::GET) {
            serverResponse = {{AnalyticsKeys::Sales::TOTAL_ORDERS, {"1"}},
                              {AnalyticsKeys::Sales::TOTAL_ORDER_LINES, {"1"}},
                              {AnalyticsKeys::Sales::TOTAL_REVENUE, {"31.50"}},
                              {AnalyticsKeys::Sales::AVERAGE_ORDER_VALUE, {"31.50"}},
                              {AnalyticsKeys::Sales::UNIQUE_CUSTOMERS, {"1"}}};
        } else if (endpoint == Endpoints::Analytics::INVENTORY && method == Method::GET) {
            serverResponse = {{AnalyticsKeys::Inventory::TOTAL_PRODUCT_TYPES, {"2"}},
                              {AnalyticsKeys::Inventory::TOTAL_STOCK_RECORDS, {"2"}},
                              {AnalyticsKeys::Inventory::TOTAL_UNITS_AVAILABLE, {"14"}},
                              {AnalyticsKeys::Inventory::IMPORTED_PRODUCT_TYPES, {"1"}},
                              {AnalyticsKeys::Inventory::ZERO_STOCK_RECORDS, {"0"}}};
        }
        emit responseReceived(endpoint, method, serverResponse);
    }
};
