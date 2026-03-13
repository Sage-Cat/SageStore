#pragma once

#include <memory>
#include <vector>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"

#include "common/Entities/PurchaseOrder.hpp"

class PurchaseOrderRepository : public IRepository<Common::Entities::PurchaseOrder> {
public:
    explicit PurchaseOrderRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~PurchaseOrderRepository() override;

    void add(const Common::Entities::PurchaseOrder &entity) override;
    void update(const Common::Entities::PurchaseOrder &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<Common::Entities::PurchaseOrder> getByField(const std::string &fieldName,
                                                            const std::string &value) const override;
    std::vector<Common::Entities::PurchaseOrder> getAll() const override;

private:
    Common::Entities::PurchaseOrder
    purchaseOrderFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

    std::shared_ptr<IDatabaseManager> m_dbManager;
};
