#pragma once

#include <map>
#include <vector>

#include <QObject>
#include <QMap>
#include <QVariantMap>

#include "common/DataTypes.hpp"
#include "common/Entities/Contact.hpp"
#include "common/Entities/Employee.hpp"
#include "common/Entities/Inventory.hpp"
#include "common/Entities/Log.hpp"
#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Entities/Role.hpp"
#include "common/Entities/SaleOrder.hpp"
#include "common/Entities/SalesOrderRecord.hpp"
#include "common/Entities/Supplier.hpp"
#include "common/Entities/SuppliersProductInfo.hpp"
#include "common/Entities/User.hpp"

class NetworkService;
enum class Method;

class ApiManager : public QObject {
    Q_OBJECT

    using ResponseHandler = std::function<void(Method method, const Dataset &)>;

public:
    ApiManager(NetworkService &networkService);
    ~ApiManager();

public slots:
    // Users
    virtual void loginUser(const QString &username, const QString &password);
    virtual void getUsers();
    virtual void addUser(const Common::Entities::User &user);
    virtual void editUser(const Common::Entities::User &user);
    virtual void deleteUser(const QString &id);

    // Roles
    virtual void getRoleList();
    virtual void createRole(const Common::Entities::Role &role);
    virtual void editRole(const Common::Entities::Role &role);
    virtual void deleteRole(const QString &id);

    // Inventory / Product types
    virtual void getProductTypes();
    virtual void createProductType(const Common::Entities::ProductType &productType);
    virtual void editProductType(const Common::Entities::ProductType &productType);
    virtual void deleteProductType(const QString &id);
    virtual void getStocks();
    virtual void createStock(const Common::Entities::Inventory &stock);
    virtual void editStock(const Common::Entities::Inventory &stock);
    virtual void deleteStock(const QString &id);
    virtual void getSupplierProducts();
    virtual void createSupplierProduct(const Common::Entities::SuppliersProductInfo &supplierProduct);
    virtual void editSupplierProduct(const Common::Entities::SuppliersProductInfo &supplierProduct);
    virtual void deleteSupplierProduct(const QString &id);

    // Management
    virtual void getContacts();
    virtual void createContact(const Common::Entities::Contact &contact);
    virtual void editContact(const Common::Entities::Contact &contact);
    virtual void deleteContact(const QString &id);
    virtual void getSuppliers();
    virtual void createSupplier(const Common::Entities::Supplier &supplier);
    virtual void editSupplier(const Common::Entities::Supplier &supplier);
    virtual void deleteSupplier(const QString &id);
    virtual void getEmployees();
    virtual void createEmployee(const Common::Entities::Employee &employee);
    virtual void editEmployee(const Common::Entities::Employee &employee);
    virtual void deleteEmployee(const QString &id);

    // Purchase
    virtual void getPurchaseOrders();
    virtual void createPurchaseOrder(const Common::Entities::PurchaseOrder &order);
    virtual void editPurchaseOrder(const Common::Entities::PurchaseOrder &order);
    virtual void deletePurchaseOrder(const QString &id);
    virtual void getPurchaseOrderRecords(const QString &orderId = {});
    virtual void createPurchaseOrderRecord(const Common::Entities::PurchaseOrderRecord &record);
    virtual void editPurchaseOrderRecord(const Common::Entities::PurchaseOrderRecord &record);
    virtual void deletePurchaseOrderRecord(const QString &id);
    virtual void postPurchaseReceipt(const QString &orderId, const QString &employeeId);

    // Sales
    virtual void getSalesOrders();
    virtual void createSalesOrder(const Common::Entities::SaleOrder &order);
    virtual void editSalesOrder(const Common::Entities::SaleOrder &order);
    virtual void deleteSalesOrder(const QString &id);
    virtual void getSalesOrderRecords(const QString &orderId = {});
    virtual void createSalesOrderRecord(const Common::Entities::SalesOrderRecord &record);
    virtual void editSalesOrderRecord(const Common::Entities::SalesOrderRecord &record);
    virtual void deleteSalesOrderRecord(const QString &id);

    // Logs
    virtual void getLogs();

    // Analytics
    virtual void getSalesAnalytics();
    virtual void getInventoryAnalytics();

protected slots:
    // for NetworkService
    virtual void handleResponse(const std::string &endpoint, Method method, const Dataset &dataset);

signals:
    // Startup
    void ready();

    // Users
    void loginSuccess(const QString &id, const QString &roleId);
    void userAdded();
    void userEdited();
    void userDeleted();
    void usersList(const std::vector<Common::Entities::User> &users);

    // Roles
    void rolesList(const std::vector<Common::Entities::Role> &roleList);
    void roleCreated();
    void roleEdited();
    void roleDeleted();

    // Inventory / Product types
    void productTypesList(const std::vector<Common::Entities::ProductType> &productTypes);
    void productTypeCreated();
    void productTypeEdited();
    void productTypeDeleted();
    void stocksList(const std::vector<Common::Entities::Inventory> &stocks);
    void stockCreated();
    void stockEdited();
    void stockDeleted();
    void supplierProductsList(
        const std::vector<Common::Entities::SuppliersProductInfo> &supplierProducts);
    void supplierProductCreated();
    void supplierProductEdited();
    void supplierProductDeleted();

    // Management
    void contactsList(const std::vector<Common::Entities::Contact> &contacts);
    void contactCreated();
    void contactEdited();
    void contactDeleted();
    void suppliersList(const std::vector<Common::Entities::Supplier> &suppliers);
    void supplierCreated();
    void supplierEdited();
    void supplierDeleted();
    void employeesList(const std::vector<Common::Entities::Employee> &employees);
    void employeeCreated();
    void employeeEdited();
    void employeeDeleted();

    // Purchase
    void purchaseOrdersList(const std::vector<Common::Entities::PurchaseOrder> &orders);
    void purchaseOrderCreated();
    void purchaseOrderEdited();
    void purchaseOrderDeleted();
    void purchaseOrderRecordsList(
        const std::vector<Common::Entities::PurchaseOrderRecord> &records);
    void purchaseOrderRecordCreated();
    void purchaseOrderRecordEdited();
    void purchaseOrderRecordDeleted();
    void purchaseReceiptPosted();

    // Sales
    void salesOrdersList(const std::vector<Common::Entities::SaleOrder> &orders);
    void salesOrderCreated();
    void salesOrderEdited();
    void salesOrderDeleted();
    void salesOrderRecordsList(const std::vector<Common::Entities::SalesOrderRecord> &records);
    void salesOrderRecordCreated();
    void salesOrderRecordEdited();
    void salesOrderRecordDeleted();

    // Logs
    void logsList(const std::vector<Common::Entities::Log> &logs);

    // Analytics
    void salesAnalyticsReady(const QVariantMap &metrics);
    void inventoryAnalyticsReady(const QVariantMap &metrics);

    // Error handling
    void errorOccurred(const QString &errorMessage);

private:
    void init();
    void setupHandlers();

    // handlers
    void handleError(const std::string &errorMessage);
    void handleLoginResponse(Method method, const Dataset &dataset);
    void handleUsers(Method method, const Dataset &dataset);
    void handleRoles(Method method, const Dataset &dataset);
    void handleProductTypes(Method method, const Dataset &dataset);
    void handleStocks(Method method, const Dataset &dataset);
    void handleSupplierProducts(Method method, const Dataset &dataset);
    void handleContacts(Method method, const Dataset &dataset);
    void handleSuppliers(Method method, const Dataset &dataset);
    void handleEmployees(Method method, const Dataset &dataset);
    void handlePurchaseOrders(Method method, const Dataset &dataset);
    void handlePurchaseOrderRecords(Method method, const Dataset &dataset);
    void handleSalesOrders(Method method, const Dataset &dataset);
    void handleSalesOrderRecords(Method method, const Dataset &dataset);
    void handleLogs(Method method, const Dataset &dataset);
    void handleSalesAnalyticsResponse(Method method, const Dataset &dataset);
    void handleInventoryAnalyticsResponse(Method method, const Dataset &dataset);

private:
    void handleRolesList(const Dataset &dataset);
    void handleUsersList(const Dataset &dataset);
    void handleProductTypesList(const Dataset &dataset);
    void handleStocksList(const Dataset &dataset);
    void handleSupplierProductsList(const Dataset &dataset);
    void handleContactsList(const Dataset &dataset);
    void handleSuppliersList(const Dataset &dataset);
    void handleEmployeesList(const Dataset &dataset);
    void handlePurchaseOrdersList(const Dataset &dataset);
    void handlePurchaseOrderRecordsList(const Dataset &dataset);
    void handleSalesOrdersList(const Dataset &dataset);
    void handleSalesOrderRecordsList(const Dataset &dataset);
    void handleLogsList(const Dataset &dataset);
    QVariantMap buildAnalyticsMetrics(const Dataset &dataset) const;

private:
    NetworkService &m_networkService;
    std::map<std::string, ResponseHandler> m_responseHandlers;
};
