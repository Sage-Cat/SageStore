#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"

#include "common/Entities/ProductInfo.hpp"

class ProductInfoRepository : public IRepository<Common::Entities::ProductInfo> {
public:
    explicit ProductInfoRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~ProductInfoRepository() override;

    void add(const Common::Entities::ProductInfo &entity) override;
    void update(const Common::Entities::ProductInfo &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<Common::Entities::ProductInfo> getByField(const std::string &fieldName,
                                                          const std::string &value) const override;
    std::vector<Common::Entities::ProductInfo> getAll() const override;

private:
    Common::Entities::ProductInfo
    userFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

private:
    std::shared_ptr<IDatabaseManager> m_dbManager;
};
