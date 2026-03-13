#include "PurchaseOrderRecordRepository.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
inline constexpr int ID              = 0;
inline constexpr int ORDER_ID        = 1;
inline constexpr int PRODUCT_TYPE_ID = 2;
inline constexpr int QUANTITY        = 3;
inline constexpr int PRICE           = 4;
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

void PurchaseOrderRecordRepository::add(const Common::Entities::PurchaseOrderRecord &entity)
{
    SPDLOG_TRACE("PurchaseOrderRecordRepository::add");

    const std::string query =
        "INSERT INTO " + std::string(Common::Entities::PurchaseOrderRecord::TABLE_NAME) +
        " (orderId, productTypeId, quantity, price) VALUES (?, ?, ?, ?);";
    m_dbManager->executeQuery(query,
                              {entity.orderId, entity.productTypeId, entity.quantity, entity.price});
}

void PurchaseOrderRecordRepository::update(const Common::Entities::PurchaseOrderRecord &entity)
{
    SPDLOG_TRACE("PurchaseOrderRecordRepository::update");

    const std::string query =
        "UPDATE " + std::string(Common::Entities::PurchaseOrderRecord::TABLE_NAME) +
        " SET orderId = ?, productTypeId = ?, quantity = ?, price = ? WHERE id = ?;";
    m_dbManager->executeQuery(query, {entity.orderId, entity.productTypeId, entity.quantity,
                                      entity.price, entity.id});
}

void PurchaseOrderRecordRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("PurchaseOrderRecordRepository::deleteResource | id = {}", id);

    const std::string query = "DELETE FROM " +
                              std::string(Common::Entities::PurchaseOrderRecord::TABLE_NAME) +
                              " WHERE id = ?;";
    m_dbManager->executeQuery(query, {id});
}

std::vector<Common::Entities::PurchaseOrderRecord> PurchaseOrderRecordRepository::getByField(
    const std::string &fieldName, const std::string &value) const
{
    SPDLOG_TRACE("PurchaseOrderRecordRepository::getByField | {} = {}", fieldName, value);

    std::vector<Common::Entities::PurchaseOrderRecord> records;
    const std::string query =
        "SELECT id, orderId, productTypeId, quantity, price FROM " +
        std::string(Common::Entities::PurchaseOrderRecord::TABLE_NAME) + " WHERE " + fieldName +
        " = ?;";

    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next()) {
        records.emplace_back(purchaseOrderRecordFromCurrentRow(result));
    }

    return records;
}

std::vector<Common::Entities::PurchaseOrderRecord> PurchaseOrderRecordRepository::getAll() const
{
    SPDLOG_TRACE("PurchaseOrderRecordRepository::getAll");

    std::vector<Common::Entities::PurchaseOrderRecord> records;
    const std::string query = "SELECT id, orderId, productTypeId, quantity, price FROM " +
                              std::string(Common::Entities::PurchaseOrderRecord::TABLE_NAME) + ";";

    auto result = m_dbManager->executeQuery(query, {});
    while (result && result->next()) {
        records.emplace_back(purchaseOrderRecordFromCurrentRow(result));
    }

    return records;
}

Common::Entities::PurchaseOrderRecord
PurchaseOrderRecordRepository::purchaseOrderRecordFromCurrentRow(
    const std::shared_ptr<IQueryResult> &queryResult) const
{
    return Common::Entities::PurchaseOrderRecord{
        .id = queryResult->getString(ID),
        .orderId = queryResult->getString(ORDER_ID),
        .productTypeId = queryResult->getString(PRODUCT_TYPE_ID),
        .quantity = queryResult->getString(QUANTITY),
        .price = queryResult->getString(PRICE)};
}
