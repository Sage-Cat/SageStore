#include "ContactRepository.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
inline constexpr int ID    = 0;
inline constexpr int NAME  = 1;
inline constexpr int TYPE  = 2;
inline constexpr int EMAIL = 3;
inline constexpr int PHONE = 4;
} // namespace

ContactRepository::ContactRepository(std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("ContactRepository::ContactRepository");
}

ContactRepository::~ContactRepository() { SPDLOG_TRACE("ContactRepository::~ContactRepository"); }

void ContactRepository::add(const Common::Entities::Contact &entity)
{
    SPDLOG_TRACE("ContactRepository::add");

    const std::string query = "INSERT INTO " + std::string(Common::Entities::Contact::TABLE_NAME) +
                              " (name, type, email, phone) VALUES (?, ?, ?, ?);";
    m_dbManager->executeQuery(query, {entity.name, entity.type, entity.email, entity.phone});
}

void ContactRepository::update(const Common::Entities::Contact &entity)
{
    SPDLOG_TRACE("ContactRepository::update");

    const std::string query = "UPDATE " + std::string(Common::Entities::Contact::TABLE_NAME) +
                              " SET name = ?, type = ?, email = ?, phone = ? WHERE id = ?;";
    m_dbManager->executeQuery(query, {entity.name, entity.type, entity.email, entity.phone,
                                      entity.id});
}

void ContactRepository::deleteResource(const std::string &id)
{
    SPDLOG_TRACE("ContactRepository::deleteResource | id = {}", id);

    const std::string query =
        "DELETE FROM " + std::string(Common::Entities::Contact::TABLE_NAME) + " WHERE id = ?;";
    m_dbManager->executeQuery(query, {id});
}

std::vector<Common::Entities::Contact>
ContactRepository::getByField(const std::string &fieldName, const std::string &value) const
{
    SPDLOG_TRACE("ContactRepository::getByField | {} = {}", fieldName, value);

    std::vector<Common::Entities::Contact> contacts;
    const std::string query =
        "SELECT id, name, type, email, phone FROM " +
        std::string(Common::Entities::Contact::TABLE_NAME) + " WHERE " + fieldName + " = ?;";

    auto result = m_dbManager->executeQuery(query, {value});
    while (result && result->next()) {
        contacts.emplace_back(contactFromCurrentRow(result));
    }

    return contacts;
}

std::vector<Common::Entities::Contact> ContactRepository::getAll() const
{
    SPDLOG_TRACE("ContactRepository::getAll");

    std::vector<Common::Entities::Contact> contacts;
    const std::string query = "SELECT id, name, type, email, phone FROM " +
                              std::string(Common::Entities::Contact::TABLE_NAME) + ";";

    auto result = m_dbManager->executeQuery(query, {});
    while (result && result->next()) {
        contacts.emplace_back(contactFromCurrentRow(result));
    }

    return contacts;
}

Common::Entities::Contact
ContactRepository::contactFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const
{
    return Common::Entities::Contact{.id = queryResult->getString(ID),
                                     .name = queryResult->getString(NAME),
                                     .type = queryResult->getString(TYPE),
                                     .email = queryResult->getString(EMAIL),
                                     .phone = queryResult->getString(PHONE)};
}
