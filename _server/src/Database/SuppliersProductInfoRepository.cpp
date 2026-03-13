#include "SuppliersProductInfoRepository.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
inline constexpr int ID              = 0;
inline constexpr int SUPPLIER_ID     = 1;
inline constexpr int PRODUCT_TYPE_ID = 2;
inline constexpr int CODE            = 3;
} // namespace

SuppliersProductInfoRepository::SuppliersProductInfoRepository(
    std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("SuppliersProductInfoRepository::SuppliersProductInfoRepository");
}

SuppliersProductInfoRepository::~SuppliersProductInfoRepository()
{
    SPDLOG_TRACE("SuppliersProductInfoRepository::~SuppliersProductInfoRepository");
}

void SuppliersProductInfoRepository::add(const Common::Entities::SuppliersProductInfo &entity)
{
    SPDLOG_TRACE("SuppliersProductInfoRepository::add");

    const std::string query =
        "INSERT INTO " + std::string(Common::Entities::SuppliersProductInfo::TABLE_NAME) +
        " (supplierID, productTypeId, code) VALUES (?, ?, ?);";
    m_dbManager->executeQuery(query, {entity.supplierID, entity.productTypeId, entity.code});
}

void SuppliersProductInfoRepository::update(const Common::Entities::SuppliersProductInfo &entity)
{
    SPDLOG_TRACE("SuppliersProductInfoRepository::update");

    const std::string query =
        "UPDATE " + std::string(Common::Entities::SuppliersProductInfo::TABLE_NAME) +
        " SET supplierID = ?, productTypeId = ?, code = ? WHERE id = ?;";
    m_dbManager->executeQuery(query,
                              {entity.supplierID, entity.productTypeId, entity.code, entity.id});
}

void SuppliersProductInfoRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("SuppliersProductInfoRepository::deleteResource | id = {}", id);

    const std::string query =
        "DELETE FROM " + std::string(Common::Entities::SuppliersProductInfo::TABLE_NAME) +
        " WHERE id = ?;";
    m_dbManager->executeQuery(query, {id});
}

std::vector<Common::Entities::SuppliersProductInfo>
SuppliersProductInfoRepository::getByField(const std::string &fieldName,
                                           const std::string &value) const
{
    SPDLOG_TRACE("SuppliersProductInfoRepository::getByField | {} = {}", fieldName, value);

    std::vector<Common::Entities::SuppliersProductInfo> mappings;
    const std::string query =
        "SELECT id, supplierID, productTypeId, code FROM " +
        std::string(Common::Entities::SuppliersProductInfo::TABLE_NAME) + " WHERE " + fieldName +
        " = ?;";

    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next()) {
        mappings.emplace_back(suppliersProductInfoFromCurrentRow(result));
    }

    return mappings;
}

std::vector<Common::Entities::SuppliersProductInfo> SuppliersProductInfoRepository::getAll() const
{
    SPDLOG_TRACE("SuppliersProductInfoRepository::getAll");

    std::vector<Common::Entities::SuppliersProductInfo> mappings;
    const std::string query = "SELECT id, supplierID, productTypeId, code FROM " +
                              std::string(Common::Entities::SuppliersProductInfo::TABLE_NAME) + ";";

    auto result = m_dbManager->executeQuery(query, {});
    while (result && result->next()) {
        mappings.emplace_back(suppliersProductInfoFromCurrentRow(result));
    }

    return mappings;
}

Common::Entities::SuppliersProductInfo
SuppliersProductInfoRepository::suppliersProductInfoFromCurrentRow(
    const std::shared_ptr<IQueryResult> &queryResult) const
{
    return Common::Entities::SuppliersProductInfo{
        .id = queryResult->getString(ID),
        .supplierID = queryResult->getString(SUPPLIER_ID),
        .productTypeId = queryResult->getString(PRODUCT_TYPE_ID),
        .code = queryResult->getString(CODE)};
}
