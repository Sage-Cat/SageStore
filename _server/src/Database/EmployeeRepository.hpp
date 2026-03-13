#pragma once

#include <memory>
#include <vector>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"

#include "common/Entities/Employee.hpp"

class EmployeeRepository : public IRepository<Common::Entities::Employee> {
public:
    explicit EmployeeRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~EmployeeRepository() override;

    void add(const Common::Entities::Employee &entity) override;
    void update(const Common::Entities::Employee &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<Common::Entities::Employee> getByField(const std::string &fieldName,
                                                       const std::string &value) const override;
    std::vector<Common::Entities::Employee> getAll() const override;

private:
    Common::Entities::Employee
    employeeFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

    std::shared_ptr<IDatabaseManager> m_dbManager;
};
