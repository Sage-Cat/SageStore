#include "PurchaseOrdersRepository.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
inline constexpr int ID         = 0;
inline constexpr int DATE       = 1;
inline constexpr int USERID     = 2;
inline constexpr int SUPPLIERID = 3;
inline constexpr int STATUS     = 4;
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

void PurchaseOrderRepository::add(const PurchaseOrder &entity)
{
    //?????
    SPDLOG_TRACE("PurchaseOrderRepository::add");
    const std::string query = "INSERT INTO " + std::string(PurchaseOrder::TABLE_NAME) +
                              " (date, userId, supplierId, status) " + "VALUES (?, ?, ?, ?);";

    const std::vector<std::string> params = {entity.date, entity.userId, entity.supplierId,
                                             entity.status};
    m_dbManager->executeQuery(query, params);
}

void PurchaseOrderRepository::update(const PurchaseOrder &entity)
{
    SPDLOG_TRACE("PurchaseOrderRepository::update");
    const std::string query = "UPDATE" + std::string(PurchaseOrder::TABLE_NAME) +
                              " SET date = ?, userId = ?, supplierId = ?, status = ? WHERE id = ?;";
    const std::vector<std::string> params = {entity.date, entity.userId, entity.supplierId,
                                             entity.status, entity.id};

    m_dbManager->executeQuery(query, params);
}

void PurchaseOrderRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("PurchaseOrderRepository::deleteResource | id = {}", id);
    const std::string query =
        "DELETE FROM " + std::string(PurchaseOrder::TABLE_NAME) + " WHERE id = ?;";
    const std::vector<std::string> params = {id};

    m_dbManager->executeQuery(query, params);
}

std::vector<PurchaseOrder> PurchaseOrderRepository::getByField(const std::string &fieldName,
                                                               const std::string &value) const
{
    SPDLOG_TRACE("PurchaseOrderRepository::getByField | {} = {}", fieldName, value);
    std::vector<PurchaseOrder> purch;

    const std::string query = "SELECT id, date, userId, supplierId, status FROM " +
                              std::string(PurchaseOrder::TABLE_NAME) + " WHERE " + fieldName +
                              " = ?;";
    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next())
        purch.emplace_back(PurchaseFromCurrentRow(result));

    return purch;
}

std::vector<PurchaseOrder> PurchaseOrderRepository::getAll() const
{
    SPDLOG_TRACE("PurchaseOrderRepository::getAll");
    std::vector<PurchaseOrder> purch;

    const std::string query = "SELECT id, date, userId, supplierId, status FROM " +
                              std::string(PurchaseOrder::TABLE_NAME) + ";";
    auto result = m_dbManager->executeQuery(query, {});
    while (result && result->next())
        purch.emplace_back(PurchaseFromCurrentRow(result));

    return purch;
}

PurchaseOrder PurchaseOrderRepository::PurchaseFromCurrentRow(
    const std::shared_ptr<IQueryResult> &queryResult) const
{
    return PurchaseOrder{
        .id         = queryResult->getString(ID),
        .date       = queryResult->getString(DATE),
        .userId     = queryResult->getString(USERID),
        .supplierId = queryResult->getString(SUPPLIERID),
        .status     = queryResult->getString(STATUS),
    };
}