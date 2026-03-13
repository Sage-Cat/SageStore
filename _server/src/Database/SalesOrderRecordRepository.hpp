#pragma once

#include <memory>
#include <vector>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"

#include "common/Entities/SalesOrderRecord.hpp"

class SalesOrderRecordRepository : public IRepository<Common::Entities::SalesOrderRecord> {
public:
    explicit SalesOrderRecordRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~SalesOrderRecordRepository() override;

    void add(const Common::Entities::SalesOrderRecord &entity) override;
    void update(const Common::Entities::SalesOrderRecord &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<Common::Entities::SalesOrderRecord>
    getByField(const std::string &fieldName, const std::string &value) const override;
    std::vector<Common::Entities::SalesOrderRecord> getAll() const override;

private:
    Common::Entities::SalesOrderRecord
    salesOrderRecordFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

    std::shared_ptr<IDatabaseManager> m_dbManager;
};
