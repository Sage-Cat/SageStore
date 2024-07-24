#include "PurchaseOrdersRepository.hpp"

#include "common/Entities/PurchaseOrder.hpp"
 
#include "common/SpdlogConfig.hpp"

namespace
{
    inline constexpr int ID = 0;
    inline constexpr int DATE = 1;
    inline constexpr int USERID = 2;
    inline constexpr int SUPPLIERID = 3;
    inline constexpr int STATUS = 4;
}

PurchaseOrderRepository::PurchaseOrderRepository(std::shared_ptr<IDatabaseManager> dbManager)
         : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("PurchaseOrderRepository::PurchaseOrderRepository");
}

PurchaseOrderRepository::~PurchaseOrderRepository()
{
    SPDLOG_TRACE("PurchaseOrderRepository::~PurchaseOrderRepository");
}

void PurchaseOrderRepository::add(const Purchaseorder& entity) 
{ 
    //?????
    SPDLOG_TRACE("PurchaseOrderRepository::add");
    const std::string query = "INSERT INTO " + std::string(Purchaseorder::TABLE_NAME) + " (date, userId, supplierId, status) " +
                               "VALUES (?, ?, ?, ?);";

    const std::vector<std::string> params = {entity.date, entity.userId, entity.supplierId, entity.status};
    m_dbManager->executeQuery(query, params);

}

void PurchaseOrderRepository::update(const Purchaseorder &entity)
{
    SPDLOG_TRACE("PurchaseOrderRepository::update");
    const std::string query = "UPDATE" + std::string(Purchaseorder::TABLE_NAME) + " SET date = ?, userId = ?, supplierId = ?, status = ? WHERE id = ?;";
    const std::vector<std::string> params  = {entity.date, entity.userId, entity.supplierId, entity.status, entity.id};

    m_dbManager->executeQuery(query, params);
}

void PurchaseOrderRepository::deleteResource(const std::string& id)
{
    SPDLOG_TRACE("PurchaseOrderRepository::deleteResource | id = {}", id);
    const std::string query = "DELETE FROM " + std::string(Purchaseorder::TABLE_NAME) + " WHERE id = ?;";
    const std::vector<std::string> params = {id};

    m_dbManager->executeQuery(query, params);
}

std::vector<Purchaseorder> PurchaseOrderRepository::getByField(const std::string &fieldName, const std::string &value) const
{
    SPDLOG_TRACE("PurchaseOrderRepository::getByField | {} = {}", fieldName, value);
    std::vector<Purchaseorder> purch;

    const std::string query = "SELECT id, date, userId, supplierId, status FROM " + std::string(Purchaseorder::TABLE_NAME) + " WHERE " + fieldName + " = ?;";
    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next())
        purch.emplace_back(PurchFromCurrentRow(result));

    return purch;
}

std::vector<Purchaseorder> PurchaseOrderRepository::getAll() const
{
    SPDLOG_TRACE("PurchaseOrderRepository::getAll");
    std::vector<Purchaseorder> purch;

    const std::string query = "SELECT id, date, userId, supplierId, status FROM " + std::string(Purchaseorder::TABLE_NAME) + ";";
    auto result = m_dbManager->executeQuery(query, {});
    while(result && result->next())
        purch.emplace_back(PurchFromCurrentRow(result));

    return purch;
}

Purchaseorder PurchaseOrderRepository::PurchFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const
{
    return Purchaseorder{
        .id = queryResult->getString(ID),
        .date = queryResult->getString(DATE),
        .userId = queryResult->getString(USERID),
        .supplierId = queryResult->getString(SUPPLIERID),
        .status = queryResult->getString(STATUS),
    };
}