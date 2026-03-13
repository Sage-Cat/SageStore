#pragma once

#include <memory>

#include "DataSpecs.hpp"
#include "IBusinessModule.hpp"

#include "Database/IRepository.hpp"

#include "common/Entities/ProductType.hpp"

class RepositoryManager;

class InventoryModule : public IBusinessModule {
public:
    explicit InventoryModule(RepositoryManager &repositoryManager);
    ~InventoryModule() override;

    ResponseData executeTask(const RequestData &requestData) override;

private:
    ResponseData getProductTypes() const;
    void addProductType(const Dataset &request) const;
    void editProductType(const Dataset &request, const std::string &resourceId) const;
    void deleteProductType(const std::string &resourceId) const;

    std::shared_ptr<IRepository<Common::Entities::ProductType>> m_productTypesRepository;
};
