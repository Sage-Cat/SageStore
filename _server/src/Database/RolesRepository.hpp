#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "Database/IRepository.hpp"
#include "Database/IDatabaseManager.hpp"

class Role;

class RolesRepository : public IRepository<Role>
{
public:
    explicit RolesRepository(std::shared_ptr<IDatabaseManager> dbManager);
    virtual ~RolesRepository() override;

    void add(const Role &entity) override;
    void update(const Role &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<Role> getByField(const std::string &fieldName, const std::string &value) const override;
    std::vector<Role> getAll() const override;

private:
    Role roleFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

private:
    std::shared_ptr<IDatabaseManager> m_dbManager;
};
