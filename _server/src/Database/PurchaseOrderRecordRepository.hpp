#pragma once

#include <memory>
#include <vector>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"

#include "common/Entities/PurchaseOrderRecord.hpp"

class PurchaseOrderRecordRepository : public IRepository<Common::Entities::PurchaseOrderRecord> {
public:
    explicit PurchaseOrderRecordRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~PurchaseOrderRecordRepository() override;

    void add(const Common::Entities::PurchaseOrderRecord &entity) override;
    void update(const Common::Entities::PurchaseOrderRecord &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<Common::Entities::PurchaseOrderRecord>
    getByField(const std::string &fieldName, const std::string &value) const override;
    std::vector<Common::Entities::PurchaseOrderRecord> getAll() const override;

private:
    Common::Entities::PurchaseOrderRecord
    purchaseOrderRecordFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

    std::shared_ptr<IDatabaseManager> m_dbManager;
};
