#include "ProductTypeRepository.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
inline constexpr int ID          = 0;
inline constexpr int CODE        = 1;
inline constexpr int BARCODE     = 2;
inline constexpr int NAME        = 3;
inline constexpr int DESCRIPTION = 4;
inline constexpr int LAST_PRICE  = 5;
inline constexpr int UNIT        = 6;
inline constexpr int IS_IMPORTED = 7;
} // namespace

ProductTypeRepository::ProductTypeRepository(std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("ProductTypeRepository::ProductTypeRepository");
}

ProductTypeRepository::~ProductTypeRepository()
{
    SPDLOG_TRACE("ProductTypeRepository::~ProductTypeRepository");
}

void ProductTypeRepository::add(const Common::Entities::ProductType &entity)
{
    SPDLOG_TRACE("ProductTypeRepository::add");

    const std::string query =
        "INSERT INTO " + std::string(Common::Entities::ProductType::TABLE_NAME) +
        " (code, barcode, name, description, lastPrice, unit, isImported) VALUES (?, ?, ?, ?, ?, ?, ?);";

    m_dbManager->executeQuery(query, {entity.code,       entity.barcode,  entity.name,
                                      entity.description, entity.lastPrice, entity.unit,
                                      entity.isImported});
}

void ProductTypeRepository::update(const Common::Entities::ProductType &entity)
{
    SPDLOG_TRACE("ProductTypeRepository::update");

    const std::string query =
        "UPDATE " + std::string(Common::Entities::ProductType::TABLE_NAME) +
        " SET code = ?, barcode = ?, name = ?, description = ?, lastPrice = ?, unit = ?, isImported = ? WHERE id = ?;";

    m_dbManager->executeQuery(query, {entity.code,       entity.barcode,  entity.name,
                                      entity.description, entity.lastPrice, entity.unit,
                                      entity.isImported,  entity.id});
}

void ProductTypeRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("ProductTypeRepository::deleteResource | id = {}", id);

    const std::string query =
        "DELETE FROM " + std::string(Common::Entities::ProductType::TABLE_NAME) + " WHERE id = ?;";
    m_dbManager->executeQuery(query, {id});
}

std::vector<Common::Entities::ProductType>
ProductTypeRepository::getByField(const std::string &fieldName, const std::string &value) const
{
    SPDLOG_TRACE("ProductTypeRepository::getByField | {} = {}", fieldName, value);

    std::vector<Common::Entities::ProductType> productTypes;
    const std::string query =
        "SELECT id, code, barcode, name, description, lastPrice, unit, isImported FROM " +
        std::string(Common::Entities::ProductType::TABLE_NAME) + " WHERE " + fieldName + " = ?;";

    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next()) {
        productTypes.emplace_back(productTypeFromCurrentRow(result));
    }

    return productTypes;
}

std::vector<Common::Entities::ProductType> ProductTypeRepository::getAll() const
{
    SPDLOG_TRACE("ProductTypeRepository::getAll");

    std::vector<Common::Entities::ProductType> productTypes;
    const std::string query =
        "SELECT id, code, barcode, name, description, lastPrice, unit, isImported FROM " +
        std::string(Common::Entities::ProductType::TABLE_NAME) + ";";

    auto result = m_dbManager->executeQuery(query, {});
    while (result && result->next()) {
        productTypes.emplace_back(productTypeFromCurrentRow(result));
    }

    return productTypes;
}

Common::Entities::ProductType ProductTypeRepository::productTypeFromCurrentRow(
    const std::shared_ptr<IQueryResult> &queryResult) const
{
    return Common::Entities::ProductType{.id          = queryResult->getString(ID),
                                         .code        = queryResult->getString(CODE),
                                         .barcode     = queryResult->getString(BARCODE),
                                         .name        = queryResult->getString(NAME),
                                         .description = queryResult->getString(DESCRIPTION),
                                         .lastPrice   = queryResult->getString(LAST_PRICE),
                                         .unit        = queryResult->getString(UNIT),
                                         .isImported  = queryResult->getString(IS_IMPORTED)};
}
