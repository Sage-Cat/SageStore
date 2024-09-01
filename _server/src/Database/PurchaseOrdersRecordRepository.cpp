#include "PurchaseOrdersRecordRepository.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
inline constexpr int ID            = 0;
inline constexpr int ORDERID       = 1;
inline constexpr int PRODUCTTYPEID = 2;
inline constexpr int QUANTITY      = 3;
inline constexpr int PRICE         = 4;
} // namespace

PurchaseOrderRecordRepository::PurchaseOrderRecordRepository(
    std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("PurchaseOrderRecordRepository::PurchaseOrderRecordRepository");
}

PurchaseOrderRecordRepository::~PurchaseOrderRecordRepository()
{
    SPDLOG_TRACE("PurchaseOrderRecordRepository::~PurchaseOrderRecordRepository");
}

void PurchaseOrderRecordRepository::add(const PurchaseOrderRecord &entity)
{
    SPDLOG_TRACE("PurchaseOrderRecordRepository::add");
    const std::string query = "INSERT INTO " + std::string(PurchaseOrderRecord::TABLE_NAME) +
                              " (orderId, productTypeId, quantity, price) " +
                              "VALUES (?, ?, ?, ?);";
    const std::vector<std::string> params = {entity.orderId, entity.productTypeId, entity.quantity,
                                             entity.price};
    m_dbManager->executeQuery(query, params);
}

void PurchaseOrderRecordRepository::update(const PurchaseOrderRecord &entity)
{
    SPDLOG_TRACE("PurchaseOrderRecordRepository::update");
    const std::string query =
        "UPDATE " + std::string(PurchaseOrderRecord::TABLE_NAME) +
        " SET orderId = ?, productTypeId = ?, quantity = ?, price = ? WHERE id = ?;";
    const std::vector<std::string> params = {entity.orderId, entity.productTypeId, entity.quantity,
                                             entity.price};

    m_dbManager->executeQuery(query, params);
}

void PurchaseOrderRecordRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("PurchaseOrderRecordRepository::deleteResource | id = {}", id);
    const std::string query =
        "DELETE FROM " + std::string(PurchaseOrderRecord::TABLE_NAME) + " WHERE id =?;";
    const std::vector<std::string> params = {id};

    m_dbManager->executeQuery(query, params);
}

std::vector<PurchaseOrderRecord>
PurchaseOrderRecordRepository::getByField(const std::string &fieldName,
                                          const std::string &value) const
{
    SPDLOG_TRACE("PurchaseOrderRecordRepository::getByField | {} = {}", fieldName, value);
    std::vector<PurchaseOrderRecord> purch;

    const std::string query = "SELECT id, orderId, productTypeId, quantity, price FROM " +
                              std::string(PurchaseOrderRecord::TABLE_NAME) + " WHERE " + fieldName +
                              " = ?;";
    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next())
        purch.push_back(PurchaseFromCurrentRow(result));

    return purch;
}

std::vector<PurchaseOrderRecord> PurchaseOrderRecordRepository::getAll() const
{
    SPDLOG_TRACE("PurchaseOrderRecordRepository::getAll");
    std::vector<PurchaseOrderRecord> purch;

    const std::string query = "SELECT id, orderId, productTypeId, quantity, price FROM " +
                              std::string(PurchaseOrderRecord::TABLE_NAME) + ";";
    auto result = m_dbManager->executeQuery(query, {});
    while (result && result->next())
        purch.emplace_back(PurchaseFromCurrentRow(result));

    return purch;
}

PurchaseOrderRecord PurchaseOrderRecordRepository::PurchaseFromCurrentRow(
    const std::shared_ptr<IQueryResult> &queryResult) const
{
    return PurchaseOrderRecord{
        .id            = queryResult->getString(ID),
        .orderId       = queryResult->getString(ORDERID),
        .productTypeId = queryResult->getString(PRODUCTTYPEID),
        .quantity      = queryResult->getString(QUANTITY),
        .price         = queryResult->getString(PRICE),
    };
}