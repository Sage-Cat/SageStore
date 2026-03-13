#pragma once

#include <memory>

#include "DataSpecs.hpp"
#include "IBusinessModule.hpp"

#include "Database/IRepository.hpp"

#include "common/Entities/ProductType.hpp"
#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"
#include "common/Entities/Inventory.hpp"
#include "common/Entities/Employee.hpp"
#include "common/Entities/Supplier.hpp"
#include "common/Entities/User.hpp"

class RepositoryManager;

class PurchaseModule : public IBusinessModule {
public:
    explicit PurchaseModule(RepositoryManager &repositoryManager);
    ~PurchaseModule() override;

    ResponseData executeTask(const RequestData &requestData) override;

private:
    ResponseData getOrders() const;
    ResponseData getOrderRecords(const std::string &orderId) const;
    void addOrder(const Dataset &request) const;
    void editOrder(const Dataset &request, const std::string &resourceId) const;
    void deleteOrder(const std::string &resourceId) const;
    void receiveOrder(const Dataset &request) const;
    void addOrderRecord(const Dataset &request) const;
    void editOrderRecord(const Dataset &request, const std::string &resourceId) const;
    void deleteOrderRecord(const std::string &resourceId) const;

    RepositoryManager &m_repositoryManager;
    std::shared_ptr<IRepository<Common::Entities::PurchaseOrder>> m_ordersRepository;
    std::shared_ptr<IRepository<Common::Entities::PurchaseOrderRecord>> m_orderRecordsRepository;
    std::shared_ptr<IRepository<Common::Entities::Inventory>> m_inventoryRepository;
    std::shared_ptr<IRepository<Common::Entities::User>> m_usersRepository;
    std::shared_ptr<IRepository<Common::Entities::Supplier>> m_suppliersRepository;
    std::shared_ptr<IRepository<Common::Entities::Employee>> m_employeesRepository;
    std::shared_ptr<IRepository<Common::Entities::ProductType>> m_productTypesRepository;
};
