#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"

#include "common/Entities/Role.hpp"

class RoleRepository : public IRepository<Common::Entities::Role> {
public:
    explicit RoleRepository(std::shared_ptr<IDatabaseManager> dbManager);
    virtual ~RoleRepository() override;

    void add(const Common::Entities::Role &entity) override;
    void update(const Common::Entities::Role &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<Common::Entities::Role> getByField(const std::string &fieldName,
                                                   const std::string &value) const override;
    std::vector<Common::Entities::Role> getAll() const override;

private:
    Common::Entities::Role
    roleFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

private:
    std::shared_ptr<IDatabaseManager> m_dbManager;
};
