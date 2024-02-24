#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "IRepository.hpp"
#include "IDatabaseManager.hpp"
#include "Entities/User.hpp"

class UsersRepository : public IRepository<User>
{
public:
    explicit UsersRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~UsersRepository() override;

    void add(const User &entity) override;
    void update(const User &entity) override;
    void deleteResource(const std::string &id) override;
    std::optional<User> getById(const std::string &id) const override;
    std::vector<User> getAll() const override;

    virtual std::optional<User> getByUsername(const std::string &username) const;

private:
    User userFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

private:
    std::shared_ptr<IDatabaseManager> m_dbManager;
};
