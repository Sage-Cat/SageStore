#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"
#include "common/Entities/PurchaseOrder.hpp"

using PurchaseOrder = Common::Entities::PurchaseOrder;

class PurchaseOrderRepository : public IRepository<Common::Entities::PurchaseOrder> {
public:
    explicit PurchaseOrderRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~PurchaseOrderRepository() override;

    void add(const PurchaseOrder &entity) override;
    void update(const PurchaseOrder &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<PurchaseOrder> getByField(const std::string &fieldName,
                                          const std::string &value) const override;
    std::vector<PurchaseOrder> getAll() const override;

private:
    PurchaseOrder PurchaseFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

private:
    std::shared_ptr<IDatabaseManager> m_dbManager;
};