#pragma once

#include <algorithm>
#include <vector>

#include "Network/ApiManager.hpp"

#include "common/AnalyticsKeys.hpp"

#include "mocks/NetworkServiceMock.hpp"

class ApiManagerMock : public ApiManager {
public:
    ApiManagerMock() : ApiManager(*(new NetworkServiceMock))
    {
        m_users = {
            Common::Entities::User{.id = "1", .username = "admin", .password = "", .roleId = "1"},
            Common::Entities::User{.id = "2", .username = "manager", .password = "", .roleId = "2"}};
        m_roles = {Common::Entities::Role{.id = "1", .name = "Administrator"},
                   Common::Entities::Role{.id = "2", .name = "Manager"}};
        m_productTypes = {
            Common::Entities::ProductType{.id = "1",
                                          .code = "PT-001",
                                          .barcode = "123",
                                          .name = "Oil",
                                          .description = "Synthetic oil",
                                          .lastPrice = "10.50",
                                          .unit = "pcs",
                                          .isImported = "0"},
            Common::Entities::ProductType{.id = "2",
                                          .code = "PT-002",
                                          .barcode = "456",
                                          .name = "Brake fluid",
                                          .description = "DOT-4",
                                          .lastPrice = "12.00",
                                          .unit = "bottle",
                                          .isImported = "1"}};
        m_stocks = {Common::Entities::Inventory{.id = "1",
                                                .productTypeId = "1",
                                                .quantityAvailable = "10",
                                                .employeeId = "1"}};
        m_contacts = {Common::Entities::Contact{.id = "1",
                                                .name = "Default Contact",
                                                .type = "Customer",
                                                .email = "contact@example.com",
                                                .phone = "123-456-7890"}};
        m_suppliers = {Common::Entities::Supplier{.id = "1",
                                                  .name = "Default Supplier",
                                                  .number = "987654321",
                                                  .email = "supplier@example.com",
                                                  .address = "456 Elm St"}};
        m_employees = {Common::Entities::Employee{.id = "1",
                                                  .name = "John Doe",
                                                  .number = "123456789",
                                                  .email = "john@example.com",
                                                  .address = "123 Main St"}};
        m_supplierProducts = {Common::Entities::SuppliersProductInfo{
            .id = "1", .supplierID = "1", .productTypeId = "1", .code = "SUP-001"}};
        m_purchaseOrders = {Common::Entities::PurchaseOrder{
            .id = "1", .date = "2026-03-13", .userId = "1", .supplierId = "1", .status = "Ordered"}};
        m_purchaseOrderRecords = {Common::Entities::PurchaseOrderRecord{
            .id = "1", .orderId = "1", .productTypeId = "1", .quantity = "5", .price = "10.50"}};
        m_salesOrders = {Common::Entities::SaleOrder{.id = "1",
                                                     .date = "2026-03-13",
                                                     .userId = "1",
                                                     .contactId = "1",
                                                     .employeeId = "1",
                                                     .status = "Confirmed"}};
        m_salesOrderRecords = {Common::Entities::SalesOrderRecord{
            .id = "1", .orderId = "1", .productTypeId = "1", .quantity = "2", .price = "15.75"}};
        m_logs = {Common::Entities::Log{.id = "1",
                                        .userId = "1",
                                        .action = "sales/orders POST #1",
                                        .timestamp = "2026-03-13 12:00:00"}};
    }

    int usersRequestCount() const { return m_getUsersCalls; }
    int rolesRequestCount() const { return m_getRolesCalls; }
    int productTypesRequestCount() const { return m_getProductTypesCalls; }
    int stocksRequestCount() const { return m_getStocksCalls; }
    int contactsRequestCount() const { return m_getContactsCalls; }
    int suppliersRequestCount() const { return m_getSuppliersCalls; }
    int employeesRequestCount() const { return m_getEmployeesCalls; }
    int supplierProductsRequestCount() const { return m_getSupplierProductsCalls; }
    int purchaseOrdersRequestCount() const { return m_getPurchaseOrdersCalls; }
    int purchaseOrderRecordsRequestCount() const { return m_getPurchaseOrderRecordsCalls; }
    int salesOrdersRequestCount() const { return m_getSalesOrdersCalls; }
    int salesOrderRecordsRequestCount() const { return m_getSalesOrderRecordsCalls; }
    int logsRequestCount() const { return m_getLogsCalls; }
    int salesAnalyticsRequestCount() const { return m_getSalesAnalyticsCalls; }
    int inventoryAnalyticsRequestCount() const { return m_getInventoryAnalyticsCalls; }
    int roleEditCount() const { return m_editRoleCalls; }
    int userEditCount() const { return m_editUserCalls; }
    int stockEditCount() const { return m_editStockCalls; }
    int supplierProductEditCount() const { return m_editSupplierProductCalls; }
    int contactEditCount() const { return m_editContactCalls; }
    int supplierEditCount() const { return m_editSupplierCalls; }
    int employeeEditCount() const { return m_editEmployeeCalls; }
    int purchaseOrderEditCount() const { return m_editPurchaseOrderCalls; }
    int purchaseOrderRecordEditCount() const { return m_editPurchaseOrderRecordCalls; }
    int salesOrderEditCount() const { return m_editSalesOrderCalls; }
    int salesOrderRecordEditCount() const { return m_editSalesOrderRecordCalls; }
    int productTypeEditCount() const { return m_editProductTypeCalls; }

    void setSalesOrders(std::vector<Common::Entities::SaleOrder> orders)
    {
        m_salesOrders = std::move(orders);
    }

    void setPurchaseOrders(std::vector<Common::Entities::PurchaseOrder> orders)
    {
        m_purchaseOrders = std::move(orders);
    }

public slots:
    void addUser(const Common::Entities::User &user) override
    {
        auto created = user;
        created.id = std::to_string(m_nextUserId++);
        m_users.push_back(created);
        emit userAdded();
    }

    void editUser(const Common::Entities::User &user) override
    {
        ++m_editUserCalls;
        auto it = std::find_if(m_users.begin(), m_users.end(),
                               [&user](const auto &existingUser) {
                                   return existingUser.id == user.id;
                               });
        if (it != m_users.end()) {
            it->username = user.username;
            it->roleId = user.roleId;
            if (!user.password.empty()) {
                it->password = user.password;
            }
        }
        emit userEdited();
    }

    void deleteUser(const QString &id) override
    {
        std::erase_if(m_users, [&id](const auto &user) { return user.id == id.toStdString(); });
        emit userDeleted();
    }
    void getUsers() override
    {
        ++m_getUsersCalls;
        emit usersList(m_users);
    }

    void editRole(const Common::Entities::Role &role) override
    {
        ++m_editRoleCalls;
        auto it = std::find_if(m_roles.begin(), m_roles.end(),
                               [&role](const auto &existingRole) {
                                   return existingRole.id == role.id;
                               });
        if (it != m_roles.end()) {
            *it = role;
        }
        emit roleEdited();
    }

    void deleteRole(const QString &id) override
    {
        std::erase_if(m_roles, [&id](const auto &role) { return role.id == id.toStdString(); });
        emit roleDeleted();
    }

    void getRoleList() override
    {
        ++m_getRolesCalls;
        emit rolesList(m_roles);
    }

    void createRole(const Common::Entities::Role &role) override
    {
        auto createdRole = role;
        createdRole.id   = std::to_string(m_nextRoleId++);
        m_roles.push_back(createdRole);
        emit roleCreated();
    }

    void getProductTypes() override
    {
        ++m_getProductTypesCalls;
        emit productTypesList(m_productTypes);
    }

    void createProductType(const Common::Entities::ProductType &productType) override
    {
        auto createdProductType = productType;
        createdProductType.id   = std::to_string(m_nextProductTypeId++);
        m_productTypes.push_back(createdProductType);
        emit productTypeCreated();
    }

    void editProductType(const Common::Entities::ProductType &productType) override
    {
        ++m_editProductTypeCalls;
        auto it = std::find_if(m_productTypes.begin(), m_productTypes.end(),
                               [&productType](const auto &existingProductType) {
                                   return existingProductType.id == productType.id;
                               });
        if (it != m_productTypes.end()) {
            *it = productType;
        }
        emit productTypeEdited();
    }

    void deleteProductType(const QString &id) override
    {
        std::erase_if(m_productTypes,
                      [&id](const auto &productType) { return productType.id == id.toStdString(); });
        emit productTypeDeleted();
    }

    void getStocks() override
    {
        ++m_getStocksCalls;
        emit stocksList(m_stocks);
    }

    void createStock(const Common::Entities::Inventory &stock) override
    {
        auto createdStock = stock;
        createdStock.id   = std::to_string(m_nextStockId++);
        m_stocks.push_back(createdStock);
        emit stockCreated();
    }

    void editStock(const Common::Entities::Inventory &stock) override
    {
        ++m_editStockCalls;
        auto it = std::find_if(m_stocks.begin(), m_stocks.end(),
                               [&stock](const auto &existingStock) {
                                   return existingStock.id == stock.id;
                               });
        if (it != m_stocks.end()) {
            *it = stock;
        }
        emit stockEdited();
    }

    void deleteStock(const QString &id) override
    {
        std::erase_if(m_stocks, [&id](const auto &stock) { return stock.id == id.toStdString(); });
        emit stockDeleted();
    }

    void getSupplierProducts() override
    {
        ++m_getSupplierProductsCalls;
        emit supplierProductsList(m_supplierProducts);
    }

    void createSupplierProduct(const Common::Entities::SuppliersProductInfo &supplierProduct) override
    {
        auto created = supplierProduct;
        created.id = std::to_string(m_nextSupplierProductId++);
        m_supplierProducts.push_back(created);
        emit supplierProductCreated();
    }

    void editSupplierProduct(const Common::Entities::SuppliersProductInfo &supplierProduct) override
    {
        ++m_editSupplierProductCalls;
        auto it = std::find_if(m_supplierProducts.begin(), m_supplierProducts.end(),
                               [&supplierProduct](const auto &existingItem) {
                                   return existingItem.id == supplierProduct.id;
                               });
        if (it != m_supplierProducts.end()) {
            *it = supplierProduct;
        }
        emit supplierProductEdited();
    }

    void deleteSupplierProduct(const QString &id) override
    {
        std::erase_if(m_supplierProducts, [&id](const auto &item) {
            return item.id == id.toStdString();
        });
        emit supplierProductDeleted();
    }

    void getContacts() override
    {
        ++m_getContactsCalls;
        emit contactsList(m_contacts);
    }

    void createContact(const Common::Entities::Contact &contact) override
    {
        auto created = contact;
        created.id = std::to_string(m_nextContactId++);
        m_contacts.push_back(created);
        emit contactCreated();
    }

    void editContact(const Common::Entities::Contact &contact) override
    {
        ++m_editContactCalls;
        auto it = std::find_if(m_contacts.begin(), m_contacts.end(),
                               [&contact](const auto &existingContact) {
                                   return existingContact.id == contact.id;
                               });
        if (it != m_contacts.end()) {
            *it = contact;
        }
        emit contactEdited();
    }

    void deleteContact(const QString &id) override
    {
        std::erase_if(m_contacts, [&id](const auto &contact) {
            return contact.id == id.toStdString();
        });
        emit contactDeleted();
    }

    void getSuppliers() override
    {
        ++m_getSuppliersCalls;
        emit suppliersList(m_suppliers);
    }

    void createSupplier(const Common::Entities::Supplier &supplier) override
    {
        auto created = supplier;
        created.id = std::to_string(m_nextSupplierId++);
        m_suppliers.push_back(created);
        emit supplierCreated();
    }

    void editSupplier(const Common::Entities::Supplier &supplier) override
    {
        ++m_editSupplierCalls;
        auto it = std::find_if(m_suppliers.begin(), m_suppliers.end(),
                               [&supplier](const auto &existingSupplier) {
                                   return existingSupplier.id == supplier.id;
                               });
        if (it != m_suppliers.end()) {
            *it = supplier;
        }
        emit supplierEdited();
    }

    void deleteSupplier(const QString &id) override
    {
        std::erase_if(m_suppliers, [&id](const auto &supplier) {
            return supplier.id == id.toStdString();
        });
        emit supplierDeleted();
    }

    void getEmployees() override
    {
        ++m_getEmployeesCalls;
        emit employeesList(m_employees);
    }

    void createEmployee(const Common::Entities::Employee &employee) override
    {
        auto created = employee;
        created.id = std::to_string(m_nextEmployeeId++);
        m_employees.push_back(created);
        emit employeeCreated();
    }

    void editEmployee(const Common::Entities::Employee &employee) override
    {
        ++m_editEmployeeCalls;
        auto it = std::find_if(m_employees.begin(), m_employees.end(),
                               [&employee](const auto &existingEmployee) {
                                   return existingEmployee.id == employee.id;
                               });
        if (it != m_employees.end()) {
            *it = employee;
        }
        emit employeeEdited();
    }

    void deleteEmployee(const QString &id) override
    {
        std::erase_if(m_employees, [&id](const auto &employee) {
            return employee.id == id.toStdString();
        });
        emit employeeDeleted();
    }

    void getPurchaseOrders() override
    {
        ++m_getPurchaseOrdersCalls;
        emit purchaseOrdersList(m_purchaseOrders);
    }

    void createPurchaseOrder(const Common::Entities::PurchaseOrder &order) override
    {
        auto created = order;
        created.id = std::to_string(m_nextPurchaseOrderId++);
        m_purchaseOrders.push_back(created);
        emit purchaseOrderCreated();
    }

    void editPurchaseOrder(const Common::Entities::PurchaseOrder &order) override
    {
        ++m_editPurchaseOrderCalls;
        auto it = std::find_if(m_purchaseOrders.begin(), m_purchaseOrders.end(),
                               [&order](const auto &existingOrder) {
                                   return existingOrder.id == order.id;
                               });
        if (it != m_purchaseOrders.end()) {
            *it = order;
        }
        emit purchaseOrderEdited();
    }

    void deletePurchaseOrder(const QString &id) override
    {
        std::erase_if(m_purchaseOrders, [&id](const auto &order) {
            return order.id == id.toStdString();
        });
        std::erase_if(m_purchaseOrderRecords, [&id](const auto &record) {
            return record.orderId == id.toStdString();
        });
        emit purchaseOrderDeleted();
    }

    void getPurchaseOrderRecords(const QString &orderId = {}) override
    {
        ++m_getPurchaseOrderRecordsCalls;
        if (orderId.isEmpty()) {
            emit purchaseOrderRecordsList(m_purchaseOrderRecords);
            return;
        }

        std::vector<Common::Entities::PurchaseOrderRecord> filtered;
        std::copy_if(m_purchaseOrderRecords.begin(), m_purchaseOrderRecords.end(),
                     std::back_inserter(filtered), [&orderId](const auto &record) {
                         return record.orderId == orderId.toStdString();
                     });
        emit purchaseOrderRecordsList(filtered);
    }

    void createPurchaseOrderRecord(const Common::Entities::PurchaseOrderRecord &record) override
    {
        auto created = record;
        created.id = std::to_string(m_nextPurchaseOrderRecordId++);
        m_purchaseOrderRecords.push_back(created);
        emit purchaseOrderRecordCreated();
    }

    void editPurchaseOrderRecord(const Common::Entities::PurchaseOrderRecord &record) override
    {
        ++m_editPurchaseOrderRecordCalls;
        auto it = std::find_if(m_purchaseOrderRecords.begin(), m_purchaseOrderRecords.end(),
                               [&record](const auto &existingRecord) {
                                   return existingRecord.id == record.id;
                               });
        if (it != m_purchaseOrderRecords.end()) {
            *it = record;
        }
        emit purchaseOrderRecordEdited();
    }

    void deletePurchaseOrderRecord(const QString &id) override
    {
        std::erase_if(m_purchaseOrderRecords, [&id](const auto &record) {
            return record.id == id.toStdString();
        });
        emit purchaseOrderRecordDeleted();
    }

    void postPurchaseReceipt(const QString &orderId, const QString &employeeId) override
    {
        const auto orderIdStd    = orderId.toStdString();
        const auto employeeIdStd = employeeId.toStdString();

        auto orderIt = std::find_if(m_purchaseOrders.begin(), m_purchaseOrders.end(),
                                    [&orderIdStd](const auto &order) {
                                        return order.id == orderIdStd;
                                    });
        if (orderIt != m_purchaseOrders.end()) {
            orderIt->status = "Received";
        }

        for (const auto &record : m_purchaseOrderRecords) {
            if (record.orderId != orderIdStd) {
                continue;
            }

            auto stockIt = std::find_if(m_stocks.begin(), m_stocks.end(),
                                        [&record](const auto &stock) {
                                            return stock.productTypeId == record.productTypeId;
                                        });
            if (stockIt == m_stocks.end()) {
                m_stocks.push_back(Common::Entities::Inventory{
                    .id = std::to_string(m_nextStockId++),
                    .productTypeId = record.productTypeId,
                    .quantityAvailable = record.quantity,
                    .employeeId = employeeIdStd});
                continue;
            }

            stockIt->quantityAvailable = std::to_string(
                std::stoll(stockIt->quantityAvailable) + std::stoll(record.quantity));
            stockIt->employeeId = employeeIdStd;
        }

        emit purchaseReceiptPosted();
    }

    void getSalesOrders() override
    {
        ++m_getSalesOrdersCalls;
        emit salesOrdersList(m_salesOrders);
    }

    void createSalesOrder(const Common::Entities::SaleOrder &order) override
    {
        auto created = order;
        created.id = std::to_string(m_nextSalesOrderId++);
        m_salesOrders.push_back(created);
        emit salesOrderCreated();
    }

    void editSalesOrder(const Common::Entities::SaleOrder &order) override
    {
        ++m_editSalesOrderCalls;
        auto it = std::find_if(m_salesOrders.begin(), m_salesOrders.end(),
                               [&order](const auto &existingOrder) {
                                   return existingOrder.id == order.id;
                               });
        if (it != m_salesOrders.end()) {
            *it = order;
        }
        emit salesOrderEdited();
    }

    void deleteSalesOrder(const QString &id) override
    {
        std::erase_if(m_salesOrders, [&id](const auto &order) {
            return order.id == id.toStdString();
        });
        std::erase_if(m_salesOrderRecords, [&id](const auto &record) {
            return record.orderId == id.toStdString();
        });
        emit salesOrderDeleted();
    }

    void getSalesOrderRecords(const QString &orderId = {}) override
    {
        ++m_getSalesOrderRecordsCalls;
        if (orderId.isEmpty()) {
            emit salesOrderRecordsList(m_salesOrderRecords);
            return;
        }

        std::vector<Common::Entities::SalesOrderRecord> filtered;
        std::copy_if(m_salesOrderRecords.begin(), m_salesOrderRecords.end(),
                     std::back_inserter(filtered), [&orderId](const auto &record) {
                         return record.orderId == orderId.toStdString();
                     });
        emit salesOrderRecordsList(filtered);
    }

    void createSalesOrderRecord(const Common::Entities::SalesOrderRecord &record) override
    {
        auto created = record;
        created.id = std::to_string(m_nextSalesOrderRecordId++);
        m_salesOrderRecords.push_back(created);
        emit salesOrderRecordCreated();
    }

    void editSalesOrderRecord(const Common::Entities::SalesOrderRecord &record) override
    {
        ++m_editSalesOrderRecordCalls;
        auto it = std::find_if(m_salesOrderRecords.begin(), m_salesOrderRecords.end(),
                               [&record](const auto &existingRecord) {
                                   return existingRecord.id == record.id;
                               });
        if (it != m_salesOrderRecords.end()) {
            *it = record;
        }
        emit salesOrderRecordEdited();
    }

    void deleteSalesOrderRecord(const QString &id) override
    {
        std::erase_if(m_salesOrderRecords, [&id](const auto &record) {
            return record.id == id.toStdString();
        });
        emit salesOrderRecordDeleted();
    }

    void getLogs() override
    {
        ++m_getLogsCalls;
        emit logsList(m_logs);
    }

    void getSalesAnalytics() override
    {
        ++m_getSalesAnalyticsCalls;
        emit salesAnalyticsReady({{AnalyticsKeys::Sales::TOTAL_ORDERS, "1"},
                                  {AnalyticsKeys::Sales::TOTAL_ORDER_LINES, "1"},
                                  {AnalyticsKeys::Sales::TOTAL_REVENUE, "31.50"},
                                  {AnalyticsKeys::Sales::TOTAL_PURCHASE_COST, "10.50"},
                                  {AnalyticsKeys::Sales::GROSS_PROFIT, "21.00"},
                                  {AnalyticsKeys::Sales::GROSS_MARGIN_PERCENT, "66.67"},
                                  {AnalyticsKeys::Sales::AVERAGE_ORDER_VALUE, "31.50"},
                                  {AnalyticsKeys::Sales::UNIQUE_CUSTOMERS, "1"},
                                  {AnalyticsKeys::Sales::DRAFT_ORDERS, "0"},
                                  {AnalyticsKeys::Sales::CONFIRMED_ORDERS, "1"},
                                  {AnalyticsKeys::Sales::INVOICED_ORDERS, "0"}});
    }

    void getInventoryAnalytics() override
    {
        ++m_getInventoryAnalyticsCalls;
        emit inventoryAnalyticsReady({{AnalyticsKeys::Inventory::TOTAL_PRODUCT_TYPES, "2"},
                                      {AnalyticsKeys::Inventory::TOTAL_STOCK_RECORDS, "1"},
                                      {AnalyticsKeys::Inventory::TOTAL_UNITS_AVAILABLE, "10"},
                                      {AnalyticsKeys::Inventory::IMPORTED_PRODUCT_TYPES, "1"},
                                      {AnalyticsKeys::Inventory::ZERO_STOCK_RECORDS, "0"},
                                      {AnalyticsKeys::Inventory::TOTAL_PURCHASE_ORDERS, "1"},
                                      {AnalyticsKeys::Inventory::OPEN_PURCHASE_ORDERS, "1"},
                                      {AnalyticsKeys::Inventory::RECEIVED_PURCHASE_ORDERS, "0"},
                                      {AnalyticsKeys::Inventory::TOTAL_PURCHASE_SPEND, "52.50"},
                                      {AnalyticsKeys::Inventory::INVENTORY_VALUE_ESTIMATE, "105.00"}});
    }

    void emitError(const QString &errorMessage) { emit errorOccurred(errorMessage); }

private:
    int m_getUsersCalls{0};
    int m_getRolesCalls{0};
    int m_getProductTypesCalls{0};
    int m_getStocksCalls{0};
    int m_getContactsCalls{0};
    int m_getSuppliersCalls{0};
    int m_getEmployeesCalls{0};
    int m_getSupplierProductsCalls{0};
    int m_getPurchaseOrdersCalls{0};
    int m_getPurchaseOrderRecordsCalls{0};
    int m_getSalesOrdersCalls{0};
    int m_getSalesOrderRecordsCalls{0};
    int m_getLogsCalls{0};
    int m_getSalesAnalyticsCalls{0};
    int m_getInventoryAnalyticsCalls{0};
    int m_editRoleCalls{0};
    int m_editUserCalls{0};
    int m_editProductTypeCalls{0};
    int m_editStockCalls{0};
    int m_editSupplierProductCalls{0};
    int m_editContactCalls{0};
    int m_editSupplierCalls{0};
    int m_editEmployeeCalls{0};
    int m_editPurchaseOrderCalls{0};
    int m_editPurchaseOrderRecordCalls{0};
    int m_editSalesOrderCalls{0};
    int m_editSalesOrderRecordCalls{0};

    int m_nextUserId{3};
    int m_nextRoleId{3};
    int m_nextProductTypeId{3};
    int m_nextStockId{2};
    int m_nextContactId{2};
    int m_nextSupplierId{2};
    int m_nextEmployeeId{2};
    int m_nextSupplierProductId{2};
    int m_nextPurchaseOrderId{2};
    int m_nextPurchaseOrderRecordId{2};
    int m_nextSalesOrderId{2};
    int m_nextSalesOrderRecordId{2};

    std::vector<Common::Entities::User> m_users;
    std::vector<Common::Entities::Role> m_roles;
    std::vector<Common::Entities::ProductType> m_productTypes;
    std::vector<Common::Entities::Inventory> m_stocks;
    std::vector<Common::Entities::Contact> m_contacts;
    std::vector<Common::Entities::Supplier> m_suppliers;
    std::vector<Common::Entities::Employee> m_employees;
    std::vector<Common::Entities::SuppliersProductInfo> m_supplierProducts;
    std::vector<Common::Entities::PurchaseOrder> m_purchaseOrders;
    std::vector<Common::Entities::PurchaseOrderRecord> m_purchaseOrderRecords;
    std::vector<Common::Entities::SaleOrder> m_salesOrders;
    std::vector<Common::Entities::SalesOrderRecord> m_salesOrderRecords;
    std::vector<Common::Entities::Log> m_logs;
};
