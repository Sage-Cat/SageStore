#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"

using PurchaseOrderRecord = Common::Entities::PurchaseOrderRecord;

class PurchaseOrderRecordRepository : public IRepository<PurchaseOrderRecord> {
public:
    explicit PurchaseOrderRecordRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~PurchaseOrderRecordRepository() override;

    void add(const PurchaseOrderRecord &entity) override;
    void update(const PurchaseOrderRecord &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<PurchaseOrderRecord> getByField(const std::string &fieldName,
                                                const std::string &value) const override;
    std::vector<PurchaseOrderRecord> getAll() const override;

private:
    PurchaseOrderRecord
    PurchaseFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

private:
    std::shared_ptr<IDatabaseManager> m_dbManager;
};