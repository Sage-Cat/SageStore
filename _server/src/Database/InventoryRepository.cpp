#include "InventoryRepository.hpp"

#include "common/Entities/Inventory.hpp"
#include "common/SpdlogConfig.hpp"

namespace {
inline constexpr int ID                 = 0;
inline constexpr int PRODUCT_TYPE_ID    = 1;
inline constexpr int QUANTITY_AVAILABLE = 2;
inline constexpr int EMPLOYEE_ID        = 3;
} // namespace

InventoryRepository::InventoryRepository(std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("InventoryRepository::InventoryRepository");
}

InventoryRepository::~InventoryRepository() { SPDLOG_TRACE("InventoryRepository::~InventoryRepository"); }

void InventoryRepository::add(const Common::Entities::Inventory &entity)
{
    SPDLOG_TRACE("InventoryRepository::add");
    const std::string query = "INSERT INTO " + std::string(Common::Entities::Inventory::TABLE_NAME) +
                              " (productTypeId, quantityAvailable, employeeId) VALUES (?, ?, ?);";
    const std::vector<std::string> params = {
        entity.productTypeId, entity.quantityAvailable, entity.employeeId};

    m_dbManager->executeQuery(query, params);
}

void InventoryRepository::update(const Common::Entities::Inventory &entity)
{
    SPDLOG_TRACE("InventoryRepository::update");
    const std::string query = "UPDATE " + std::string(Common::Entities::Inventory::TABLE_NAME) +
                              " SET productTypeId = ?, quantityAvailable = ?, employeeId = ? "
                              "WHERE id = ?;";
    const std::vector<std::string> params = {entity.productTypeId, entity.quantityAvailable,
                                             entity.employeeId, entity.id};

    m_dbManager->executeQuery(query, params);
}

void InventoryRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("InventoryRepository::deleteResource | id = {}", id);
    const std::string query =
        "DELETE FROM " + std::string(Common::Entities::Inventory::TABLE_NAME) + " WHERE id = ?;";
    m_dbManager->executeQuery(query, {id});
}

std::vector<Common::Entities::Inventory>
InventoryRepository::getByField(const std::string &fieldName, const std::string &value) const
{
    SPDLOG_TRACE("InventoryRepository::getByField | {} = {}", fieldName, value);
    std::vector<Common::Entities::Inventory> inventoryRecords;

    const std::string query = "SELECT id, productTypeId, quantityAvailable, employeeId FROM " +
                              std::string(Common::Entities::Inventory::TABLE_NAME) + " WHERE " +
                              fieldName + " = ?;";
    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next()) {
        inventoryRecords.emplace_back(inventoryFromCurrentRow(result));
    }

    return inventoryRecords;
}

std::vector<Common::Entities::Inventory> InventoryRepository::getAll() const
{
    SPDLOG_TRACE("InventoryRepository::getAll");
    std::vector<Common::Entities::Inventory> inventoryRecords;

    const std::string query = "SELECT id, productTypeId, quantityAvailable, employeeId FROM " +
                              std::string(Common::Entities::Inventory::TABLE_NAME) + ";";
    auto result = m_dbManager->executeQuery(query, {});
    while (result && result->next()) {
        inventoryRecords.emplace_back(inventoryFromCurrentRow(result));
    }

    return inventoryRecords;
}

Common::Entities::Inventory
InventoryRepository::inventoryFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const
{
    return Common::Entities::Inventory{.id = queryResult->getString(ID),
                                       .productTypeId = queryResult->getString(PRODUCT_TYPE_ID),
                                       .quantityAvailable =
                                           queryResult->getString(QUANTITY_AVAILABLE),
                                       .employeeId = queryResult->getString(EMPLOYEE_ID)};
}
