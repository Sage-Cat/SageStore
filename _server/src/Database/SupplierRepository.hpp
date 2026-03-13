#pragma once

#include <memory>
#include <vector>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"

#include "common/Entities/Supplier.hpp"

class SupplierRepository : public IRepository<Common::Entities::Supplier> {
public:
    explicit SupplierRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~SupplierRepository() override;

    void add(const Common::Entities::Supplier &entity) override;
    void update(const Common::Entities::Supplier &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<Common::Entities::Supplier> getByField(const std::string &fieldName,
                                                       const std::string &value) const override;
    std::vector<Common::Entities::Supplier> getAll() const override;

private:
    Common::Entities::Supplier
    supplierFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

    std::shared_ptr<IDatabaseManager> m_dbManager;
};
