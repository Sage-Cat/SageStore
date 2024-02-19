#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "IRepository.hpp"
#include "DatabaseManager.hpp"
#include "Entities/Role.hpp"

class RolesRepository : public IRepository<Role>
{
public:
    explicit RolesRepository(std::shared_ptr<DatabaseManager> dbManager);
    virtual ~RolesRepository() override;

    void add(const Role &entity) override;
    void update(const Role &entity) override;
    void deleteResource(const std::string &id) override;
    std::optional<Role> getById(const std::string &id) const override;
    std::vector<Role> getAll() const override;

private:
    void executePrepared(const std::string &query, const std::vector<std::string> &params) const;
    std::optional<Role> roleFromCurrentRow() const;

private:
    std::shared_ptr<DatabaseManager> m_dbManager;
};
