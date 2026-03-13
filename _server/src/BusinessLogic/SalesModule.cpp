#include "SalesModule.hpp"

#include "Database/RepositoryManager.hpp"
#include "ModuleUtils.hpp"

#define _M "SalesModule"

namespace {
Common::Entities::SaleOrder buildOrder(const Dataset &request, const std::string &resourceId)
{
    return Common::Entities::SaleOrder{
        .id = resourceId,
        .date = ModuleUtils::getRequiredDatasetValue(request, Common::Entities::SaleOrder::DATE_KEY,
                                                     _M, "SalesModule::buildOrder"),
        .userId =
            ModuleUtils::getRequiredPositiveInteger(request,
                                                    Common::Entities::SaleOrder::USER_ID_KEY, _M,
                                                    "SalesModule::buildOrder"),
        .contactId = ModuleUtils::getRequiredPositiveInteger(
            request, Common::Entities::SaleOrder::CONTACT_ID_KEY, _M, "SalesModule::buildOrder"),
        .employeeId = ModuleUtils::getRequiredPositiveInteger(
            request, Common::Entities::SaleOrder::EMPLOYEE_ID_KEY, _M,
            "SalesModule::buildOrder"),
        .status = ModuleUtils::getRequiredDatasetValue(request,
                                                       Common::Entities::SaleOrder::STATUS_KEY,
                                                       _M, "SalesModule::buildOrder")};
}

Common::Entities::SalesOrderRecord buildOrderRecord(const Dataset &request,
                                                    const std::string &resourceId)
{
    return Common::Entities::SalesOrderRecord{
        .id = resourceId,
        .orderId = ModuleUtils::getRequiredPositiveInteger(
            request, Common::Entities::SalesOrderRecord::ORDER_ID_KEY, _M,
            "SalesModule::buildOrderRecord"),
        .productTypeId = ModuleUtils::getRequiredPositiveInteger(
            request, Common::Entities::SalesOrderRecord::PRODUCT_TYPE_ID_KEY, _M,
            "SalesModule::buildOrderRecord"),
        .quantity = ModuleUtils::getRequiredPositiveInteger(
            request, Common::Entities::SalesOrderRecord::QUANTITY_KEY, _M,
            "SalesModule::buildOrderRecord"),
        .price = ModuleUtils::getRequiredNumber(request,
                                                Common::Entities::SalesOrderRecord::PRICE_KEY, _M,
                                                "SalesModule::buildOrderRecord")};
}
} // namespace

SalesModule::SalesModule(RepositoryManager &repositoryManager)
{
    m_ordersRepository       = std::move(repositoryManager.getSaleOrderRepository());
    m_orderRecordsRepository = std::move(repositoryManager.getSalesOrderRecordRepository());
    m_usersRepository        = std::move(repositoryManager.getUserRepository());
    m_contactsRepository     = std::move(repositoryManager.getContactRepository());
    m_employeesRepository    = std::move(repositoryManager.getEmployeeRepository());
    m_productTypesRepository = std::move(repositoryManager.getProductTypeRepository());
}

SalesModule::~SalesModule() = default;

ResponseData SalesModule::executeTask(const RequestData &requestData)
{
    if (requestData.submodule == "orders") {
        if (requestData.method == "GET") {
            return getOrders();
        }
        if (requestData.method == "POST") {
            addOrder(requestData.dataset);
            return {};
        }
        if (requestData.method == "PUT") {
            editOrder(requestData.dataset, requestData.resourceId);
            return {};
        }
        if (requestData.method == "DELETE") {
            deleteOrder(requestData.resourceId);
            return {};
        }
        throw ServerException(_M, "Unrecognized method");
    }

    if (requestData.submodule == "order-records") {
        if (requestData.method == "GET") {
            return getOrderRecords(requestData.resourceId);
        }
        if (requestData.method == "POST") {
            addOrderRecord(requestData.dataset);
            return {};
        }
        if (requestData.method == "PUT") {
            editOrderRecord(requestData.dataset, requestData.resourceId);
            return {};
        }
        if (requestData.method == "DELETE") {
            deleteOrderRecord(requestData.resourceId);
            return {};
        }
        throw ServerException(_M, "Unrecognized method");
    }

    throw ServerException(_M, "Unrecognized sales task");
}

ResponseData SalesModule::getOrders() const
{
    ResponseData response;
    response.dataset[Common::Entities::SaleOrder::ID_KEY]          = {};
    response.dataset[Common::Entities::SaleOrder::DATE_KEY]        = {};
    response.dataset[Common::Entities::SaleOrder::USER_ID_KEY]     = {};
    response.dataset[Common::Entities::SaleOrder::CONTACT_ID_KEY]  = {};
    response.dataset[Common::Entities::SaleOrder::EMPLOYEE_ID_KEY] = {};
    response.dataset[Common::Entities::SaleOrder::STATUS_KEY]      = {};

    for (const auto &order : m_ordersRepository->getAll()) {
        response.dataset[Common::Entities::SaleOrder::ID_KEY].emplace_back(order.id);
        response.dataset[Common::Entities::SaleOrder::DATE_KEY].emplace_back(order.date);
        response.dataset[Common::Entities::SaleOrder::USER_ID_KEY].emplace_back(order.userId);
        response.dataset[Common::Entities::SaleOrder::CONTACT_ID_KEY].emplace_back(order.contactId);
        response.dataset[Common::Entities::SaleOrder::EMPLOYEE_ID_KEY].emplace_back(
            order.employeeId);
        response.dataset[Common::Entities::SaleOrder::STATUS_KEY].emplace_back(order.status);
    }

    return response;
}

ResponseData SalesModule::getOrderRecords(const std::string &orderId) const
{
    ResponseData response;
    response.dataset[Common::Entities::SalesOrderRecord::ID_KEY]              = {};
    response.dataset[Common::Entities::SalesOrderRecord::ORDER_ID_KEY]        = {};
    response.dataset[Common::Entities::SalesOrderRecord::PRODUCT_TYPE_ID_KEY] = {};
    response.dataset[Common::Entities::SalesOrderRecord::QUANTITY_KEY]        = {};
    response.dataset[Common::Entities::SalesOrderRecord::PRICE_KEY]           = {};

    const auto records = orderId.empty()
                             ? m_orderRecordsRepository->getAll()
                             : m_orderRecordsRepository->getByField(
                                   Common::Entities::SalesOrderRecord::ORDER_ID_KEY, orderId);

    for (const auto &record : records) {
        response.dataset[Common::Entities::SalesOrderRecord::ID_KEY].emplace_back(record.id);
        response.dataset[Common::Entities::SalesOrderRecord::ORDER_ID_KEY].emplace_back(
            record.orderId);
        response.dataset[Common::Entities::SalesOrderRecord::PRODUCT_TYPE_ID_KEY].emplace_back(
            record.productTypeId);
        response.dataset[Common::Entities::SalesOrderRecord::QUANTITY_KEY].emplace_back(
            record.quantity);
        response.dataset[Common::Entities::SalesOrderRecord::PRICE_KEY].emplace_back(record.price);
    }

    return response;
}

void SalesModule::addOrder(const Dataset &request) const
{
    const auto order = buildOrder(request, "");
    ModuleUtils::ensureEntityExists(m_usersRepository, Common::Entities::User::ID_KEY, order.userId,
                                    _M, "Referenced user does not exist");
    ModuleUtils::ensureEntityExists(m_contactsRepository, Common::Entities::Contact::ID_KEY,
                                    order.contactId, _M, "Referenced contact does not exist");
    ModuleUtils::ensureEntityExists(m_employeesRepository, Common::Entities::Employee::ID_KEY,
                                    order.employeeId, _M, "Referenced employee does not exist");
    m_ordersRepository->add(order);
}

void SalesModule::editOrder(const Dataset &request, const std::string &resourceId) const
{
    ModuleUtils::ensureResourceId(resourceId, _M, "Sales order");
    ModuleUtils::ensureEntityExists(m_ordersRepository, Common::Entities::SaleOrder::ID_KEY,
                                    resourceId, _M, "Sales order does not exist");

    const auto order = buildOrder(request, resourceId);
    ModuleUtils::ensureEntityExists(m_usersRepository, Common::Entities::User::ID_KEY, order.userId,
                                    _M, "Referenced user does not exist");
    ModuleUtils::ensureEntityExists(m_contactsRepository, Common::Entities::Contact::ID_KEY,
                                    order.contactId, _M, "Referenced contact does not exist");
    ModuleUtils::ensureEntityExists(m_employeesRepository, Common::Entities::Employee::ID_KEY,
                                    order.employeeId, _M, "Referenced employee does not exist");
    m_ordersRepository->update(order);
}

void SalesModule::deleteOrder(const std::string &resourceId) const
{
    ModuleUtils::ensureResourceId(resourceId, _M, "Sales order");
    ModuleUtils::ensureEntityExists(m_ordersRepository, Common::Entities::SaleOrder::ID_KEY,
                                    resourceId, _M, "Sales order does not exist");

    for (const auto &record : m_orderRecordsRepository->getByField(
             Common::Entities::SalesOrderRecord::ORDER_ID_KEY, resourceId)) {
        m_orderRecordsRepository->deleteResource(record.id);
    }

    m_ordersRepository->deleteResource(resourceId);
}

void SalesModule::addOrderRecord(const Dataset &request) const
{
    const auto record = buildOrderRecord(request, "");
    ModuleUtils::ensureEntityExists(m_ordersRepository, Common::Entities::SaleOrder::ID_KEY,
                                    record.orderId, _M, "Referenced sales order does not exist");
    ModuleUtils::ensureEntityExists(m_productTypesRepository,
                                    Common::Entities::ProductType::ID_KEY, record.productTypeId,
                                    _M, "Referenced product type does not exist");
    m_orderRecordsRepository->add(record);
}

void SalesModule::editOrderRecord(const Dataset &request, const std::string &resourceId) const
{
    ModuleUtils::ensureResourceId(resourceId, _M, "Sales order record");
    ModuleUtils::ensureEntityExists(m_orderRecordsRepository,
                                    Common::Entities::SalesOrderRecord::ID_KEY, resourceId, _M,
                                    "Sales order record does not exist");

    const auto record = buildOrderRecord(request, resourceId);
    ModuleUtils::ensureEntityExists(m_ordersRepository, Common::Entities::SaleOrder::ID_KEY,
                                    record.orderId, _M, "Referenced sales order does not exist");
    ModuleUtils::ensureEntityExists(m_productTypesRepository,
                                    Common::Entities::ProductType::ID_KEY, record.productTypeId,
                                    _M, "Referenced product type does not exist");
    m_orderRecordsRepository->update(record);
}

void SalesModule::deleteOrderRecord(const std::string &resourceId) const
{
    ModuleUtils::ensureResourceId(resourceId, _M, "Sales order record");
    ModuleUtils::ensureEntityExists(m_orderRecordsRepository,
                                    Common::Entities::SalesOrderRecord::ID_KEY, resourceId, _M,
                                    "Sales order record does not exist");
    m_orderRecordsRepository->deleteResource(resourceId);
}
