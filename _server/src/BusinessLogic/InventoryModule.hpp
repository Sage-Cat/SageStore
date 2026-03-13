#pragma once

#include <memory>

#include "DataSpecs.hpp"
#include "IBusinessModule.hpp"

#include "Database/IRepository.hpp"

#include "common/Entities/Inventory.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Entities/Supplier.hpp"
#include "common/Entities/SuppliersProductInfo.hpp"

class RepositoryManager;

class InventoryModule : public IBusinessModule {
public:
    explicit InventoryModule(RepositoryManager &repositoryManager);
    ~InventoryModule() override;

    ResponseData executeTask(const RequestData &requestData) override;

private:
    ResponseData getProductTypes() const;
    ResponseData getStocks() const;
    ResponseData getSupplierProducts() const;
    void addProductType(const Dataset &request) const;
    void editProductType(const Dataset &request, const std::string &resourceId) const;
    void deleteProductType(const std::string &resourceId) const;
    void addStock(const Dataset &request) const;
    void editStock(const Dataset &request, const std::string &resourceId) const;
    void deleteStock(const std::string &resourceId) const;
    void addSupplierProduct(const Dataset &request) const;
    void editSupplierProduct(const Dataset &request, const std::string &resourceId) const;
    void deleteSupplierProduct(const std::string &resourceId) const;

    std::shared_ptr<IRepository<Common::Entities::Inventory>> m_inventoryRepository;
    std::shared_ptr<IRepository<Common::Entities::ProductType>> m_productTypesRepository;
    std::shared_ptr<IRepository<Common::Entities::Supplier>> m_suppliersRepository;
    std::shared_ptr<IRepository<Common::Entities::SuppliersProductInfo>>
        m_suppliersProductInfoRepository;
};
