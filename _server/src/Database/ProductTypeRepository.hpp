#pragma once

#include <memory>
#include <vector>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"

#include "common/Entities/ProductType.hpp"

class ProductTypeRepository : public IRepository<Common::Entities::ProductType> {
public:
    explicit ProductTypeRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~ProductTypeRepository() override;

    void add(const Common::Entities::ProductType &entity) override;
    void update(const Common::Entities::ProductType &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<Common::Entities::ProductType> getByField(const std::string &fieldName,
                                                          const std::string &value) const override;
    std::vector<Common::Entities::ProductType> getAll() const override;

private:
    Common::Entities::ProductType
    productTypeFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

    std::shared_ptr<IDatabaseManager> m_dbManager;
};
