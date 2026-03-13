#include "RepositoryManager.hpp"

#include "ContactRepository.hpp"
#include "EmployeeRepository.hpp"
#include "InventoryRepository.hpp"
#include "LogRepository.hpp"
#include "PurchaseOrderRecordRepository.hpp"
#include "PurchaseOrderRepository.hpp"
#include "ProductTypeRepository.hpp"
#include "RoleRepository.hpp"
#include "SaleOrderRepository.hpp"
#include "SalesOrderRecordRepository.hpp"
#include "SupplierRepository.hpp"
#include "SuppliersProductInfoRepository.hpp"
#include "UserRepository.hpp"

#include "common/SpdlogConfig.hpp"

RepositoryManager::RepositoryManager(std::shared_ptr<IDatabaseManager> dbManager)
    : m_dbManager(std::move(dbManager))
{
    SPDLOG_TRACE("RepositoryManager::RepositoryManager");

    m_dbManager->open();
}

RepositoryManager::~RepositoryManager() { m_dbManager->close(); }

auto RepositoryManager::getUserRepository() -> std::shared_ptr<IRepository<Common::Entities::User>>
{
    SPDLOG_TRACE("RepositoryManager::getUserRepository");
    if (!m_usersRepository) {
        m_usersRepository = std::make_shared<UserRepository>(m_dbManager);
    }
    return m_usersRepository;
}

auto RepositoryManager::getRoleRepository() -> std::shared_ptr<IRepository<Common::Entities::Role>>
{
    SPDLOG_TRACE("RepositoryManager::getRoleRepository");
    if (!m_rolesRepository) {
        m_rolesRepository = std::make_shared<RoleRepository>(m_dbManager);
    }
    return m_rolesRepository;
}

auto RepositoryManager::getInventoryRepository()
    -> std::shared_ptr<IRepository<Common::Entities::Inventory>>
{
    SPDLOG_TRACE("RepositoryManager::getInventoryRepository");
    if (!m_inventoryRepository) {
        m_inventoryRepository = std::make_shared<InventoryRepository>(m_dbManager);
    }
    return m_inventoryRepository;
}

auto RepositoryManager::getContactRepository()
    -> std::shared_ptr<IRepository<Common::Entities::Contact>>
{
    SPDLOG_TRACE("RepositoryManager::getContactRepository");
    if (!m_contactsRepository) {
        m_contactsRepository = std::make_shared<ContactRepository>(m_dbManager);
    }
    return m_contactsRepository;
}

auto RepositoryManager::getEmployeeRepository()
    -> std::shared_ptr<IRepository<Common::Entities::Employee>>
{
    SPDLOG_TRACE("RepositoryManager::getEmployeeRepository");
    if (!m_employeesRepository) {
        m_employeesRepository = std::make_shared<EmployeeRepository>(m_dbManager);
    }
    return m_employeesRepository;
}

auto RepositoryManager::getLogRepository() -> std::shared_ptr<IRepository<Common::Entities::Log>>
{
    SPDLOG_TRACE("RepositoryManager::getLogRepository");
    if (!m_logsRepository) {
        m_logsRepository = std::make_shared<LogRepository>(m_dbManager);
    }
    return m_logsRepository;
}

auto RepositoryManager::getPurchaseOrderRepository()
    -> std::shared_ptr<IRepository<Common::Entities::PurchaseOrder>>
{
    SPDLOG_TRACE("RepositoryManager::getPurchaseOrderRepository");
    if (!m_purchaseOrdersRepository) {
        m_purchaseOrdersRepository = std::make_shared<PurchaseOrderRepository>(m_dbManager);
    }
    return m_purchaseOrdersRepository;
}

auto RepositoryManager::getPurchaseOrderRecordRepository()
    -> std::shared_ptr<IRepository<Common::Entities::PurchaseOrderRecord>>
{
    SPDLOG_TRACE("RepositoryManager::getPurchaseOrderRecordRepository");
    if (!m_purchaseOrderRecordsRepository) {
        m_purchaseOrderRecordsRepository =
            std::make_shared<PurchaseOrderRecordRepository>(m_dbManager);
    }
    return m_purchaseOrderRecordsRepository;
}

auto RepositoryManager::getProductTypeRepository()
    -> std::shared_ptr<IRepository<Common::Entities::ProductType>>
{
    SPDLOG_TRACE("RepositoryManager::getProductTypeRepository");
    if (!m_productTypesRepository) {
        m_productTypesRepository = std::make_shared<ProductTypeRepository>(m_dbManager);
    }
    return m_productTypesRepository;
}

auto RepositoryManager::getSaleOrderRepository()
    -> std::shared_ptr<IRepository<Common::Entities::SaleOrder>>
{
    SPDLOG_TRACE("RepositoryManager::getSaleOrderRepository");
    if (!m_saleOrdersRepository) {
        m_saleOrdersRepository = std::make_shared<SaleOrderRepository>(m_dbManager);
    }
    return m_saleOrdersRepository;
}

auto RepositoryManager::getSalesOrderRecordRepository()
    -> std::shared_ptr<IRepository<Common::Entities::SalesOrderRecord>>
{
    SPDLOG_TRACE("RepositoryManager::getSalesOrderRecordRepository");
    if (!m_salesOrderRecordsRepository) {
        m_salesOrderRecordsRepository = std::make_shared<SalesOrderRecordRepository>(m_dbManager);
    }
    return m_salesOrderRecordsRepository;
}

auto RepositoryManager::getSupplierRepository()
    -> std::shared_ptr<IRepository<Common::Entities::Supplier>>
{
    SPDLOG_TRACE("RepositoryManager::getSupplierRepository");
    if (!m_suppliersRepository) {
        m_suppliersRepository = std::make_shared<SupplierRepository>(m_dbManager);
    }
    return m_suppliersRepository;
}

auto RepositoryManager::getSuppliersProductInfoRepository()
    -> std::shared_ptr<IRepository<Common::Entities::SuppliersProductInfo>>
{
    SPDLOG_TRACE("RepositoryManager::getSuppliersProductInfoRepository");
    if (!m_suppliersProductInfoRepository) {
        m_suppliersProductInfoRepository =
            std::make_shared<SuppliersProductInfoRepository>(m_dbManager);
    }
    return m_suppliersProductInfoRepository;
}

void RepositoryManager::runInTransaction(const std::function<void()> &operation)
{
    m_dbManager->beginTransaction();
    try {
        operation();
        m_dbManager->commitTransaction();
    } catch (...) {
        try {
            m_dbManager->rollbackTransaction();
        } catch (...) {
            SPDLOG_ERROR("RepositoryManager::runInTransaction | rollback failed");
        }
        throw;
    }
}
