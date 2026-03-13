#pragma once

#include <memory>
#include <vector>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"

#include "common/Entities/Inventory.hpp"

class InventoryRepository : public IRepository<Common::Entities::Inventory> {
public:
    explicit InventoryRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~InventoryRepository() override;

    void add(const Common::Entities::Inventory &entity) override;
    void update(const Common::Entities::Inventory &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<Common::Entities::Inventory> getByField(const std::string &fieldName,
                                                        const std::string &value) const override;
    std::vector<Common::Entities::Inventory> getAll() const override;

private:
    Common::Entities::Inventory
    inventoryFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

    std::shared_ptr<IDatabaseManager> m_dbManager;
};
