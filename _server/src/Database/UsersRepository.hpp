#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "IRepository.hpp"
#include "DatabaseManager.hpp"
#include "Entities/User.hpp"

class UsersRepository : public IRepository<User>
{
public:
    explicit UsersRepository(std::shared_ptr<DatabaseManager> dbManager);
    virtual ~UsersRepository() override;

    void add(const User &entity) override;
    void update(const User &entity) override;
    void deleteResource(const std::string &id) override;
    std::optional<User> getById(const std::string &id) const override;
    std::vector<User> getAll() const override;

    std::optional<User> getByUsername(const std::string &username) const;

private:
    void executePrepared(const std::string &query, const std::vector<std::string> &params) const;
    std::optional<User> userFromCurrentRow() const;

private:
    std::shared_ptr<DatabaseManager> m_dbManager;
};
