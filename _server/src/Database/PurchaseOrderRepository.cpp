#include "PurchaseOrderRepository.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
inline constexpr int ID          = 0;
inline constexpr int DATE        = 1;
inline constexpr int USER_ID     = 2;
inline constexpr int SUPPLIER_ID = 3;
inline constexpr int STATUS      = 4;
} // namespace

PurchaseOrderRepository::PurchaseOrderRepository(std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("PurchaseOrderRepository::PurchaseOrderRepository");
}

PurchaseOrderRepository::~PurchaseOrderRepository()
{
    SPDLOG_TRACE("PurchaseOrderRepository::~PurchaseOrderRepository");
}

void PurchaseOrderRepository::add(const Common::Entities::PurchaseOrder &entity)
{
    SPDLOG_TRACE("PurchaseOrderRepository::add");

    const std::string query =
        "INSERT INTO " + std::string(Common::Entities::PurchaseOrder::TABLE_NAME) +
        " (date, userId, supplierId, status) VALUES (?, ?, ?, ?);";
    m_dbManager->executeQuery(query, {entity.date, entity.userId, entity.supplierId, entity.status});
}

void PurchaseOrderRepository::update(const Common::Entities::PurchaseOrder &entity)
{
    SPDLOG_TRACE("PurchaseOrderRepository::update");

    const std::string query =
        "UPDATE " + std::string(Common::Entities::PurchaseOrder::TABLE_NAME) +
        " SET date = ?, userId = ?, supplierId = ?, status = ? WHERE id = ?;";
    m_dbManager->executeQuery(query,
                              {entity.date, entity.userId, entity.supplierId, entity.status,
                               entity.id});
}

void PurchaseOrderRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("PurchaseOrderRepository::deleteResource | id = {}", id);

    const std::string query = "DELETE FROM " +
                              std::string(Common::Entities::PurchaseOrder::TABLE_NAME) +
                              " WHERE id = ?;";
    m_dbManager->executeQuery(query, {id});
}

std::vector<Common::Entities::PurchaseOrder>
PurchaseOrderRepository::getByField(const std::string &fieldName, const std::string &value) const
{
    SPDLOG_TRACE("PurchaseOrderRepository::getByField | {} = {}", fieldName, value);

    std::vector<Common::Entities::PurchaseOrder> orders;
    const std::string query =
        "SELECT id, date, userId, supplierId, status FROM " +
        std::string(Common::Entities::PurchaseOrder::TABLE_NAME) + " WHERE " + fieldName + " = ?;";

    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next()) {
        orders.emplace_back(purchaseOrderFromCurrentRow(result));
    }

    return orders;
}

std::vector<Common::Entities::PurchaseOrder> PurchaseOrderRepository::getAll() const
{
    SPDLOG_TRACE("PurchaseOrderRepository::getAll");

    std::vector<Common::Entities::PurchaseOrder> orders;
    const std::string query = "SELECT id, date, userId, supplierId, status FROM " +
                              std::string(Common::Entities::PurchaseOrder::TABLE_NAME) + ";";

    auto result = m_dbManager->executeQuery(query, {});
    while (result && result->next()) {
        orders.emplace_back(purchaseOrderFromCurrentRow(result));
    }

    return orders;
}

Common::Entities::PurchaseOrder PurchaseOrderRepository::purchaseOrderFromCurrentRow(
    const std::shared_ptr<IQueryResult> &queryResult) const
{
    return Common::Entities::PurchaseOrder{.id = queryResult->getString(ID),
                                           .date = queryResult->getString(DATE),
                                           .userId = queryResult->getString(USER_ID),
                                           .supplierId = queryResult->getString(SUPPLIER_ID),
                                           .status = queryResult->getString(STATUS)};
}
