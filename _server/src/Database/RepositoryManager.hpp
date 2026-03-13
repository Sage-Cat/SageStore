#pragma once

#include <functional>
#include <memory>
#include <type_traits>

#include "IDatabaseManager.hpp"
#include "IRepository.hpp"

#include "common/Entities/Role.hpp"
#include "common/Entities/Inventory.hpp"
#include "common/Entities/Contact.hpp"
#include "common/Entities/Employee.hpp"
#include "common/Entities/Log.hpp"
#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Entities/SaleOrder.hpp"
#include "common/Entities/SalesOrderRecord.hpp"
#include "common/Entities/Supplier.hpp"
#include "common/Entities/SuppliersProductInfo.hpp"
#include "common/Entities/User.hpp"

class RepositoryManager {
public:
    explicit RepositoryManager(std::shared_ptr<IDatabaseManager> dbManager);
    ~RepositoryManager();

    [[nodiscard]] virtual auto
    getUserRepository() -> std::shared_ptr<IRepository<Common::Entities::User>>;
    [[nodiscard]] virtual auto
    getRoleRepository() -> std::shared_ptr<IRepository<Common::Entities::Role>>;
    [[nodiscard]] virtual auto
    getInventoryRepository() -> std::shared_ptr<IRepository<Common::Entities::Inventory>>;
    [[nodiscard]] virtual auto
    getContactRepository() -> std::shared_ptr<IRepository<Common::Entities::Contact>>;
    [[nodiscard]] virtual auto
    getEmployeeRepository() -> std::shared_ptr<IRepository<Common::Entities::Employee>>;
    [[nodiscard]] virtual auto
    getLogRepository() -> std::shared_ptr<IRepository<Common::Entities::Log>>;
    [[nodiscard]] virtual auto
    getPurchaseOrderRepository() -> std::shared_ptr<IRepository<Common::Entities::PurchaseOrder>>;
    [[nodiscard]] virtual auto getPurchaseOrderRecordRepository()
        -> std::shared_ptr<IRepository<Common::Entities::PurchaseOrderRecord>>;
    [[nodiscard]] virtual auto
    getProductTypeRepository() -> std::shared_ptr<IRepository<Common::Entities::ProductType>>;
    [[nodiscard]] virtual auto
    getSaleOrderRepository() -> std::shared_ptr<IRepository<Common::Entities::SaleOrder>>;
    [[nodiscard]] virtual auto getSalesOrderRecordRepository()
        -> std::shared_ptr<IRepository<Common::Entities::SalesOrderRecord>>;
    [[nodiscard]] virtual auto
    getSupplierRepository() -> std::shared_ptr<IRepository<Common::Entities::Supplier>>;
    [[nodiscard]] virtual auto getSuppliersProductInfoRepository()
        -> std::shared_ptr<IRepository<Common::Entities::SuppliersProductInfo>>;
    virtual void runInTransaction(const std::function<void()> &operation);

private:
    std::shared_ptr<IDatabaseManager> m_dbManager;

    // Repositories
    std::shared_ptr<IRepository<Common::Entities::User>> m_usersRepository;
    std::shared_ptr<IRepository<Common::Entities::Role>> m_rolesRepository;
    std::shared_ptr<IRepository<Common::Entities::Inventory>> m_inventoryRepository;
    std::shared_ptr<IRepository<Common::Entities::Contact>> m_contactsRepository;
    std::shared_ptr<IRepository<Common::Entities::Employee>> m_employeesRepository;
    std::shared_ptr<IRepository<Common::Entities::Log>> m_logsRepository;
    std::shared_ptr<IRepository<Common::Entities::PurchaseOrder>> m_purchaseOrdersRepository;
    std::shared_ptr<IRepository<Common::Entities::PurchaseOrderRecord>>
        m_purchaseOrderRecordsRepository;
    std::shared_ptr<IRepository<Common::Entities::ProductType>> m_productTypesRepository;
    std::shared_ptr<IRepository<Common::Entities::SaleOrder>> m_saleOrdersRepository;
    std::shared_ptr<IRepository<Common::Entities::SalesOrderRecord>> m_salesOrderRecordsRepository;
    std::shared_ptr<IRepository<Common::Entities::Supplier>> m_suppliersRepository;
    std::shared_ptr<IRepository<Common::Entities::SuppliersProductInfo>>
        m_suppliersProductInfoRepository;
};
