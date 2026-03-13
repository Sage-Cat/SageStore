#include "SaleOrderRepository.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
inline constexpr int ID          = 0;
inline constexpr int DATE        = 1;
inline constexpr int USER_ID     = 2;
inline constexpr int CONTACT_ID  = 3;
inline constexpr int EMPLOYEE_ID = 4;
inline constexpr int STATUS      = 5;
} // namespace

SaleOrderRepository::SaleOrderRepository(std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("SaleOrderRepository::SaleOrderRepository");
}

SaleOrderRepository::~SaleOrderRepository()
{
    SPDLOG_TRACE("SaleOrderRepository::~SaleOrderRepository");
}

void SaleOrderRepository::add(const Common::Entities::SaleOrder &entity)
{
    SPDLOG_TRACE("SaleOrderRepository::add");

    const std::string query =
        "INSERT INTO " + std::string(Common::Entities::SaleOrder::TABLE_NAME) +
        " (date, userId, contactId, employeeId, status) VALUES (?, ?, ?, ?, ?);";
    m_dbManager->executeQuery(query,
                              {entity.date, entity.userId, entity.contactId, entity.employeeId,
                               entity.status});
}

void SaleOrderRepository::update(const Common::Entities::SaleOrder &entity)
{
    SPDLOG_TRACE("SaleOrderRepository::update");

    const std::string query =
        "UPDATE " + std::string(Common::Entities::SaleOrder::TABLE_NAME) +
        " SET date = ?, userId = ?, contactId = ?, employeeId = ?, status = ? WHERE id = ?;";
    m_dbManager->executeQuery(query,
                              {entity.date, entity.userId, entity.contactId, entity.employeeId,
                               entity.status, entity.id});
}

void SaleOrderRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("SaleOrderRepository::deleteResource | id = {}", id);

    const std::string query =
        "DELETE FROM " + std::string(Common::Entities::SaleOrder::TABLE_NAME) + " WHERE id = ?;";
    m_dbManager->executeQuery(query, {id});
}

std::vector<Common::Entities::SaleOrder>
SaleOrderRepository::getByField(const std::string &fieldName, const std::string &value) const
{
    SPDLOG_TRACE("SaleOrderRepository::getByField | {} = {}", fieldName, value);

    std::vector<Common::Entities::SaleOrder> orders;
    const std::string query =
        "SELECT id, date, userId, contactId, employeeId, status FROM " +
        std::string(Common::Entities::SaleOrder::TABLE_NAME) + " WHERE " + fieldName + " = ?;";

    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next()) {
        orders.emplace_back(saleOrderFromCurrentRow(result));
    }

    return orders;
}

std::vector<Common::Entities::SaleOrder> SaleOrderRepository::getAll() const
{
    SPDLOG_TRACE("SaleOrderRepository::getAll");

    std::vector<Common::Entities::SaleOrder> orders;
    const std::string query = "SELECT id, date, userId, contactId, employeeId, status FROM " +
                              std::string(Common::Entities::SaleOrder::TABLE_NAME) + ";";

    auto result = m_dbManager->executeQuery(query, {});
    while (result && result->next()) {
        orders.emplace_back(saleOrderFromCurrentRow(result));
    }

    return orders;
}

Common::Entities::SaleOrder
SaleOrderRepository::saleOrderFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const
{
    return Common::Entities::SaleOrder{.id = queryResult->getString(ID),
                                       .date = queryResult->getString(DATE),
                                       .userId = queryResult->getString(USER_ID),
                                       .contactId = queryResult->getString(CONTACT_ID),
                                       .employeeId = queryResult->getString(EMPLOYEE_ID),
                                       .status = queryResult->getString(STATUS)};
}
