#pragma once

#include <memory>
#include <vector>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"

#include "common/Entities/SaleOrder.hpp"

class SaleOrderRepository : public IRepository<Common::Entities::SaleOrder> {
public:
    explicit SaleOrderRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~SaleOrderRepository() override;

    void add(const Common::Entities::SaleOrder &entity) override;
    void update(const Common::Entities::SaleOrder &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<Common::Entities::SaleOrder> getByField(const std::string &fieldName,
                                                        const std::string &value) const override;
    std::vector<Common::Entities::SaleOrder> getAll() const override;

private:
    Common::Entities::SaleOrder
    saleOrderFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

    std::shared_ptr<IDatabaseManager> m_dbManager;
};
