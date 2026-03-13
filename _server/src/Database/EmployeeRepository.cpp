#include "EmployeeRepository.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
inline constexpr int ID      = 0;
inline constexpr int NAME    = 1;
inline constexpr int NUMBER  = 2;
inline constexpr int EMAIL   = 3;
inline constexpr int ADDRESS = 4;
} // namespace

EmployeeRepository::EmployeeRepository(std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("EmployeeRepository::EmployeeRepository");
}

EmployeeRepository::~EmployeeRepository()
{
    SPDLOG_TRACE("EmployeeRepository::~EmployeeRepository");
}

void EmployeeRepository::add(const Common::Entities::Employee &entity)
{
    SPDLOG_TRACE("EmployeeRepository::add");

    const std::string query =
        "INSERT INTO " + std::string(Common::Entities::Employee::TABLE_NAME) +
        " (name, number, email, address) VALUES (?, ?, ?, ?);";
    m_dbManager->executeQuery(query, {entity.name, entity.number, entity.email, entity.address});
}

void EmployeeRepository::update(const Common::Entities::Employee &entity)
{
    SPDLOG_TRACE("EmployeeRepository::update");

    const std::string query =
        "UPDATE " + std::string(Common::Entities::Employee::TABLE_NAME) +
        " SET name = ?, number = ?, email = ?, address = ? WHERE id = ?;";
    m_dbManager->executeQuery(query, {entity.name, entity.number, entity.email, entity.address,
                                      entity.id});
}

void EmployeeRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("EmployeeRepository::deleteResource | id = {}", id);

    const std::string query =
        "DELETE FROM " + std::string(Common::Entities::Employee::TABLE_NAME) + " WHERE id = ?;";
    m_dbManager->executeQuery(query, {id});
}

std::vector<Common::Entities::Employee>
EmployeeRepository::getByField(const std::string &fieldName, const std::string &value) const
{
    SPDLOG_TRACE("EmployeeRepository::getByField | {} = {}", fieldName, value);

    std::vector<Common::Entities::Employee> employees;
    const std::string query =
        "SELECT id, name, number, email, address FROM " +
        std::string(Common::Entities::Employee::TABLE_NAME) + " WHERE " + fieldName + " = ?;";

    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next()) {
        employees.emplace_back(employeeFromCurrentRow(result));
    }

    return employees;
}

std::vector<Common::Entities::Employee> EmployeeRepository::getAll() const
{
    SPDLOG_TRACE("EmployeeRepository::getAll");

    std::vector<Common::Entities::Employee> employees;
    const std::string query = "SELECT id, name, number, email, address FROM " +
                              std::string(Common::Entities::Employee::TABLE_NAME) + ";";

    auto result = m_dbManager->executeQuery(query, {});
    while (result && result->next()) {
        employees.emplace_back(employeeFromCurrentRow(result));
    }

    return employees;
}

Common::Entities::Employee
EmployeeRepository::employeeFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const
{
    return Common::Entities::Employee{.id = queryResult->getString(ID),
                                      .name = queryResult->getString(NAME),
                                      .number = queryResult->getString(NUMBER),
                                      .email = queryResult->getString(EMAIL),
                                      .address = queryResult->getString(ADDRESS)};
}
