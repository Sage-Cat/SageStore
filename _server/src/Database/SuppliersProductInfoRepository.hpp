#pragma once

#include <memory>
#include <vector>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"

#include "common/Entities/SuppliersProductInfo.hpp"

class SuppliersProductInfoRepository
    : public IRepository<Common::Entities::SuppliersProductInfo> {
public:
    explicit SuppliersProductInfoRepository(std::shared_ptr<IDatabaseManager> dbManager);
    ~SuppliersProductInfoRepository() override;

    void add(const Common::Entities::SuppliersProductInfo &entity) override;
    void update(const Common::Entities::SuppliersProductInfo &entity) override;
    void deleteResource(const std::string &id) override;
    std::vector<Common::Entities::SuppliersProductInfo>
    getByField(const std::string &fieldName, const std::string &value) const override;
    std::vector<Common::Entities::SuppliersProductInfo> getAll() const override;

private:
    Common::Entities::SuppliersProductInfo
    suppliersProductInfoFromCurrentRow(const std::shared_ptr<IQueryResult> &queryResult) const;

    std::shared_ptr<IDatabaseManager> m_dbManager;
};
