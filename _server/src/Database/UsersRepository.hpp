#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "IRepository.hpp"
#include "IDatabaseManager.hpp"

class User;

class UsersRepository : public IRepository<User>
{
public:
    explicit UsersRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~UsersRepository() override;

    void add(const User &entity) override;
    void update(const User &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<User> getByField(const std::string &fieldName, const std::string &value) const override;
    std::vector<User> getAll() const override;

private:
    User userFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

private:
    std::shared_ptr<IDatabaseManager> m_dbManager;
};
