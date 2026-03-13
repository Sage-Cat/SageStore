#pragma once

#include <memory>
#include <vector>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"

#include "common/Entities/Contact.hpp"

class ContactRepository : public IRepository<Common::Entities::Contact> {
public:
    explicit ContactRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~ContactRepository() override;

    void add(const Common::Entities::Contact &entity) override;
    void update(const Common::Entities::Contact &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<Common::Entities::Contact> getByField(const std::string &fieldName,
                                                      const std::string &value) const override;
    std::vector<Common::Entities::Contact> getAll() const override;

private:
    Common::Entities::Contact
    contactFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

    std::shared_ptr<IDatabaseManager> m_dbManager;
};
