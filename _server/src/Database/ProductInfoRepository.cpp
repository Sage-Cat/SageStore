#include "ProductInfoRepository.hpp"

#include "common/Entities/ProductInfo.hpp"
#include "common/SpdlogConfig.hpp"

namespace {
inline constexpr int ID              = 0;
inline constexpr int PRODUCT_TYPE_ID = 1;
inline constexpr int NAME            = 2;
inline constexpr int VALUE           = 3;
} // namespace

ProductInfoRepository::ProductInfoRepository(std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("ProductInfoRepository::ProductInfoRepository");
}

ProductInfoRepository::~ProductInfoRepository() { SPDLOG_TRACE("ProductInfoRepository::~ProductInfoRepository"); }

void ProductInfoRepository::add(const Common::Entities::ProductInfo &entity)
{
    SPDLOG_TRACE("ProductInfoRepository::add");
    const std::string query = "INSERT INTO " +
                              std::string(Common::Entities::ProductInfo::TABLE_NAME) +
                              " (id, productTypeId, name, value) " + "VALUES (?, ?, ?, ?);";

    const std::vector<std::string> params =
        std::vector<std::string>{entity.id, entity.productTypeId, entity.name, entity.value};

    m_dbManager->executeQuery(query, params);
}

void ProductInfoRepository::update(const Common::Entities::ProductInfo &entity)
{
    SPDLOG_TRACE("ProductInfoRepository::update");
    const std::string query = "UPDATE " + std::string(Common::Entities::ProductInfo::TABLE_NAME) +
                              " SET id = ?, productTypeId = ?, name = ? value = ?;";
    const std::vector<std::string> params = {entity.id, entity.productTypeId, entity.name,
                                             entity.value};

    m_dbManager->executeQuery(query, params);
}

void ProductInfoRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("ProductInfoRepository::deleteResource | id = {}", id);
    const std::string query =
        "DELETE FROM " + std::string(Common::Entities::ProductInfo::TABLE_NAME) + " WHERE id = ?;";

    const std::vector<std::string> params = {id};

    m_dbManager->executeQuery(query, params);
}

std::vector<Common::Entities::ProductInfo>
ProductInfoRepository::getByField(const std::string &fieldName, const std::string &value) const
{
    SPDLOG_TRACE("ProductInfoRepository::getByField | {} = {}", fieldName, value);
    std::vector<Common::Entities::ProductInfo> productInfo;

    const std::string query = "SELECT productTypeId, name, value FROM " +
                              std::string(Common::Entities::ProductInfo::TABLE_NAME) + " WHERE " +
                              fieldName + " = ?;";

    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next())
        productInfo.emplace_back(userFromCurrentRow(result));

    return productInfo;
}

std::vector<Common::Entities::ProductInfo> ProductInfoRepository::getAll() const
{
    SPDLOG_TRACE("ProductInfoRepository::getAll");
    std::vector<Common::Entities::ProductInfo> productInfo;

    const std::string query = "SELECT id, productTypeId, name, value FROM  " +
                              std::string(Common::Entities::ProductInfo::TABLE_NAME) + " ;";
    auto result = m_dbManager->executeQuery(query, {});
    while (result && result->next())
        productInfo.emplace_back(userFromCurrentRow(result));

    return productInfo;
}

Common::Entities::ProductInfo
ProductInfoRepository::userFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const
{
    SPDLOG_TRACE("ProductInfoRepository::userFromCurrentRow");
    return Common::Entities::ProductInfo{.id            = queryResult->getString(ID),
                                         .productTypeId = queryResult->getString(PRODUCT_TYPE_ID),
                                         .name          = queryResult->getString(NAME),
                                         .value         = queryResult->getString(VALUE)};
}
