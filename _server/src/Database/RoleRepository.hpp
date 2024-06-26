#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "IRepository.hpp"
#include "IDatabaseManager.hpp"

class Role;

class RoleRepository : public IRepository<Role>
{
public:
    explicit RoleRepository(std::shared_ptr<IDatabaseManager> dbManager);
    virtual ~RoleRepository() override;

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
