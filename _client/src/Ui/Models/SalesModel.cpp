#include "Ui/Models/SalesModel.hpp"

#include "Network/ApiManager.hpp"

SalesModel::SalesModel(ApiManager &apiManager, QObject *parent)
    : BaseModel(parent), m_apiManager(apiManager)
{
    connect(&m_apiManager, &ApiManager::errorOccurred, this, &SalesModel::errorOccurred);
    connect(&m_apiManager, &ApiManager::salesOrdersList, this, &SalesModel::onOrdersList);
    connect(&m_apiManager, &ApiManager::salesOrderRecordsList, this, &SalesModel::onOrderRecordsList);
    connect(&m_apiManager, &ApiManager::usersList, this, &SalesModel::onUsersList);
    connect(&m_apiManager, &ApiManager::contactsList, this, &SalesModel::onContactsList);
    connect(&m_apiManager, &ApiManager::employeesList, this, &SalesModel::onEmployeesList);
    connect(&m_apiManager, &ApiManager::productTypesList, this, &SalesModel::onProductTypesList);
    connect(&m_apiManager, &ApiManager::salesOrderCreated, this, &SalesModel::onOrderCreated);
    connect(&m_apiManager, &ApiManager::salesOrderEdited, this, &SalesModel::onOrderEdited);
    connect(&m_apiManager, &ApiManager::salesOrderDeleted, this, &SalesModel::onOrderDeleted);
    connect(&m_apiManager, &ApiManager::salesOrderRecordCreated, this,
            &SalesModel::onOrderRecordCreated);
    connect(&m_apiManager, &ApiManager::salesOrderRecordEdited, this,
            &SalesModel::onOrderRecordEdited);
    connect(&m_apiManager, &ApiManager::salesOrderRecordDeleted, this,
            &SalesModel::onOrderRecordDeleted);
}

QVector<Common::Entities::SaleOrder> SalesModel::orders() const { return m_orders; }

QVector<Common::Entities::SalesOrderRecord> SalesModel::orderRecords() const
{
    return m_orderRecords;
}

QVector<Common::Entities::User> SalesModel::users() const { return m_users; }

QVector<Common::Entities::Contact> SalesModel::contacts() const { return m_contacts; }

QVector<Common::Entities::Employee> SalesModel::employees() const { return m_employees; }

QVector<Common::Entities::ProductType> SalesModel::productTypes() const
{
    return m_productTypes;
}

void SalesModel::fetchAll()
{
    fetchReferenceData();
    fetchOrders();
}

void SalesModel::fetchOrders() { m_apiManager.getSalesOrders(); }

void SalesModel::fetchOrderRecords(const QString &orderId)
{
    m_lastOrderId = orderId;
    m_apiManager.getSalesOrderRecords(orderId);
}

void SalesModel::fetchReferenceData()
{
    m_apiManager.getUsers();
    m_apiManager.getContacts();
    m_apiManager.getEmployees();
    m_apiManager.getProductTypes();
}

void SalesModel::createOrder(const Common::Entities::SaleOrder &order)
{
    m_apiManager.createSalesOrder(order);
}

void SalesModel::editOrder(const Common::Entities::SaleOrder &order)
{
    m_apiManager.editSalesOrder(order);
}

void SalesModel::deleteOrder(const QString &id) { m_apiManager.deleteSalesOrder(id); }

void SalesModel::createOrderRecord(const Common::Entities::SalesOrderRecord &record)
{
    m_apiManager.createSalesOrderRecord(record);
}

void SalesModel::editOrderRecord(const Common::Entities::SalesOrderRecord &record)
{
    m_apiManager.editSalesOrderRecord(record);
}

void SalesModel::deleteOrderRecord(const QString &id)
{
    m_apiManager.deleteSalesOrderRecord(id);
}

void SalesModel::onOrdersList(const std::vector<Common::Entities::SaleOrder> &orders)
{
    m_orders = QVector<Common::Entities::SaleOrder>(orders.begin(), orders.end());
    emit ordersChanged();
}

void SalesModel::onOrderRecordsList(const std::vector<Common::Entities::SalesOrderRecord> &records)
{
    m_orderRecords = QVector<Common::Entities::SalesOrderRecord>(records.begin(), records.end());
    emit orderRecordsChanged();
}

void SalesModel::onUsersList(const std::vector<Common::Entities::User> &users)
{
    m_users = QVector<Common::Entities::User>(users.begin(), users.end());
    emit usersChanged();
}

void SalesModel::onContactsList(const std::vector<Common::Entities::Contact> &contacts)
{
    m_contacts = QVector<Common::Entities::Contact>(contacts.begin(), contacts.end());
    emit contactsChanged();
}

void SalesModel::onEmployeesList(const std::vector<Common::Entities::Employee> &employees)
{
    m_employees = QVector<Common::Entities::Employee>(employees.begin(), employees.end());
    emit employeesChanged();
}

void SalesModel::onProductTypesList(const std::vector<Common::Entities::ProductType> &productTypes)
{
    m_productTypes = QVector<Common::Entities::ProductType>(productTypes.begin(), productTypes.end());
    emit productTypesChanged();
}

void SalesModel::onOrderCreated()
{
    fetchOrders();
    refreshCurrentRecords();
}

void SalesModel::onOrderEdited()
{
    fetchOrders();
    refreshCurrentRecords();
}

void SalesModel::onOrderDeleted()
{
    fetchOrders();
    refreshCurrentRecords();
}

void SalesModel::onOrderRecordCreated() { refreshCurrentRecords(); }

void SalesModel::onOrderRecordEdited() { refreshCurrentRecords(); }

void SalesModel::onOrderRecordDeleted() { refreshCurrentRecords(); }

void SalesModel::refreshCurrentRecords()
{
    if (!m_lastOrderId.isEmpty()) {
        m_apiManager.getSalesOrderRecords(m_lastOrderId);
    }
}
