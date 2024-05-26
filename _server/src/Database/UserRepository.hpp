#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"

#include "common/Entities/User.hpp"

class UserRepository : public IRepository<Common::Entities::User> {
public:
    explicit UserRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~UserRepository() override;

    void add(const Common::Entities::User &entity) override;
    void update(const Common::Entities::User &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<Common::Entities::User> getByField(const std::string &fieldName,
                                                   const std::string &value) const override;
    std::vector<Common::Entities::User> getAll() const override;

private:
    Common::Entities::User
    userFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

private:
    std::shared_ptr<IDatabaseManager> m_dbManager;
};
