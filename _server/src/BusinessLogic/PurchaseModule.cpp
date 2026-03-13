#include "PurchaseModule.hpp"

#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

#include "Database/RepositoryManager.hpp"
#include "ModuleUtils.hpp"

#define _M "PurchaseModule"

namespace {
std::string trimStatus(std::string value)
{
    const auto first = value.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos) {
        return {};
    }

    const auto last = value.find_last_not_of(" \t\n\r\f\v");
    return value.substr(first, last - first + 1);
}

std::string normalizeOrderStatus(const std::string &status, const char *context)
{
    const auto normalized = trimStatus(status);
    if (normalized == "Draft" || normalized == "Чернетка") {
        return "Draft";
    }
    if (normalized == "Ordered" || normalized == "Pending" || normalized == "Замовлено") {
        return "Ordered";
    }
    if (normalized == "Received" || normalized == "Отримано") {
        return "Received";
    }

    SPDLOG_ERROR("{} | Invalid purchase order status: {}", context, status);
    throw ServerException(_M, "Invalid purchase order status");
}

struct PlannedStockMutation {
    bool create{};
    Common::Entities::Inventory stock;
};

Common::Entities::PurchaseOrder getOrderById(
    const std::shared_ptr<IRepository<Common::Entities::PurchaseOrder>> &ordersRepository,
    const std::string &orderId, const char *context)
{
    ModuleUtils::ensureEntityExists(ordersRepository, Common::Entities::PurchaseOrder::ID_KEY,
                                    orderId, _M, "Purchase order does not exist");

    const auto orders =
        ordersRepository->getByField(Common::Entities::PurchaseOrder::ID_KEY, orderId);
    if (orders.empty()) {
        SPDLOG_ERROR("{} | Purchase order not found after existence check: {}", context, orderId);
        throw ServerException(_M, "Purchase order does not exist");
    }

    return orders.front();
}

void ensureOrderIsNotReceived(const Common::Entities::PurchaseOrder &order, const char *context,
                              const char *message)
{
    if (normalizeOrderStatus(order.status, context) == "Received") {
        SPDLOG_ERROR("{} | Purchase order {} is already received", context, order.id);
        throw ServerException(_M, message);
    }
}

void ensureOrderStatusCanBeSetManually(const Common::Entities::PurchaseOrder &order,
                                       const char *context)
{
    if (order.status == "Received") {
        SPDLOG_ERROR("{} | Purchase order {} attempted direct Received transition", context,
                     order.id);
        throw ServerException(_M, "Purchase orders must be received via receipt posting");
    }
}

Common::Entities::PurchaseOrder buildOrder(const Dataset &request, const std::string &resourceId)
{
    return Common::Entities::PurchaseOrder{
        .id = resourceId,
        .date = ModuleUtils::getRequiredDatasetValue(request,
                                                     Common::Entities::PurchaseOrder::DATE_KEY, _M,
                                                     "PurchaseModule::buildOrder"),
        .userId =
            ModuleUtils::getRequiredPositiveInteger(request,
                                                    Common::Entities::PurchaseOrder::USER_ID_KEY,
                                                    _M, "PurchaseModule::buildOrder"),
        .supplierId = ModuleUtils::getRequiredPositiveInteger(
            request, Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY, _M,
            "PurchaseModule::buildOrder"),
        .status = normalizeOrderStatus(ModuleUtils::getRequiredDatasetValue(
                                           request, Common::Entities::PurchaseOrder::STATUS_KEY, _M,
                                           "PurchaseModule::buildOrder"),
                                       "PurchaseModule::buildOrder")};
}

Common::Entities::PurchaseOrderRecord buildOrderRecord(const Dataset &request,
                                                       const std::string &resourceId)
{
    return Common::Entities::PurchaseOrderRecord{
        .id = resourceId,
        .orderId = ModuleUtils::getRequiredPositiveInteger(
            request, Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, _M,
            "PurchaseModule::buildOrderRecord"),
        .productTypeId = ModuleUtils::getRequiredPositiveInteger(
            request, Common::Entities::PurchaseOrderRecord::PRODUCT_TYPE_ID_KEY, _M,
            "PurchaseModule::buildOrderRecord"),
        .quantity = ModuleUtils::getRequiredPositiveInteger(
            request, Common::Entities::PurchaseOrderRecord::QUANTITY_KEY, _M,
            "PurchaseModule::buildOrderRecord"),
        .price = ModuleUtils::getRequiredNumber(
            request, Common::Entities::PurchaseOrderRecord::PRICE_KEY, _M,
            "PurchaseModule::buildOrderRecord")};
}

long long parseNonNegativeQuantity(const std::string &value, const char *context)
{
    if (!ModuleUtils::isDigits(value)) {
        SPDLOG_ERROR("{} | Invalid quantity value: {}", context, value);
        throw ServerException(_M, "Invalid quantity value");
    }

    try {
        return std::stoll(value);
    } catch (...) {
        SPDLOG_ERROR("{} | Quantity is out of range: {}", context, value);
        throw ServerException(_M, "Quantity is out of range");
    }
}

long long addQuantities(const std::string &currentQuantity, const std::string &receivedQuantity)
{
    const auto current = parseNonNegativeQuantity(currentQuantity,
                                                  "PurchaseModule::addQuantities/current");
    const auto received = parseNonNegativeQuantity(receivedQuantity,
                                                   "PurchaseModule::addQuantities/received");
    if (current > std::numeric_limits<long long>::max() - received) {
        SPDLOG_ERROR("PurchaseModule::addQuantities | Quantity overflow: {} + {}", currentQuantity,
                     receivedQuantity);
        throw ServerException(_M, "Stock quantity overflow");
    }

    return current + received;
}
} // namespace

PurchaseModule::PurchaseModule(RepositoryManager &repositoryManager)
    : m_repositoryManager(repositoryManager)
{
    m_ordersRepository       = std::move(repositoryManager.getPurchaseOrderRepository());
    m_orderRecordsRepository = std::move(repositoryManager.getPurchaseOrderRecordRepository());
    m_inventoryRepository    = std::move(repositoryManager.getInventoryRepository());
    m_usersRepository        = std::move(repositoryManager.getUserRepository());
    m_suppliersRepository    = std::move(repositoryManager.getSupplierRepository());
    m_employeesRepository    = std::move(repositoryManager.getEmployeeRepository());
    m_productTypesRepository = std::move(repositoryManager.getProductTypeRepository());
}

PurchaseModule::~PurchaseModule() = default;

ResponseData PurchaseModule::executeTask(const RequestData &requestData)
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

    if (requestData.submodule == "receipts") {
        if (requestData.method == "POST") {
            receiveOrder(requestData.dataset);
            return {};
        }

        throw ServerException(_M, "Unrecognized method");
    }

    throw ServerException(_M, "Unrecognized purchase task");
}

ResponseData PurchaseModule::getOrders() const
{
    ResponseData response;
    response.dataset[Common::Entities::PurchaseOrder::ID_KEY]          = {};
    response.dataset[Common::Entities::PurchaseOrder::DATE_KEY]        = {};
    response.dataset[Common::Entities::PurchaseOrder::USER_ID_KEY]     = {};
    response.dataset[Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY] = {};
    response.dataset[Common::Entities::PurchaseOrder::STATUS_KEY]      = {};

    for (const auto &order : m_ordersRepository->getAll()) {
        response.dataset[Common::Entities::PurchaseOrder::ID_KEY].emplace_back(order.id);
        response.dataset[Common::Entities::PurchaseOrder::DATE_KEY].emplace_back(order.date);
        response.dataset[Common::Entities::PurchaseOrder::USER_ID_KEY].emplace_back(order.userId);
        response.dataset[Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY].emplace_back(
            order.supplierId);
        response.dataset[Common::Entities::PurchaseOrder::STATUS_KEY].emplace_back(order.status);
    }

    return response;
}

ResponseData PurchaseModule::getOrderRecords(const std::string &orderId) const
{
    ResponseData response;
    response.dataset[Common::Entities::PurchaseOrderRecord::ID_KEY]              = {};
    response.dataset[Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY]        = {};
    response.dataset[Common::Entities::PurchaseOrderRecord::PRODUCT_TYPE_ID_KEY] = {};
    response.dataset[Common::Entities::PurchaseOrderRecord::QUANTITY_KEY]        = {};
    response.dataset[Common::Entities::PurchaseOrderRecord::PRICE_KEY]           = {};

    const auto records = orderId.empty()
                             ? m_orderRecordsRepository->getAll()
                             : m_orderRecordsRepository->getByField(
                                   Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, orderId);

    for (const auto &record : records) {
        response.dataset[Common::Entities::PurchaseOrderRecord::ID_KEY].emplace_back(record.id);
        response.dataset[Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY].emplace_back(
            record.orderId);
        response.dataset[Common::Entities::PurchaseOrderRecord::PRODUCT_TYPE_ID_KEY].emplace_back(
            record.productTypeId);
        response.dataset[Common::Entities::PurchaseOrderRecord::QUANTITY_KEY].emplace_back(
            record.quantity);
        response.dataset[Common::Entities::PurchaseOrderRecord::PRICE_KEY].emplace_back(
            record.price);
    }

    return response;
}

void PurchaseModule::addOrder(const Dataset &request) const
{
    const auto order = buildOrder(request, "");
    ensureOrderStatusCanBeSetManually(order, "PurchaseModule::addOrder");
    ModuleUtils::ensureEntityExists(m_usersRepository, Common::Entities::User::ID_KEY, order.userId,
                                    _M, "Referenced user does not exist");
    ModuleUtils::ensureEntityExists(m_suppliersRepository, Common::Entities::Supplier::ID_KEY,
                                    order.supplierId, _M, "Referenced supplier does not exist");
    m_ordersRepository->add(order);
}

void PurchaseModule::editOrder(const Dataset &request, const std::string &resourceId) const
{
    ModuleUtils::ensureResourceId(resourceId, _M, "Purchase order");
    const auto existingOrder =
        getOrderById(m_ordersRepository, resourceId, "PurchaseModule::editOrder");
    ensureOrderIsNotReceived(existingOrder, "PurchaseModule::editOrder",
                             "Received purchase orders cannot be edited");

    const auto order = buildOrder(request, resourceId);
    ensureOrderStatusCanBeSetManually(order, "PurchaseModule::editOrder");
    ModuleUtils::ensureEntityExists(m_usersRepository, Common::Entities::User::ID_KEY, order.userId,
                                    _M, "Referenced user does not exist");
    ModuleUtils::ensureEntityExists(m_suppliersRepository, Common::Entities::Supplier::ID_KEY,
                                    order.supplierId, _M, "Referenced supplier does not exist");
    m_ordersRepository->update(order);
}

void PurchaseModule::deleteOrder(const std::string &resourceId) const
{
    ModuleUtils::ensureResourceId(resourceId, _M, "Purchase order");
    const auto existingOrder =
        getOrderById(m_ordersRepository, resourceId, "PurchaseModule::deleteOrder");
    ensureOrderIsNotReceived(existingOrder, "PurchaseModule::deleteOrder",
                             "Received purchase orders cannot be deleted");

    for (const auto &record : m_orderRecordsRepository->getByField(
             Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, resourceId)) {
        m_orderRecordsRepository->deleteResource(record.id);
    }

    m_ordersRepository->deleteResource(resourceId);
}

void PurchaseModule::receiveOrder(const Dataset &request) const
{
    const auto orderId = ModuleUtils::getRequiredPositiveInteger(
        request, Common::Entities::PurchaseOrder::ID_KEY, _M, "PurchaseModule::receiveOrder");
    const auto employeeId = ModuleUtils::getRequiredPositiveInteger(
        request, Common::Entities::Inventory::EMPLOYEE_ID_KEY, _M,
        "PurchaseModule::receiveOrder");

    ModuleUtils::ensureEntityExists(m_employeesRepository, Common::Entities::Employee::ID_KEY,
                                    employeeId, _M, "Referenced employee does not exist");

    auto order = getOrderById(m_ordersRepository, orderId, "PurchaseModule::receiveOrder");
    ensureOrderIsNotReceived(order, "PurchaseModule::receiveOrder",
                             "Purchase order is already received");

    const auto records = m_orderRecordsRepository->getByField(
        Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, orderId);
    if (records.empty()) {
        throw ServerException(_M, "Purchase order has no line items");
    }

    std::unordered_map<std::string, PlannedStockMutation> plannedStocks;
    std::vector<std::string> plannedOrder;
    plannedStocks.reserve(records.size());
    plannedOrder.reserve(records.size());

    for (const auto &record : records) {
        ModuleUtils::ensureEntityExists(m_productTypesRepository,
                                        Common::Entities::ProductType::ID_KEY,
                                        record.productTypeId, _M,
                                        "Referenced product type does not exist");

        auto plannedIt = plannedStocks.find(record.productTypeId);
        if (plannedIt == plannedStocks.end()) {
            const auto matchingStocks = m_inventoryRepository->getByField(
                Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, record.productTypeId);

            PlannedStockMutation mutation;
            if (matchingStocks.empty()) {
                mutation.create = true;
                mutation.stock  = Common::Entities::Inventory{.id = "",
                                                              .productTypeId = record.productTypeId,
                                                              .quantityAvailable = "0",
                                                              .employeeId = employeeId};
            } else {
                mutation.create = false;
                mutation.stock  = matchingStocks.front();
            }

            plannedIt = plannedStocks.emplace(record.productTypeId, std::move(mutation)).first;
            plannedOrder.push_back(record.productTypeId);
        }

        plannedIt->second.stock.quantityAvailable =
            std::to_string(addQuantities(plannedIt->second.stock.quantityAvailable, record.quantity));
        plannedIt->second.stock.employeeId = employeeId;
    }

    order.status = "Received";
    m_repositoryManager.runInTransaction([&]() {
        for (const auto &productTypeId : plannedOrder) {
            const auto &mutation = plannedStocks.at(productTypeId);
            if (mutation.create) {
                m_inventoryRepository->add(mutation.stock);
            } else {
                m_inventoryRepository->update(mutation.stock);
            }
        }

        m_ordersRepository->update(order);
    });
}

void PurchaseModule::addOrderRecord(const Dataset &request) const
{
    const auto record = buildOrderRecord(request, "");
    const auto parentOrder =
        getOrderById(m_ordersRepository, record.orderId, "PurchaseModule::addOrderRecord");
    ensureOrderIsNotReceived(parentOrder, "PurchaseModule::addOrderRecord",
                             "Received purchase orders cannot be modified through line items");
    ModuleUtils::ensureEntityExists(m_productTypesRepository,
                                    Common::Entities::ProductType::ID_KEY, record.productTypeId,
                                    _M, "Referenced product type does not exist");
    m_orderRecordsRepository->add(record);
}

void PurchaseModule::editOrderRecord(const Dataset &request, const std::string &resourceId) const
{
    ModuleUtils::ensureResourceId(resourceId, _M, "Purchase order record");
    ModuleUtils::ensureEntityExists(m_orderRecordsRepository,
                                    Common::Entities::PurchaseOrderRecord::ID_KEY, resourceId, _M,
                                    "Purchase order record does not exist");
    const auto existingRecords = m_orderRecordsRepository->getByField(
        Common::Entities::PurchaseOrderRecord::ID_KEY, resourceId);
    if (existingRecords.empty()) {
        SPDLOG_ERROR("PurchaseModule::editOrderRecord | Purchase order record {} not found after "
                     "existence check",
                     resourceId);
        throw ServerException(_M, "Purchase order record does not exist");
    }

    const auto record = buildOrderRecord(request, resourceId);
    const auto existingParentOrder = getOrderById(m_ordersRepository, existingRecords.front().orderId,
                                                  "PurchaseModule::editOrderRecord");
    ensureOrderIsNotReceived(existingParentOrder, "PurchaseModule::editOrderRecord",
                             "Received purchase orders cannot be modified through line items");
    const auto targetParentOrder =
        getOrderById(m_ordersRepository, record.orderId, "PurchaseModule::editOrderRecord");
    ensureOrderIsNotReceived(targetParentOrder, "PurchaseModule::editOrderRecord",
                             "Received purchase orders cannot be modified through line items");
    ModuleUtils::ensureEntityExists(m_productTypesRepository,
                                    Common::Entities::ProductType::ID_KEY, record.productTypeId,
                                    _M, "Referenced product type does not exist");
    m_orderRecordsRepository->update(record);
}

void PurchaseModule::deleteOrderRecord(const std::string &resourceId) const
{
    ModuleUtils::ensureResourceId(resourceId, _M, "Purchase order record");
    ModuleUtils::ensureEntityExists(m_orderRecordsRepository,
                                    Common::Entities::PurchaseOrderRecord::ID_KEY, resourceId, _M,
                                    "Purchase order record does not exist");
    const auto records = m_orderRecordsRepository->getByField(
        Common::Entities::PurchaseOrderRecord::ID_KEY, resourceId);
    if (records.empty()) {
        SPDLOG_ERROR("PurchaseModule::deleteOrderRecord | Purchase order record {} not found after "
                     "existence check",
                     resourceId);
        throw ServerException(_M, "Purchase order record does not exist");
    }

    const auto parentOrder = getOrderById(m_ordersRepository, records.front().orderId,
                                          "PurchaseModule::deleteOrderRecord");
    ensureOrderIsNotReceived(parentOrder, "PurchaseModule::deleteOrderRecord",
                             "Received purchase orders cannot be modified through line items");
    m_orderRecordsRepository->deleteResource(resourceId);
}
