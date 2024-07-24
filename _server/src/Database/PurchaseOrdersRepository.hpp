#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "IRepository.hpp"
#include "IDatabaseManager.hpp"

class Purchaseorder;

class PurchaseOrderRepository : public IRepository<Purchaseorder>
{
public:
    explicit PurchaseOrderRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~PurchaseOrderRepository() override;

    void add(const Purchaseorder &entity) override;
    void update(const Purchaseorder &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<Purchaseorder> getByField(const std::string &fieldName, const std::string &value) const override;
    std::vector<Purchaseorder> getAll() const override;

private:
    Purchaseorder PurchFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;
private:
    std::shared_ptr<IDatabaseManager> m_dbManager;
};