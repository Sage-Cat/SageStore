#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "IRepository.hpp"
#include "IDatabaseManager.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"

using Purchaseorderrecord = Common::Entities::PurchaseOrderRecord;

class PurchaseOrderRecordRepository : public IRepository<Purchaseorderrecord>
{
public:
    explicit PurchaseOrderRecordRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~PurchaseOrderRecordRepository() override;

    void add(const Purchaseorderrecord &entity) override;
    void update(const Purchaseorderrecord &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<Purchaseorderrecord> getByField(const std::string &fieldName, const std::string &value) const override;
    std::vector<Purchaseorderrecord> getAll() const override;

private:
    Purchaseorderrecord PurchFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;
private:
    std::shared_ptr<IDatabaseManager> m_dbManager;
};