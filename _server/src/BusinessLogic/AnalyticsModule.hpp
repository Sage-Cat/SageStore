#pragma once

#include <memory>

#include "DataSpecs.hpp"
#include "IBusinessModule.hpp"

#include "Database/IRepository.hpp"

#include "common/Entities/Inventory.hpp"
#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Entities/SaleOrder.hpp"
#include "common/Entities/SalesOrderRecord.hpp"

class RepositoryManager;

class AnalyticsModule : public IBusinessModule {
public:
    explicit AnalyticsModule(RepositoryManager &repositoryManager);
    ~AnalyticsModule() override;

    ResponseData executeTask(const RequestData &requestData) override;

private:
    ResponseData getSalesAnalytics() const;
    ResponseData getInventoryAnalytics() const;

    std::shared_ptr<IRepository<Common::Entities::SaleOrder>> m_salesOrdersRepository;
    std::shared_ptr<IRepository<Common::Entities::SalesOrderRecord>> m_salesOrderRecordsRepository;
    std::shared_ptr<IRepository<Common::Entities::PurchaseOrder>> m_purchaseOrdersRepository;
    std::shared_ptr<IRepository<Common::Entities::PurchaseOrderRecord>>
        m_purchaseOrderRecordsRepository;
    std::shared_ptr<IRepository<Common::Entities::Inventory>> m_inventoryRepository;
    std::shared_ptr<IRepository<Common::Entities::ProductType>> m_productTypesRepository;
};
