#include "SupplierRepository.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
inline constexpr int ID      = 0;
inline constexpr int NAME    = 1;
inline constexpr int NUMBER  = 2;
inline constexpr int EMAIL   = 3;
inline constexpr int ADDRESS = 4;
} // namespace

SupplierRepository::SupplierRepository(std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("SupplierRepository::SupplierRepository");
}

SupplierRepository::~SupplierRepository()
{
    SPDLOG_TRACE("SupplierRepository::~SupplierRepository");
}

void SupplierRepository::add(const Common::Entities::Supplier &entity)
{
    SPDLOG_TRACE("SupplierRepository::add");

    const std::string query =
        "INSERT INTO " + std::string(Common::Entities::Supplier::TABLE_NAME) +
        " (name, number, email, address) VALUES (?, ?, ?, ?);";
    m_dbManager->executeQuery(query, {entity.name, entity.number, entity.email, entity.address});
}

void SupplierRepository::update(const Common::Entities::Supplier &entity)
{
    SPDLOG_TRACE("SupplierRepository::update");

    const std::string query =
        "UPDATE " + std::string(Common::Entities::Supplier::TABLE_NAME) +
        " SET name = ?, number = ?, email = ?, address = ? WHERE id = ?;";
    m_dbManager->executeQuery(query, {entity.name, entity.number, entity.email, entity.address,
                                      entity.id});
}

void SupplierRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("SupplierRepository::deleteResource | id = {}", id);

    const std::string query =
        "DELETE FROM " + std::string(Common::Entities::Supplier::TABLE_NAME) + " WHERE id = ?;";
    m_dbManager->executeQuery(query, {id});
}

std::vector<Common::Entities::Supplier>
SupplierRepository::getByField(const std::string &fieldName, const std::string &value) const
{
    SPDLOG_TRACE("SupplierRepository::getByField | {} = {}", fieldName, value);

    std::vector<Common::Entities::Supplier> suppliers;
    const std::string query =
        "SELECT id, name, number, email, address FROM " +
        std::string(Common::Entities::Supplier::TABLE_NAME) + " WHERE " + fieldName + " = ?;";

    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next()) {
        suppliers.emplace_back(supplierFromCurrentRow(result));
    }

    return suppliers;
}

std::vector<Common::Entities::Supplier> SupplierRepository::getAll() const
{
    SPDLOG_TRACE("SupplierRepository::getAll");

    std::vector<Common::Entities::Supplier> suppliers;
    const std::string query = "SELECT id, name, number, email, address FROM " +
                              std::string(Common::Entities::Supplier::TABLE_NAME) + ";";

    auto result = m_dbManager->executeQuery(query, {});
    while (result && result->next()) {
        suppliers.emplace_back(supplierFromCurrentRow(result));
    }

    return suppliers;
}

Common::Entities::Supplier
SupplierRepository::supplierFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const
{
    return Common::Entities::Supplier{.id = queryResult->getString(ID),
                                      .name = queryResult->getString(NAME),
                                      .number = queryResult->getString(NUMBER),
                                      .email = queryResult->getString(EMAIL),
                                      .address = queryResult->getString(ADDRESS)};
}
