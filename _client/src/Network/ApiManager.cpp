#include "ApiManager.hpp"

#include "Network/NetworkService.hpp"
#include "common/AnalyticsKeys.hpp"
#include "common/Endpoints.hpp"
#include "common/Entities/Contact.hpp"
#include "common/Entities/Employee.hpp"
#include "common/Entities/Inventory.hpp"
#include "common/Entities/Log.hpp"
#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Entities/SaleOrder.hpp"
#include "common/Entities/SalesOrderRecord.hpp"
#include "common/Entities/Supplier.hpp"
#include "common/Entities/SuppliersProductInfo.hpp"
#include "common/Keys.hpp"
#include "common/SpdlogConfig.hpp"

ApiManager::ApiManager(NetworkService &networkService) : m_networkService(networkService)
{
    SPDLOG_TRACE("ApiManager::ApiManager");

    connect(&m_networkService, &NetworkService::connected, this, &ApiManager::ready);
    connect(&m_networkService, &NetworkService::responseReceived, this,
            &ApiManager::handleResponse);

    setupHandlers();
}

ApiManager::~ApiManager() { SPDLOG_TRACE("ApiManager::~ApiManager"); }

void ApiManager::loginUser(const QString &username, const QString &password)
{
    SPDLOG_TRACE("ApiManager::loginUser username={}", username.toStdString());

    Dataset dataset;
    dataset[Common::Entities::User::USERNAME_KEY] = {username.toStdString()};
    dataset[Common::Entities::User::PASSWORD_KEY] = {password.toStdString()};
    m_networkService.sendRequest(Endpoints::Users::LOGIN, Method::POST, dataset);
}

void ApiManager::getUsers()
{
    SPDLOG_TRACE("ApiManager::getUsers");

    m_networkService.sendRequest(Endpoints::Users::USERS, Method::GET);
}

void ApiManager::addUser(const Common::Entities::User &user)
{
    SPDLOG_TRACE("ApiManager::addUser");

    Dataset dataset;
    dataset[Common::Entities::User::ID_KEY]       = {user.id};
    dataset[Common::Entities::User::USERNAME_KEY] = {user.username};
    dataset[Common::Entities::User::PASSWORD_KEY] = {user.password};
    dataset[Common::Entities::User::ROLE_ID_KEY]  = {user.roleId};

    m_networkService.sendRequest(Endpoints::Users::USERS, Method::POST, dataset);
}

void ApiManager::editUser(const Common::Entities::User &user)
{
    SPDLOG_TRACE("ApiManager::editUser");

    Dataset dataset;
    dataset[Common::Entities::User::ID_KEY]       = {user.id};
    dataset[Common::Entities::User::USERNAME_KEY] = {user.username};
    dataset[Common::Entities::User::PASSWORD_KEY] = {user.password};
    dataset[Common::Entities::User::ROLE_ID_KEY]  = {user.roleId};

    m_networkService.sendRequest(Endpoints::Users::USERS, Method::PUT, dataset, user.id);
}

void ApiManager::deleteUser(const QString &id)
{
    SPDLOG_TRACE("ApiManager::deleteUser");

    m_networkService.sendRequest(Endpoints::Users::USERS, Method::DEL, {}, id.toStdString());
}

void ApiManager::getRoleList()
{
    SPDLOG_TRACE("ApiManager::getRole");

    m_networkService.sendRequest(Endpoints::Users::ROLES, Method::GET);
}

void ApiManager::createRole(const Common::Entities::Role &role)
{
    SPDLOG_TRACE("ApiManager::postRole");
    Dataset dataset;
    dataset[Common::Entities::Role::NAME_KEY] = {role.name};
    dataset[Common::Entities::Role::ID_KEY]   = {};

    m_networkService.sendRequest(Endpoints::Users::ROLES, Method::POST, dataset);
}

void ApiManager::editRole(const Common::Entities::Role &role)
{
    SPDLOG_TRACE("ApiManager::editRole");
    Dataset dataset;
    dataset[Common::Entities::Role::NAME_KEY] = {role.name};

    m_networkService.sendRequest(Endpoints::Users::ROLES, Method::PUT, dataset, role.id);
}

void ApiManager::deleteRole(const QString &id)
{
    SPDLOG_TRACE("ApiManager::deleteRole");

    m_networkService.sendRequest(Endpoints::Users::ROLES, Method::DEL, {}, id.toStdString());
}

void ApiManager::getProductTypes()
{
    SPDLOG_TRACE("ApiManager::getProductTypes");

    m_networkService.sendRequest(Endpoints::Inventory::PRODUCT_TYPES, Method::GET);
}

void ApiManager::createProductType(const Common::Entities::ProductType &productType)
{
    SPDLOG_TRACE("ApiManager::createProductType");

    Dataset dataset;
    dataset[Common::Entities::ProductType::CODE_KEY]        = {productType.code};
    dataset[Common::Entities::ProductType::BARCODE_KEY]     = {productType.barcode};
    dataset[Common::Entities::ProductType::NAME_KEY]        = {productType.name};
    dataset[Common::Entities::ProductType::DESCRIPTION_KEY] = {productType.description};
    dataset[Common::Entities::ProductType::LAST_PRICE_KEY]  = {productType.lastPrice};
    dataset[Common::Entities::ProductType::UNIT_KEY]        = {productType.unit};
    dataset[Common::Entities::ProductType::IS_IMPORTED_KEY] = {productType.isImported};

    m_networkService.sendRequest(Endpoints::Inventory::PRODUCT_TYPES, Method::POST, dataset);
}

void ApiManager::editProductType(const Common::Entities::ProductType &productType)
{
    SPDLOG_TRACE("ApiManager::editProductType");

    Dataset dataset;
    dataset[Common::Entities::ProductType::CODE_KEY]        = {productType.code};
    dataset[Common::Entities::ProductType::BARCODE_KEY]     = {productType.barcode};
    dataset[Common::Entities::ProductType::NAME_KEY]        = {productType.name};
    dataset[Common::Entities::ProductType::DESCRIPTION_KEY] = {productType.description};
    dataset[Common::Entities::ProductType::LAST_PRICE_KEY]  = {productType.lastPrice};
    dataset[Common::Entities::ProductType::UNIT_KEY]        = {productType.unit};
    dataset[Common::Entities::ProductType::IS_IMPORTED_KEY] = {productType.isImported};

    m_networkService.sendRequest(Endpoints::Inventory::PRODUCT_TYPES, Method::PUT, dataset,
                                 productType.id);
}

void ApiManager::deleteProductType(const QString &id)
{
    SPDLOG_TRACE("ApiManager::deleteProductType");

    m_networkService.sendRequest(Endpoints::Inventory::PRODUCT_TYPES, Method::DEL, {},
                                 id.toStdString());
}

void ApiManager::getStocks()
{
    SPDLOG_TRACE("ApiManager::getStocks");

    m_networkService.sendRequest(Endpoints::Inventory::STOCKS, Method::GET);
}

void ApiManager::createStock(const Common::Entities::Inventory &stock)
{
    SPDLOG_TRACE("ApiManager::createStock");

    Dataset dataset;
    dataset[Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY]    = {stock.productTypeId};
    dataset[Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY] = {stock.quantityAvailable};
    dataset[Common::Entities::Inventory::EMPLOYEE_ID_KEY]        = {stock.employeeId};

    m_networkService.sendRequest(Endpoints::Inventory::STOCKS, Method::POST, dataset);
}

void ApiManager::editStock(const Common::Entities::Inventory &stock)
{
    SPDLOG_TRACE("ApiManager::editStock");

    Dataset dataset;
    dataset[Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY]    = {stock.productTypeId};
    dataset[Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY] = {stock.quantityAvailable};
    dataset[Common::Entities::Inventory::EMPLOYEE_ID_KEY]        = {stock.employeeId};

    m_networkService.sendRequest(Endpoints::Inventory::STOCKS, Method::PUT, dataset, stock.id);
}

void ApiManager::deleteStock(const QString &id)
{
    SPDLOG_TRACE("ApiManager::deleteStock");

    m_networkService.sendRequest(Endpoints::Inventory::STOCKS, Method::DEL, {}, id.toStdString());
}

void ApiManager::getSupplierProducts()
{
    SPDLOG_TRACE("ApiManager::getSupplierProducts");
    m_networkService.sendRequest(Endpoints::Inventory::SUPPLIER_PRODUCTS, Method::GET);
}

void ApiManager::createSupplierProduct(
    const Common::Entities::SuppliersProductInfo &supplierProduct)
{
    SPDLOG_TRACE("ApiManager::createSupplierProduct");
    Dataset dataset;
    dataset[Common::Entities::SuppliersProductInfo::SUPPLIER_ID_KEY] = {supplierProduct.supplierID};
    dataset[Common::Entities::SuppliersProductInfo::PRODUCT_TYPE_ID_KEY] = {
        supplierProduct.productTypeId};
    dataset[Common::Entities::SuppliersProductInfo::CODE_KEY] = {supplierProduct.code};

    m_networkService.sendRequest(Endpoints::Inventory::SUPPLIER_PRODUCTS, Method::POST, dataset);
}

void ApiManager::editSupplierProduct(const Common::Entities::SuppliersProductInfo &supplierProduct)
{
    SPDLOG_TRACE("ApiManager::editSupplierProduct");
    Dataset dataset;
    dataset[Common::Entities::SuppliersProductInfo::SUPPLIER_ID_KEY] = {supplierProduct.supplierID};
    dataset[Common::Entities::SuppliersProductInfo::PRODUCT_TYPE_ID_KEY] = {
        supplierProduct.productTypeId};
    dataset[Common::Entities::SuppliersProductInfo::CODE_KEY] = {supplierProduct.code};

    m_networkService.sendRequest(Endpoints::Inventory::SUPPLIER_PRODUCTS, Method::PUT, dataset,
                                 supplierProduct.id);
}

void ApiManager::deleteSupplierProduct(const QString &id)
{
    SPDLOG_TRACE("ApiManager::deleteSupplierProduct");
    m_networkService.sendRequest(Endpoints::Inventory::SUPPLIER_PRODUCTS, Method::DEL, {},
                                 id.toStdString());
}

void ApiManager::getContacts()
{
    SPDLOG_TRACE("ApiManager::getContacts");
    m_networkService.sendRequest(Endpoints::Management::CONTACTS, Method::GET);
}

void ApiManager::createContact(const Common::Entities::Contact &contact)
{
    SPDLOG_TRACE("ApiManager::createContact");
    Dataset dataset;
    dataset[Common::Entities::Contact::NAME_KEY]  = {contact.name};
    dataset[Common::Entities::Contact::TYPE_KEY]  = {contact.type};
    dataset[Common::Entities::Contact::EMAIL_KEY] = {contact.email};
    dataset[Common::Entities::Contact::PHONE_KEY] = {contact.phone};

    m_networkService.sendRequest(Endpoints::Management::CONTACTS, Method::POST, dataset);
}

void ApiManager::editContact(const Common::Entities::Contact &contact)
{
    SPDLOG_TRACE("ApiManager::editContact");
    Dataset dataset;
    dataset[Common::Entities::Contact::NAME_KEY]  = {contact.name};
    dataset[Common::Entities::Contact::TYPE_KEY]  = {contact.type};
    dataset[Common::Entities::Contact::EMAIL_KEY] = {contact.email};
    dataset[Common::Entities::Contact::PHONE_KEY] = {contact.phone};

    m_networkService.sendRequest(Endpoints::Management::CONTACTS, Method::PUT, dataset,
                                 contact.id);
}

void ApiManager::deleteContact(const QString &id)
{
    SPDLOG_TRACE("ApiManager::deleteContact");
    m_networkService.sendRequest(Endpoints::Management::CONTACTS, Method::DEL, {},
                                 id.toStdString());
}

void ApiManager::getSuppliers()
{
    SPDLOG_TRACE("ApiManager::getSuppliers");
    m_networkService.sendRequest(Endpoints::Management::SUPPLIERS, Method::GET);
}

void ApiManager::createSupplier(const Common::Entities::Supplier &supplier)
{
    SPDLOG_TRACE("ApiManager::createSupplier");
    Dataset dataset;
    dataset[Common::Entities::Supplier::NAME_KEY]    = {supplier.name};
    dataset[Common::Entities::Supplier::NUMBER_KEY]  = {supplier.number};
    dataset[Common::Entities::Supplier::EMAIL_KEY]   = {supplier.email};
    dataset[Common::Entities::Supplier::ADDRESS_KEY] = {supplier.address};

    m_networkService.sendRequest(Endpoints::Management::SUPPLIERS, Method::POST, dataset);
}

void ApiManager::editSupplier(const Common::Entities::Supplier &supplier)
{
    SPDLOG_TRACE("ApiManager::editSupplier");
    Dataset dataset;
    dataset[Common::Entities::Supplier::NAME_KEY]    = {supplier.name};
    dataset[Common::Entities::Supplier::NUMBER_KEY]  = {supplier.number};
    dataset[Common::Entities::Supplier::EMAIL_KEY]   = {supplier.email};
    dataset[Common::Entities::Supplier::ADDRESS_KEY] = {supplier.address};

    m_networkService.sendRequest(Endpoints::Management::SUPPLIERS, Method::PUT, dataset,
                                 supplier.id);
}

void ApiManager::deleteSupplier(const QString &id)
{
    SPDLOG_TRACE("ApiManager::deleteSupplier");
    m_networkService.sendRequest(Endpoints::Management::SUPPLIERS, Method::DEL, {},
                                 id.toStdString());
}

void ApiManager::getEmployees()
{
    SPDLOG_TRACE("ApiManager::getEmployees");
    m_networkService.sendRequest(Endpoints::Management::EMPLOYEES, Method::GET);
}

void ApiManager::createEmployee(const Common::Entities::Employee &employee)
{
    SPDLOG_TRACE("ApiManager::createEmployee");
    Dataset dataset;
    dataset[Common::Entities::Employee::NAME_KEY]    = {employee.name};
    dataset[Common::Entities::Employee::NUMBER_KEY]  = {employee.number};
    dataset[Common::Entities::Employee::EMAIL_KEY]   = {employee.email};
    dataset[Common::Entities::Employee::ADDRESS_KEY] = {employee.address};

    m_networkService.sendRequest(Endpoints::Management::EMPLOYEES, Method::POST, dataset);
}

void ApiManager::editEmployee(const Common::Entities::Employee &employee)
{
    SPDLOG_TRACE("ApiManager::editEmployee");
    Dataset dataset;
    dataset[Common::Entities::Employee::NAME_KEY]    = {employee.name};
    dataset[Common::Entities::Employee::NUMBER_KEY]  = {employee.number};
    dataset[Common::Entities::Employee::EMAIL_KEY]   = {employee.email};
    dataset[Common::Entities::Employee::ADDRESS_KEY] = {employee.address};

    m_networkService.sendRequest(Endpoints::Management::EMPLOYEES, Method::PUT, dataset,
                                 employee.id);
}

void ApiManager::deleteEmployee(const QString &id)
{
    SPDLOG_TRACE("ApiManager::deleteEmployee");
    m_networkService.sendRequest(Endpoints::Management::EMPLOYEES, Method::DEL, {},
                                 id.toStdString());
}

void ApiManager::getPurchaseOrders()
{
    SPDLOG_TRACE("ApiManager::getPurchaseOrders");
    m_networkService.sendRequest(Endpoints::Purchase::ORDERS, Method::GET);
}

void ApiManager::createPurchaseOrder(const Common::Entities::PurchaseOrder &order)
{
    SPDLOG_TRACE("ApiManager::createPurchaseOrder");
    Dataset dataset;
    dataset[Common::Entities::PurchaseOrder::DATE_KEY]        = {order.date};
    dataset[Common::Entities::PurchaseOrder::USER_ID_KEY]     = {order.userId};
    dataset[Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY] = {order.supplierId};
    dataset[Common::Entities::PurchaseOrder::STATUS_KEY]      = {order.status};

    m_networkService.sendRequest(Endpoints::Purchase::ORDERS, Method::POST, dataset);
}

void ApiManager::editPurchaseOrder(const Common::Entities::PurchaseOrder &order)
{
    SPDLOG_TRACE("ApiManager::editPurchaseOrder");
    Dataset dataset;
    dataset[Common::Entities::PurchaseOrder::DATE_KEY]        = {order.date};
    dataset[Common::Entities::PurchaseOrder::USER_ID_KEY]     = {order.userId};
    dataset[Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY] = {order.supplierId};
    dataset[Common::Entities::PurchaseOrder::STATUS_KEY]      = {order.status};

    m_networkService.sendRequest(Endpoints::Purchase::ORDERS, Method::PUT, dataset, order.id);
}

void ApiManager::deletePurchaseOrder(const QString &id)
{
    SPDLOG_TRACE("ApiManager::deletePurchaseOrder");
    m_networkService.sendRequest(Endpoints::Purchase::ORDERS, Method::DEL, {}, id.toStdString());
}

void ApiManager::getPurchaseOrderRecords(const QString &orderId)
{
    SPDLOG_TRACE("ApiManager::getPurchaseOrderRecords");
    m_networkService.sendRequest(Endpoints::Purchase::ORDER_RECORDS, Method::GET, {},
                                 orderId.toStdString());
}

void ApiManager::createPurchaseOrderRecord(const Common::Entities::PurchaseOrderRecord &record)
{
    SPDLOG_TRACE("ApiManager::createPurchaseOrderRecord");
    Dataset dataset;
    dataset[Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY]        = {record.orderId};
    dataset[Common::Entities::PurchaseOrderRecord::PRODUCT_TYPE_ID_KEY] = {record.productTypeId};
    dataset[Common::Entities::PurchaseOrderRecord::QUANTITY_KEY]        = {record.quantity};
    dataset[Common::Entities::PurchaseOrderRecord::PRICE_KEY]           = {record.price};

    m_networkService.sendRequest(Endpoints::Purchase::ORDER_RECORDS, Method::POST, dataset);
}

void ApiManager::editPurchaseOrderRecord(const Common::Entities::PurchaseOrderRecord &record)
{
    SPDLOG_TRACE("ApiManager::editPurchaseOrderRecord");
    Dataset dataset;
    dataset[Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY]        = {record.orderId};
    dataset[Common::Entities::PurchaseOrderRecord::PRODUCT_TYPE_ID_KEY] = {record.productTypeId};
    dataset[Common::Entities::PurchaseOrderRecord::QUANTITY_KEY]        = {record.quantity};
    dataset[Common::Entities::PurchaseOrderRecord::PRICE_KEY]           = {record.price};

    m_networkService.sendRequest(Endpoints::Purchase::ORDER_RECORDS, Method::PUT, dataset,
                                 record.id);
}

void ApiManager::deletePurchaseOrderRecord(const QString &id)
{
    SPDLOG_TRACE("ApiManager::deletePurchaseOrderRecord");
    m_networkService.sendRequest(Endpoints::Purchase::ORDER_RECORDS, Method::DEL, {},
                                 id.toStdString());
}

void ApiManager::postPurchaseReceipt(const QString &orderId, const QString &employeeId)
{
    SPDLOG_TRACE("ApiManager::postPurchaseReceipt");
    Dataset dataset;
    dataset[Common::Entities::PurchaseOrder::ID_KEY]       = {orderId.toStdString()};
    dataset[Common::Entities::Inventory::EMPLOYEE_ID_KEY]  = {employeeId.toStdString()};

    m_networkService.sendRequest(Endpoints::Purchase::RECEIPTS, Method::POST, dataset);
}

void ApiManager::getSalesOrders()
{
    SPDLOG_TRACE("ApiManager::getSalesOrders");
    m_networkService.sendRequest(Endpoints::Sales::ORDERS, Method::GET);
}

void ApiManager::createSalesOrder(const Common::Entities::SaleOrder &order)
{
    SPDLOG_TRACE("ApiManager::createSalesOrder");
    Dataset dataset;
    dataset[Common::Entities::SaleOrder::DATE_KEY]        = {order.date};
    dataset[Common::Entities::SaleOrder::USER_ID_KEY]     = {order.userId};
    dataset[Common::Entities::SaleOrder::CONTACT_ID_KEY]  = {order.contactId};
    dataset[Common::Entities::SaleOrder::EMPLOYEE_ID_KEY] = {order.employeeId};
    dataset[Common::Entities::SaleOrder::STATUS_KEY]      = {order.status};

    m_networkService.sendRequest(Endpoints::Sales::ORDERS, Method::POST, dataset);
}

void ApiManager::editSalesOrder(const Common::Entities::SaleOrder &order)
{
    SPDLOG_TRACE("ApiManager::editSalesOrder");
    Dataset dataset;
    dataset[Common::Entities::SaleOrder::DATE_KEY]        = {order.date};
    dataset[Common::Entities::SaleOrder::USER_ID_KEY]     = {order.userId};
    dataset[Common::Entities::SaleOrder::CONTACT_ID_KEY]  = {order.contactId};
    dataset[Common::Entities::SaleOrder::EMPLOYEE_ID_KEY] = {order.employeeId};
    dataset[Common::Entities::SaleOrder::STATUS_KEY]      = {order.status};

    m_networkService.sendRequest(Endpoints::Sales::ORDERS, Method::PUT, dataset, order.id);
}

void ApiManager::deleteSalesOrder(const QString &id)
{
    SPDLOG_TRACE("ApiManager::deleteSalesOrder");
    m_networkService.sendRequest(Endpoints::Sales::ORDERS, Method::DEL, {}, id.toStdString());
}

void ApiManager::getSalesOrderRecords(const QString &orderId)
{
    SPDLOG_TRACE("ApiManager::getSalesOrderRecords");
    m_networkService.sendRequest(Endpoints::Sales::ORDER_RECORDS, Method::GET, {},
                                 orderId.toStdString());
}

void ApiManager::createSalesOrderRecord(const Common::Entities::SalesOrderRecord &record)
{
    SPDLOG_TRACE("ApiManager::createSalesOrderRecord");
    Dataset dataset;
    dataset[Common::Entities::SalesOrderRecord::ORDER_ID_KEY]        = {record.orderId};
    dataset[Common::Entities::SalesOrderRecord::PRODUCT_TYPE_ID_KEY] = {record.productTypeId};
    dataset[Common::Entities::SalesOrderRecord::QUANTITY_KEY]        = {record.quantity};
    dataset[Common::Entities::SalesOrderRecord::PRICE_KEY]           = {record.price};

    m_networkService.sendRequest(Endpoints::Sales::ORDER_RECORDS, Method::POST, dataset);
}

void ApiManager::editSalesOrderRecord(const Common::Entities::SalesOrderRecord &record)
{
    SPDLOG_TRACE("ApiManager::editSalesOrderRecord");
    Dataset dataset;
    dataset[Common::Entities::SalesOrderRecord::ORDER_ID_KEY]        = {record.orderId};
    dataset[Common::Entities::SalesOrderRecord::PRODUCT_TYPE_ID_KEY] = {record.productTypeId};
    dataset[Common::Entities::SalesOrderRecord::QUANTITY_KEY]        = {record.quantity};
    dataset[Common::Entities::SalesOrderRecord::PRICE_KEY]           = {record.price};

    m_networkService.sendRequest(Endpoints::Sales::ORDER_RECORDS, Method::PUT, dataset, record.id);
}

void ApiManager::deleteSalesOrderRecord(const QString &id)
{
    SPDLOG_TRACE("ApiManager::deleteSalesOrderRecord");
    m_networkService.sendRequest(Endpoints::Sales::ORDER_RECORDS, Method::DEL, {}, id.toStdString());
}

void ApiManager::getLogs()
{
    SPDLOG_TRACE("ApiManager::getLogs");
    m_networkService.sendRequest(Endpoints::Logs::ENTRIES, Method::GET);
}

void ApiManager::getSalesAnalytics()
{
    SPDLOG_TRACE("ApiManager::getSalesAnalytics");
    m_networkService.sendRequest(Endpoints::Analytics::SALES, Method::GET);
}

void ApiManager::getInventoryAnalytics()
{
    SPDLOG_TRACE("ApiManager::getInventoryAnalytics");
    m_networkService.sendRequest(Endpoints::Analytics::INVENTORY, Method::GET);
}

void ApiManager::setupHandlers()
{
    SPDLOG_TRACE("ApiManager::setupHandlers");
    m_responseHandlers[Endpoints::Users::LOGIN] = [this](Method method, const Dataset &dataset) {
        handleLoginResponse(method, dataset);
    };
    m_responseHandlers[Endpoints::Users::USERS] = [this](Method method, const Dataset &dataset) {
        handleUsers(method, dataset);
    };
    m_responseHandlers[Endpoints::Users::ROLES] = [this](Method method, const Dataset &dataset) {
        handleRoles(method, dataset);
    };
    m_responseHandlers[Endpoints::Inventory::PRODUCT_TYPES] =
        [this](Method method, const Dataset &dataset) { handleProductTypes(method, dataset); };
    m_responseHandlers[Endpoints::Inventory::STOCKS] =
        [this](Method method, const Dataset &dataset) { handleStocks(method, dataset); };
    m_responseHandlers[Endpoints::Inventory::SUPPLIER_PRODUCTS] =
        [this](Method method, const Dataset &dataset) { handleSupplierProducts(method, dataset); };
    m_responseHandlers[Endpoints::Management::CONTACTS] =
        [this](Method method, const Dataset &dataset) { handleContacts(method, dataset); };
    m_responseHandlers[Endpoints::Management::SUPPLIERS] =
        [this](Method method, const Dataset &dataset) { handleSuppliers(method, dataset); };
    m_responseHandlers[Endpoints::Management::EMPLOYEES] =
        [this](Method method, const Dataset &dataset) { handleEmployees(method, dataset); };
    m_responseHandlers[Endpoints::Purchase::ORDERS] =
        [this](Method method, const Dataset &dataset) { handlePurchaseOrders(method, dataset); };
    m_responseHandlers[Endpoints::Purchase::ORDER_RECORDS] =
        [this](Method method, const Dataset &dataset) { handlePurchaseOrderRecords(method, dataset); };
    m_responseHandlers[Endpoints::Purchase::RECEIPTS] =
        [this](Method method, const Dataset &dataset) {
            Q_UNUSED(dataset);
            if (method == Method::POST) {
                emit purchaseReceiptPosted();
            } else {
                SPDLOG_WARN("ApiManager::setupHandlers | Unexpected method for purchase receipts");
            }
        };
    m_responseHandlers[Endpoints::Sales::ORDERS] =
        [this](Method method, const Dataset &dataset) { handleSalesOrders(method, dataset); };
    m_responseHandlers[Endpoints::Sales::ORDER_RECORDS] =
        [this](Method method, const Dataset &dataset) { handleSalesOrderRecords(method, dataset); };
    m_responseHandlers[Endpoints::Logs::ENTRIES] =
        [this](Method method, const Dataset &dataset) { handleLogs(method, dataset); };
    m_responseHandlers[Endpoints::Analytics::SALES] =
        [this](Method method, const Dataset &dataset) {
            handleSalesAnalyticsResponse(method, dataset);
        };
    m_responseHandlers[Endpoints::Analytics::INVENTORY] =
        [this](Method method, const Dataset &dataset) {
            handleInventoryAnalyticsResponse(method, dataset);
        };
}

void ApiManager::handleResponse(const std::string &endpoint, Method method, const Dataset &dataset)
{
    SPDLOG_DEBUG("ApiManager::handleResponse for endpoint={}", endpoint);
    auto handler = m_responseHandlers.find(endpoint);
    if (handler != m_responseHandlers.end()) {
        std::string errorMsg{};
        if (dataset.contains(Keys::_ERROR)) {
            for (const auto &errorData : dataset.at(Keys::_ERROR)) {
                errorMsg += errorData + "\n";
            }
        }

        if (errorMsg.empty()) {
            handler->second(method, dataset);
        } else {
            handleError(errorMsg);
        }
    } else {
        SPDLOG_ERROR("ApiManager::handleResponse | Can't find handler for endpoint={}", endpoint);
    }
}

void ApiManager::handleError(const std::string &errorMessage)
{
    SPDLOG_TRACE("ApiManager::handleError | {}", errorMessage);
    emit errorOccurred(QString::fromStdString(errorMessage));
}

void ApiManager::handleRoles(Method method, const Dataset &dataset)
{
    switch (method) {
    case Method::GET:
        handleRolesList(dataset);
        break;
    case Method::POST:
        emit roleCreated();
        break;
    case Method::PUT:
        emit roleEdited();
        break;
    case Method::DEL:
        emit roleDeleted();
        break;
    default:
        SPDLOG_WARN("ApiManager::handleRoles | Got unhandled method");
        break;
    }
}

void ApiManager::handleProductTypes(Method method, const Dataset &dataset)
{
    switch (method) {
    case Method::GET:
        handleProductTypesList(dataset);
        break;
    case Method::POST:
        emit productTypeCreated();
        break;
    case Method::PUT:
        emit productTypeEdited();
        break;
    case Method::DEL:
        emit productTypeDeleted();
        break;
    default:
        SPDLOG_WARN("ApiManager::handleProductTypes | Got unhandled method");
        break;
    }
}

void ApiManager::handleStocks(Method method, const Dataset &dataset)
{
    switch (method) {
    case Method::GET:
        handleStocksList(dataset);
        break;
    case Method::POST:
        emit stockCreated();
        break;
    case Method::PUT:
        emit stockEdited();
        break;
    case Method::DEL:
        emit stockDeleted();
        break;
    default:
        SPDLOG_WARN("ApiManager::handleStocks | Got unhandled method");
        break;
    }
}

void ApiManager::handleSupplierProducts(Method method, const Dataset &dataset)
{
    switch (method) {
    case Method::GET:
        handleSupplierProductsList(dataset);
        break;
    case Method::POST:
        emit supplierProductCreated();
        break;
    case Method::PUT:
        emit supplierProductEdited();
        break;
    case Method::DEL:
        emit supplierProductDeleted();
        break;
    default:
        SPDLOG_WARN("ApiManager::handleSupplierProducts | Got unhandled method");
        break;
    }
}

void ApiManager::handleContacts(Method method, const Dataset &dataset)
{
    switch (method) {
    case Method::GET:
        handleContactsList(dataset);
        break;
    case Method::POST:
        emit contactCreated();
        break;
    case Method::PUT:
        emit contactEdited();
        break;
    case Method::DEL:
        emit contactDeleted();
        break;
    default:
        SPDLOG_WARN("ApiManager::handleContacts | Got unhandled method");
        break;
    }
}

void ApiManager::handleSuppliers(Method method, const Dataset &dataset)
{
    switch (method) {
    case Method::GET:
        handleSuppliersList(dataset);
        break;
    case Method::POST:
        emit supplierCreated();
        break;
    case Method::PUT:
        emit supplierEdited();
        break;
    case Method::DEL:
        emit supplierDeleted();
        break;
    default:
        SPDLOG_WARN("ApiManager::handleSuppliers | Got unhandled method");
        break;
    }
}

void ApiManager::handleEmployees(Method method, const Dataset &dataset)
{
    switch (method) {
    case Method::GET:
        handleEmployeesList(dataset);
        break;
    case Method::POST:
        emit employeeCreated();
        break;
    case Method::PUT:
        emit employeeEdited();
        break;
    case Method::DEL:
        emit employeeDeleted();
        break;
    default:
        SPDLOG_WARN("ApiManager::handleEmployees | Got unhandled method");
        break;
    }
}

void ApiManager::handlePurchaseOrders(Method method, const Dataset &dataset)
{
    switch (method) {
    case Method::GET:
        handlePurchaseOrdersList(dataset);
        break;
    case Method::POST:
        emit purchaseOrderCreated();
        break;
    case Method::PUT:
        emit purchaseOrderEdited();
        break;
    case Method::DEL:
        emit purchaseOrderDeleted();
        break;
    default:
        SPDLOG_WARN("ApiManager::handlePurchaseOrders | Got unhandled method");
        break;
    }
}

void ApiManager::handlePurchaseOrderRecords(Method method, const Dataset &dataset)
{
    switch (method) {
    case Method::GET:
        handlePurchaseOrderRecordsList(dataset);
        break;
    case Method::POST:
        emit purchaseOrderRecordCreated();
        break;
    case Method::PUT:
        emit purchaseOrderRecordEdited();
        break;
    case Method::DEL:
        emit purchaseOrderRecordDeleted();
        break;
    default:
        SPDLOG_WARN("ApiManager::handlePurchaseOrderRecords | Got unhandled method");
        break;
    }
}

void ApiManager::handleSalesOrders(Method method, const Dataset &dataset)
{
    switch (method) {
    case Method::GET:
        handleSalesOrdersList(dataset);
        break;
    case Method::POST:
        emit salesOrderCreated();
        break;
    case Method::PUT:
        emit salesOrderEdited();
        break;
    case Method::DEL:
        emit salesOrderDeleted();
        break;
    default:
        SPDLOG_WARN("ApiManager::handleSalesOrders | Got unhandled method");
        break;
    }
}

void ApiManager::handleSalesOrderRecords(Method method, const Dataset &dataset)
{
    switch (method) {
    case Method::GET:
        handleSalesOrderRecordsList(dataset);
        break;
    case Method::POST:
        emit salesOrderRecordCreated();
        break;
    case Method::PUT:
        emit salesOrderRecordEdited();
        break;
    case Method::DEL:
        emit salesOrderRecordDeleted();
        break;
    default:
        SPDLOG_WARN("ApiManager::handleSalesOrderRecords | Got unhandled method");
        break;
    }
}

void ApiManager::handleLogs(Method method, const Dataset &dataset)
{
    if (method == Method::GET) {
        handleLogsList(dataset);
        return;
    }

    SPDLOG_WARN("ApiManager::handleLogs | Unexpected non-GET method");
}

void ApiManager::handleSalesAnalyticsResponse(Method method, const Dataset &dataset)
{
    if (method != Method::GET) {
        SPDLOG_WARN("ApiManager::handleSalesAnalyticsResponse | Unexpected non-GET method");
        return;
    }

    emit salesAnalyticsReady(buildAnalyticsMetrics(dataset));
}

void ApiManager::handleInventoryAnalyticsResponse(Method method, const Dataset &dataset)
{
    if (method != Method::GET) {
        SPDLOG_WARN("ApiManager::handleInventoryAnalyticsResponse | Unexpected non-GET method");
        return;
    }

    emit inventoryAnalyticsReady(buildAnalyticsMetrics(dataset));
}

void ApiManager::handleLoginResponse(Method, const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleLoginResponse");

    try {
        const auto &id_list      = dataset.at(Common::Entities::User::ID_KEY);
        const auto &role_id_list = dataset.at(Common::Entities::User::ROLE_ID_KEY);

        if (id_list.empty() || role_id_list.empty()) {
            handleError("ApiManager::handleLoginResponse | empty lists");
            return;
        }

        const std::string &id     = id_list.front();
        const std::string &roleId = role_id_list.front();

        if (id.empty() || roleId.empty()) {
            handleError("ApiManager::handleLoginResponse | empty id or roleId");
        } else {
            emit loginSuccess(QString::fromStdString(id), QString::fromStdString(roleId));
        }
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("Missing required keys in dataset: {}", e.what());
        handleError("ApiManager::handleLoginResponse | missing key in dataset");
    }
}

void ApiManager::handleUsers(Method method, const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleUsers");

    switch (method) {
    case Method::GET:
        handleUsersList(dataset);
        break;
    case Method::POST:
        emit userAdded();
        break;
    case Method::PUT:
        emit userEdited();
        break;
    case Method::DEL:
        emit userDeleted();
        break;
    default:
        SPDLOG_WARN("ApiManager::handleRoles | Got unhandled method");
        break;
    }
}

void ApiManager::handleRolesList(const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleRolesList");

    try {
        if (!dataset.contains(Common::Entities::Role::ID_KEY) ||
            !dataset.contains(Common::Entities::Role::NAME_KEY)) {
            handleError("dataset doesn't contain ID or NAME lists");
            return;
        }

        const auto &idList   = dataset.at(Common::Entities::Role::ID_KEY);
        const auto &nameList = dataset.at(Common::Entities::Role::NAME_KEY);

        if (idList.size() != nameList.size()) {
            handleError("ID and Name lists have different sizes.");
            return;
        }

        std::vector<Common::Entities::Role> roles;
        auto idIt   = idList.begin();
        auto nameIt = nameList.begin();

        while (idIt != idList.end() && nameIt != nameList.end()) {
            Common::Entities::Role role{.id = *idIt, .name = *nameIt};
            roles.push_back(role);

            ++idIt;
            ++nameIt;
        }

        emit rolesList(roles);
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("handleRolesList | Key missing in dataset: {}", e.what());
        handleError("Key missing in dataset");
    }
}

void ApiManager::handleUsersList(const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleUsersList");

    try {
        if (!dataset.contains(Common::Entities::User::ID_KEY) ||
            !dataset.contains(Common::Entities::User::USERNAME_KEY) ||
            !dataset.contains(Common::Entities::User::ROLE_ID_KEY)) {
            handleError("Dataset doesn't contain required User keys");
            return;
        }

        const auto &idList       = dataset.at(Common::Entities::User::ID_KEY);
        const auto &usernameList = dataset.at(Common::Entities::User::USERNAME_KEY);
        const auto &roleIdList   = dataset.at(Common::Entities::User::ROLE_ID_KEY);

        if (idList.size() != usernameList.size() || usernameList.size() != roleIdList.size()) {
            handleError("User lists have different sizes.");
            return;
        }

        std::vector<Common::Entities::User> users;
        auto idIt       = idList.begin();
        auto usernameIt = usernameList.begin();
        auto roleIdIt   = roleIdList.begin();

        while (idIt != idList.end() && usernameIt != usernameList.end() &&
               roleIdIt != roleIdList.end()) {
            Common::Entities::User user{.id = *idIt, .username = *usernameIt, .roleId = *roleIdIt};
            users.push_back(user);

            ++idIt;
            ++usernameIt;
            ++roleIdIt;
        }

        emit usersList(users);
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("handleUsersList | Key missing in dataset: {}", e.what());
        handleError("Key missing in dataset");
    }
}

void ApiManager::handleProductTypesList(const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleProductTypesList");

    try {
        const auto &idList          = dataset.at(Common::Entities::ProductType::ID_KEY);
        const auto &codeList        = dataset.at(Common::Entities::ProductType::CODE_KEY);
        const auto &barcodeList     = dataset.at(Common::Entities::ProductType::BARCODE_KEY);
        const auto &nameList        = dataset.at(Common::Entities::ProductType::NAME_KEY);
        const auto &descriptionList = dataset.at(Common::Entities::ProductType::DESCRIPTION_KEY);
        const auto &lastPriceList   = dataset.at(Common::Entities::ProductType::LAST_PRICE_KEY);
        const auto &unitList        = dataset.at(Common::Entities::ProductType::UNIT_KEY);
        const auto &isImportedList  = dataset.at(Common::Entities::ProductType::IS_IMPORTED_KEY);

        const auto itemCount = idList.size();
        if (codeList.size() != itemCount || barcodeList.size() != itemCount ||
            nameList.size() != itemCount || descriptionList.size() != itemCount ||
            lastPriceList.size() != itemCount || unitList.size() != itemCount ||
            isImportedList.size() != itemCount) {
            handleError("ProductType lists have different sizes.");
            return;
        }

        std::vector<Common::Entities::ProductType> productTypes;
        auto idIt          = idList.begin();
        auto codeIt        = codeList.begin();
        auto barcodeIt     = barcodeList.begin();
        auto nameIt        = nameList.begin();
        auto descriptionIt = descriptionList.begin();
        auto lastPriceIt   = lastPriceList.begin();
        auto unitIt        = unitList.begin();
        auto importedIt    = isImportedList.begin();

        while (idIt != idList.end()) {
            productTypes.push_back(Common::Entities::ProductType{.id = *idIt,
                                                                 .code = *codeIt,
                                                                 .barcode = *barcodeIt,
                                                                 .name = *nameIt,
                                                                 .description = *descriptionIt,
                                                                 .lastPrice = *lastPriceIt,
                                                                 .unit = *unitIt,
                                                                 .isImported = *importedIt});
            ++idIt;
            ++codeIt;
            ++barcodeIt;
            ++nameIt;
            ++descriptionIt;
            ++lastPriceIt;
            ++unitIt;
            ++importedIt;
        }

        emit productTypesList(productTypes);
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("handleProductTypesList | Key missing in dataset: {}", e.what());
        handleError("Key missing in dataset");
    }
}

void ApiManager::handleStocksList(const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleStocksList");

    try {
        const auto &idList = dataset.at(Common::Entities::Inventory::ID_KEY);
        const auto &productTypeIdList = dataset.at(Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY);
        const auto &quantityList = dataset.at(Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY);
        const auto &employeeIdList = dataset.at(Common::Entities::Inventory::EMPLOYEE_ID_KEY);

        const auto itemCount = idList.size();
        if (productTypeIdList.size() != itemCount || quantityList.size() != itemCount ||
            employeeIdList.size() != itemCount) {
            handleError("Inventory lists have different sizes.");
            return;
        }

        std::vector<Common::Entities::Inventory> stocks;
        auto idIt          = idList.begin();
        auto productTypeIt = productTypeIdList.begin();
        auto quantityIt    = quantityList.begin();
        auto employeeIt    = employeeIdList.begin();

        while (idIt != idList.end()) {
            stocks.push_back(Common::Entities::Inventory{.id = *idIt,
                                                         .productTypeId = *productTypeIt,
                                                         .quantityAvailable = *quantityIt,
                                                         .employeeId = *employeeIt});
            ++idIt;
            ++productTypeIt;
            ++quantityIt;
            ++employeeIt;
        }

        emit stocksList(stocks);
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("handleStocksList | Key missing in dataset: {}", e.what());
        handleError("Key missing in dataset");
    }
}

void ApiManager::handleSupplierProductsList(const Dataset &dataset)
{
    try {
        const auto &idList = dataset.at(Common::Entities::SuppliersProductInfo::ID_KEY);
        const auto &supplierIdList =
            dataset.at(Common::Entities::SuppliersProductInfo::SUPPLIER_ID_KEY);
        const auto &productTypeIdList =
            dataset.at(Common::Entities::SuppliersProductInfo::PRODUCT_TYPE_ID_KEY);
        const auto &codeList = dataset.at(Common::Entities::SuppliersProductInfo::CODE_KEY);

        const auto itemCount = idList.size();
        if (supplierIdList.size() != itemCount || productTypeIdList.size() != itemCount ||
            codeList.size() != itemCount) {
            handleError("Supplier product lists have different sizes.");
            return;
        }

        std::vector<Common::Entities::SuppliersProductInfo> supplierProducts;
        auto idIt            = idList.begin();
        auto supplierIdIt    = supplierIdList.begin();
        auto productTypeIdIt = productTypeIdList.begin();
        auto codeIt          = codeList.begin();
        while (idIt != idList.end()) {
            supplierProducts.push_back(Common::Entities::SuppliersProductInfo{
                .id = *idIt,
                .supplierID = *supplierIdIt,
                .productTypeId = *productTypeIdIt,
                .code = *codeIt});
            ++idIt;
            ++supplierIdIt;
            ++productTypeIdIt;
            ++codeIt;
        }

        emit supplierProductsList(supplierProducts);
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("handleSupplierProductsList | Key missing in dataset: {}", e.what());
        handleError("Key missing in dataset");
    }
}

void ApiManager::handleContactsList(const Dataset &dataset)
{
    try {
        const auto &idList    = dataset.at(Common::Entities::Contact::ID_KEY);
        const auto &nameList  = dataset.at(Common::Entities::Contact::NAME_KEY);
        const auto &typeList  = dataset.at(Common::Entities::Contact::TYPE_KEY);
        const auto &emailList = dataset.at(Common::Entities::Contact::EMAIL_KEY);
        const auto &phoneList = dataset.at(Common::Entities::Contact::PHONE_KEY);

        const auto itemCount = idList.size();
        if (nameList.size() != itemCount || typeList.size() != itemCount ||
            emailList.size() != itemCount || phoneList.size() != itemCount) {
            handleError("Contact lists have different sizes.");
            return;
        }

        std::vector<Common::Entities::Contact> contacts;
        auto idIt    = idList.begin();
        auto nameIt  = nameList.begin();
        auto typeIt  = typeList.begin();
        auto emailIt = emailList.begin();
        auto phoneIt = phoneList.begin();
        while (idIt != idList.end()) {
            contacts.push_back(Common::Entities::Contact{.id = *idIt,
                                                         .name = *nameIt,
                                                         .type = *typeIt,
                                                         .email = *emailIt,
                                                         .phone = *phoneIt});
            ++idIt;
            ++nameIt;
            ++typeIt;
            ++emailIt;
            ++phoneIt;
        }

        emit contactsList(contacts);
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("handleContactsList | Key missing in dataset: {}", e.what());
        handleError("Key missing in dataset");
    }
}

void ApiManager::handleSuppliersList(const Dataset &dataset)
{
    try {
        const auto &idList      = dataset.at(Common::Entities::Supplier::ID_KEY);
        const auto &nameList    = dataset.at(Common::Entities::Supplier::NAME_KEY);
        const auto &numberList  = dataset.at(Common::Entities::Supplier::NUMBER_KEY);
        const auto &emailList   = dataset.at(Common::Entities::Supplier::EMAIL_KEY);
        const auto &addressList = dataset.at(Common::Entities::Supplier::ADDRESS_KEY);

        const auto itemCount = idList.size();
        if (nameList.size() != itemCount || numberList.size() != itemCount ||
            emailList.size() != itemCount || addressList.size() != itemCount) {
            handleError("Supplier lists have different sizes.");
            return;
        }

        std::vector<Common::Entities::Supplier> suppliers;
        auto idIt      = idList.begin();
        auto nameIt    = nameList.begin();
        auto numberIt  = numberList.begin();
        auto emailIt   = emailList.begin();
        auto addressIt = addressList.begin();
        while (idIt != idList.end()) {
            suppliers.push_back(Common::Entities::Supplier{.id = *idIt,
                                                           .name = *nameIt,
                                                           .number = *numberIt,
                                                           .email = *emailIt,
                                                           .address = *addressIt});
            ++idIt;
            ++nameIt;
            ++numberIt;
            ++emailIt;
            ++addressIt;
        }

        emit suppliersList(suppliers);
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("handleSuppliersList | Key missing in dataset: {}", e.what());
        handleError("Key missing in dataset");
    }
}

void ApiManager::handleEmployeesList(const Dataset &dataset)
{
    try {
        const auto &idList      = dataset.at(Common::Entities::Employee::ID_KEY);
        const auto &nameList    = dataset.at(Common::Entities::Employee::NAME_KEY);
        const auto &numberList  = dataset.at(Common::Entities::Employee::NUMBER_KEY);
        const auto &emailList   = dataset.at(Common::Entities::Employee::EMAIL_KEY);
        const auto &addressList = dataset.at(Common::Entities::Employee::ADDRESS_KEY);

        const auto itemCount = idList.size();
        if (nameList.size() != itemCount || numberList.size() != itemCount ||
            emailList.size() != itemCount || addressList.size() != itemCount) {
            handleError("Employee lists have different sizes.");
            return;
        }

        std::vector<Common::Entities::Employee> employees;
        auto idIt      = idList.begin();
        auto nameIt    = nameList.begin();
        auto numberIt  = numberList.begin();
        auto emailIt   = emailList.begin();
        auto addressIt = addressList.begin();
        while (idIt != idList.end()) {
            employees.push_back(Common::Entities::Employee{.id = *idIt,
                                                           .name = *nameIt,
                                                           .number = *numberIt,
                                                           .email = *emailIt,
                                                           .address = *addressIt});
            ++idIt;
            ++nameIt;
            ++numberIt;
            ++emailIt;
            ++addressIt;
        }

        emit employeesList(employees);
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("handleEmployeesList | Key missing in dataset: {}", e.what());
        handleError("Key missing in dataset");
    }
}

void ApiManager::handlePurchaseOrdersList(const Dataset &dataset)
{
    try {
        const auto &idList = dataset.at(Common::Entities::PurchaseOrder::ID_KEY);
        const auto &dateList = dataset.at(Common::Entities::PurchaseOrder::DATE_KEY);
        const auto &userIdList = dataset.at(Common::Entities::PurchaseOrder::USER_ID_KEY);
        const auto &supplierIdList = dataset.at(Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY);
        const auto &statusList = dataset.at(Common::Entities::PurchaseOrder::STATUS_KEY);

        const auto itemCount = idList.size();
        if (dateList.size() != itemCount || userIdList.size() != itemCount ||
            supplierIdList.size() != itemCount || statusList.size() != itemCount) {
            handleError("Purchase order lists have different sizes.");
            return;
        }

        std::vector<Common::Entities::PurchaseOrder> orders;
        auto idIt         = idList.begin();
        auto dateIt       = dateList.begin();
        auto userIdIt     = userIdList.begin();
        auto supplierIdIt = supplierIdList.begin();
        auto statusIt     = statusList.begin();
        while (idIt != idList.end()) {
            orders.push_back(Common::Entities::PurchaseOrder{.id = *idIt,
                                                             .date = *dateIt,
                                                             .userId = *userIdIt,
                                                             .supplierId = *supplierIdIt,
                                                             .status = *statusIt});
            ++idIt;
            ++dateIt;
            ++userIdIt;
            ++supplierIdIt;
            ++statusIt;
        }

        emit purchaseOrdersList(orders);
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("handlePurchaseOrdersList | Key missing in dataset: {}", e.what());
        handleError("Key missing in dataset");
    }
}

void ApiManager::handlePurchaseOrderRecordsList(const Dataset &dataset)
{
    try {
        const auto &idList = dataset.at(Common::Entities::PurchaseOrderRecord::ID_KEY);
        const auto &orderIdList = dataset.at(Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY);
        const auto &productTypeIdList =
            dataset.at(Common::Entities::PurchaseOrderRecord::PRODUCT_TYPE_ID_KEY);
        const auto &quantityList = dataset.at(Common::Entities::PurchaseOrderRecord::QUANTITY_KEY);
        const auto &priceList = dataset.at(Common::Entities::PurchaseOrderRecord::PRICE_KEY);

        const auto itemCount = idList.size();
        if (orderIdList.size() != itemCount || productTypeIdList.size() != itemCount ||
            quantityList.size() != itemCount || priceList.size() != itemCount) {
            handleError("Purchase order record lists have different sizes.");
            return;
        }

        std::vector<Common::Entities::PurchaseOrderRecord> records;
        auto idIt            = idList.begin();
        auto orderIdIt       = orderIdList.begin();
        auto productTypeIdIt = productTypeIdList.begin();
        auto quantityIt      = quantityList.begin();
        auto priceIt         = priceList.begin();
        while (idIt != idList.end()) {
            records.push_back(Common::Entities::PurchaseOrderRecord{
                .id = *idIt,
                .orderId = *orderIdIt,
                .productTypeId = *productTypeIdIt,
                .quantity = *quantityIt,
                .price = *priceIt});
            ++idIt;
            ++orderIdIt;
            ++productTypeIdIt;
            ++quantityIt;
            ++priceIt;
        }

        emit purchaseOrderRecordsList(records);
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("handlePurchaseOrderRecordsList | Key missing in dataset: {}", e.what());
        handleError("Key missing in dataset");
    }
}

void ApiManager::handleSalesOrdersList(const Dataset &dataset)
{
    try {
        const auto &idList = dataset.at(Common::Entities::SaleOrder::ID_KEY);
        const auto &dateList = dataset.at(Common::Entities::SaleOrder::DATE_KEY);
        const auto &userIdList = dataset.at(Common::Entities::SaleOrder::USER_ID_KEY);
        const auto &contactIdList = dataset.at(Common::Entities::SaleOrder::CONTACT_ID_KEY);
        const auto &employeeIdList = dataset.at(Common::Entities::SaleOrder::EMPLOYEE_ID_KEY);
        const auto &statusList = dataset.at(Common::Entities::SaleOrder::STATUS_KEY);

        const auto itemCount = idList.size();
        if (dateList.size() != itemCount || userIdList.size() != itemCount ||
            contactIdList.size() != itemCount || employeeIdList.size() != itemCount ||
            statusList.size() != itemCount) {
            handleError("Sales order lists have different sizes.");
            return;
        }

        std::vector<Common::Entities::SaleOrder> orders;
        auto idIt         = idList.begin();
        auto dateIt       = dateList.begin();
        auto userIdIt     = userIdList.begin();
        auto contactIdIt  = contactIdList.begin();
        auto employeeIdIt = employeeIdList.begin();
        auto statusIt     = statusList.begin();
        while (idIt != idList.end()) {
            orders.push_back(Common::Entities::SaleOrder{.id = *idIt,
                                                         .date = *dateIt,
                                                         .userId = *userIdIt,
                                                         .contactId = *contactIdIt,
                                                         .employeeId = *employeeIdIt,
                                                         .status = *statusIt});
            ++idIt;
            ++dateIt;
            ++userIdIt;
            ++contactIdIt;
            ++employeeIdIt;
            ++statusIt;
        }

        emit salesOrdersList(orders);
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("handleSalesOrdersList | Key missing in dataset: {}", e.what());
        handleError("Key missing in dataset");
    }
}

void ApiManager::handleSalesOrderRecordsList(const Dataset &dataset)
{
    try {
        const auto &idList = dataset.at(Common::Entities::SalesOrderRecord::ID_KEY);
        const auto &orderIdList = dataset.at(Common::Entities::SalesOrderRecord::ORDER_ID_KEY);
        const auto &productTypeIdList =
            dataset.at(Common::Entities::SalesOrderRecord::PRODUCT_TYPE_ID_KEY);
        const auto &quantityList = dataset.at(Common::Entities::SalesOrderRecord::QUANTITY_KEY);
        const auto &priceList = dataset.at(Common::Entities::SalesOrderRecord::PRICE_KEY);

        const auto itemCount = idList.size();
        if (orderIdList.size() != itemCount || productTypeIdList.size() != itemCount ||
            quantityList.size() != itemCount || priceList.size() != itemCount) {
            handleError("Sales order record lists have different sizes.");
            return;
        }

        std::vector<Common::Entities::SalesOrderRecord> records;
        auto idIt            = idList.begin();
        auto orderIdIt       = orderIdList.begin();
        auto productTypeIdIt = productTypeIdList.begin();
        auto quantityIt      = quantityList.begin();
        auto priceIt         = priceList.begin();
        while (idIt != idList.end()) {
            records.push_back(Common::Entities::SalesOrderRecord{
                .id = *idIt,
                .orderId = *orderIdIt,
                .productTypeId = *productTypeIdIt,
                .quantity = *quantityIt,
                .price = *priceIt});
            ++idIt;
            ++orderIdIt;
            ++productTypeIdIt;
            ++quantityIt;
            ++priceIt;
        }

        emit salesOrderRecordsList(records);
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("handleSalesOrderRecordsList | Key missing in dataset: {}", e.what());
        handleError("Key missing in dataset");
    }
}

void ApiManager::handleLogsList(const Dataset &dataset)
{
    try {
        const auto &idList = dataset.at(Common::Entities::Log::ID_KEY);
        const auto &userIdList = dataset.at(Common::Entities::Log::USER_ID_KEY);
        const auto &actionList = dataset.at(Common::Entities::Log::ACTION_KEY);
        const auto &timestampList = dataset.at(Common::Entities::Log::TIMESTAMP_KEY);

        const auto itemCount = idList.size();
        if (userIdList.size() != itemCount || actionList.size() != itemCount ||
            timestampList.size() != itemCount) {
            handleError("Log lists have different sizes.");
            return;
        }

        std::vector<Common::Entities::Log> logs;
        auto idIt        = idList.begin();
        auto userIdIt    = userIdList.begin();
        auto actionIt    = actionList.begin();
        auto timestampIt = timestampList.begin();
        while (idIt != idList.end()) {
            logs.push_back(Common::Entities::Log{.id = *idIt,
                                                 .userId = *userIdIt,
                                                 .action = *actionIt,
                                                 .timestamp = *timestampIt});
            ++idIt;
            ++userIdIt;
            ++actionIt;
            ++timestampIt;
        }

        emit logsList(logs);
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("handleLogsList | Key missing in dataset: {}", e.what());
        handleError("Key missing in dataset");
    }
}

QVariantMap ApiManager::buildAnalyticsMetrics(const Dataset &dataset) const
{
    QVariantMap metrics;
    for (const auto &[key, values] : dataset) {
        metrics.insert(QString::fromStdString(key),
                       values.empty() ? QString() : QString::fromStdString(values.front()));
    }

    return metrics;
}
