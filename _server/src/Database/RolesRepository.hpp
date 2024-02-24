#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "IRepository.hpp"
#include "IDatabaseManager.hpp"
#include "Entities/Role.hpp"

class RolesRepository : public IRepository<Role>
{
public:
    explicit RolesRepository(std::shared_ptr<IDatabaseManager> dbManager);
    virtual ~RolesRepository() override;

    void add(const Role &entity) override;
    void update(const Role &entity) override;
    void deleteResource(const std::string &id) override;
    std::optional<Role> getById(const std::string &id) const override;
    std::vector<Role> getAll() const override;

private:
    Role roleFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

private:
    std::shared_ptr<IDatabaseManager> m_dbManager;
};
