#pragma once

#include <memory>
#include <vector>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"

#include "common/Entities/Log.hpp"

class LogRepository : public IRepository<Common::Entities::Log> {
public:
    explicit LogRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~LogRepository() override;

    void add(const Common::Entities::Log &entity) override;
    void update(const Common::Entities::Log &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<Common::Entities::Log> getByField(const std::string &fieldName,
                                                  const std::string &value) const override;
    std::vector<Common::Entities::Log> getAll() const override;

private:
    Common::Entities::Log logFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

    std::shared_ptr<IDatabaseManager> m_dbManager;
};
