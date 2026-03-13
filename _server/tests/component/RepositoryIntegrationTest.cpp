#include <algorithm>
#include <chrono>
#include <filesystem>
#include <memory>

#include "Database/ContactRepository.hpp"
#include <gtest/gtest.h>

#include "Database/EmployeeRepository.hpp"
#include "Database/RepositoryManager.hpp"
#include "Database/InventoryRepository.hpp"
#include "Database/LogRepository.hpp"
#include "Database/ProductTypeRepository.hpp"
#include "Database/PurchaseOrderRecordRepository.hpp"
#include "Database/PurchaseOrderRepository.hpp"
#include "Database/RoleRepository.hpp"
#include "Database/SaleOrderRepository.hpp"
#include "Database/SalesOrderRecordRepository.hpp"
#include "Database/SqliteDatabaseManager.hpp"
#include "Database/SupplierRepository.hpp"
#include "Database/SuppliersProductInfoRepository.hpp"
#include "Database/UserRepository.hpp"

#include "ServerException.hpp"

#include "common/Entities/Contact.hpp"
#include "common/Entities/Employee.hpp"
#include "common/Entities/Inventory.hpp"
#include "common/Entities/Log.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"
#include "common/Entities/Role.hpp"
#include "common/Entities/SaleOrder.hpp"
#include "common/Entities/SalesOrderRecord.hpp"
#include "common/Entities/Supplier.hpp"
#include "common/Entities/SuppliersProductInfo.hpp"
#include "common/Entities/User.hpp"
#include "common/SpdlogConfig.hpp"

class RepositoryIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        const auto uniqueSuffix =
            std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        m_dbPath = std::filesystem::temp_directory_path() /
                   ("sagestore_repository_integration_" + uniqueSuffix + ".db");

        auto dbManager      = std::make_shared<SqliteDatabaseManager>(m_dbPath.string(),
                                                                      TEST_CREATE_DB_SQL_FILE_PATH);
        m_repositoryManager = std::make_unique<RepositoryManager>(dbManager);
        m_userRepository    = m_repositoryManager->getUserRepository();
        m_roleRepository    = m_repositoryManager->getRoleRepository();
        m_inventoryRepository = m_repositoryManager->getInventoryRepository();
        m_productTypeRepository = m_repositoryManager->getProductTypeRepository();
        m_contactRepository = m_repositoryManager->getContactRepository();
        m_employeeRepository = m_repositoryManager->getEmployeeRepository();
        m_logRepository = m_repositoryManager->getLogRepository();
        m_purchaseOrderRepository = m_repositoryManager->getPurchaseOrderRepository();
        m_purchaseOrderRecordRepository = m_repositoryManager->getPurchaseOrderRecordRepository();
        m_saleOrderRepository = m_repositoryManager->getSaleOrderRepository();
        m_salesOrderRecordRepository = m_repositoryManager->getSalesOrderRecordRepository();
        m_supplierRepository = m_repositoryManager->getSupplierRepository();
        m_supplierProductRepository = m_repositoryManager->getSuppliersProductInfoRepository();
    }

    void TearDown() override
    {
        m_supplierProductRepository.reset();
        m_supplierRepository.reset();
        m_salesOrderRecordRepository.reset();
        m_saleOrderRepository.reset();
        m_purchaseOrderRecordRepository.reset();
        m_purchaseOrderRepository.reset();
        m_logRepository.reset();
        m_employeeRepository.reset();
        m_contactRepository.reset();
        m_roleRepository.reset();
        m_userRepository.reset();
        m_inventoryRepository.reset();
        m_productTypeRepository.reset();
        m_repositoryManager.reset();
        std::error_code errorCode;
        std::filesystem::remove(m_dbPath, errorCode);
    }

    std::filesystem::path m_dbPath;
    std::unique_ptr<RepositoryManager> m_repositoryManager;
    std::shared_ptr<IRepository<Common::Entities::User>> m_userRepository;
    std::shared_ptr<IRepository<Common::Entities::Role>> m_roleRepository;
    std::shared_ptr<IRepository<Common::Entities::Inventory>> m_inventoryRepository;
    std::shared_ptr<IRepository<Common::Entities::ProductType>> m_productTypeRepository;
    std::shared_ptr<IRepository<Common::Entities::Contact>> m_contactRepository;
    std::shared_ptr<IRepository<Common::Entities::Employee>> m_employeeRepository;
    std::shared_ptr<IRepository<Common::Entities::Log>> m_logRepository;
    std::shared_ptr<IRepository<Common::Entities::PurchaseOrder>> m_purchaseOrderRepository;
    std::shared_ptr<IRepository<Common::Entities::PurchaseOrderRecord>>
        m_purchaseOrderRecordRepository;
    std::shared_ptr<IRepository<Common::Entities::SaleOrder>> m_saleOrderRepository;
    std::shared_ptr<IRepository<Common::Entities::SalesOrderRecord>> m_salesOrderRecordRepository;
    std::shared_ptr<IRepository<Common::Entities::Supplier>> m_supplierRepository;
    std::shared_ptr<IRepository<Common::Entities::SuppliersProductInfo>>
        m_supplierProductRepository;

    std::string createProductType(const std::string &code)
    {
        m_productTypeRepository->add(Common::Entities::ProductType{.id = "",
                                                                   .code = code,
                                                                   .barcode = code + "-BARCODE",
                                                                   .name = code + "-NAME",
                                                                   .description = code + "-DESC",
                                                                   .lastPrice = "10.00",
                                                                   .unit = "pcs",
                                                                   .isImported = "0"});

        const auto created =
            m_productTypeRepository->getByField(Common::Entities::ProductType::CODE_KEY, code);
        EXPECT_EQ(created.size(), 1U);
        return created.empty() ? std::string{} : created.front().id;
    }

    std::string createSupplier(const std::string &name)
    {
        m_supplierRepository->add(Common::Entities::Supplier{
            .id = "",
            .name = name,
            .number = "SUP-" + name,
            .email = name + "@example.com",
            .address = "Supplier address"});

        const auto created =
            m_supplierRepository->getByField(Common::Entities::Supplier::NAME_KEY, name);
        EXPECT_EQ(created.size(), 1U);
        return created.empty() ? std::string{} : created.front().id;
    }

    std::string createContact(const std::string &name)
    {
        m_contactRepository->add(Common::Entities::Contact{
            .id = "",
            .name = name,
            .type = "Customer",
            .email = name + "@example.com",
            .phone = "+380123456789"});

        const auto created =
            m_contactRepository->getByField(Common::Entities::Contact::NAME_KEY, name);
        EXPECT_EQ(created.size(), 1U);
        return created.empty() ? std::string{} : created.front().id;
    }
};

TEST_F(RepositoryIntegrationTest, RoleRepository_CRUD_RoundTrip)
{
    const auto initialRoles = m_roleRepository->getAll();
    ASSERT_GE(initialRoles.size(), 2U);

    m_roleRepository->add(Common::Entities::Role{.id = "", .name = "auditor"});
    auto createdRoles = m_roleRepository->getByField(Common::Entities::Role::NAME_KEY, "auditor");
    ASSERT_EQ(createdRoles.size(), 1U);
    ASSERT_FALSE(createdRoles.front().id.empty());

    const std::string roleId = createdRoles.front().id;
    m_roleRepository->update(Common::Entities::Role{.id = roleId, .name = "auditor_updated"});
    const auto updatedRoles =
        m_roleRepository->getByField(Common::Entities::Role::NAME_KEY, "auditor_updated");
    ASSERT_EQ(updatedRoles.size(), 1U);
    EXPECT_EQ(updatedRoles.front().id, roleId);

    m_roleRepository->deleteResource(roleId);
    const auto deletedRoles =
        m_roleRepository->getByField(Common::Entities::Role::NAME_KEY, "auditor_updated");
    EXPECT_TRUE(deletedRoles.empty());
}

TEST_F(RepositoryIntegrationTest, UserRepository_CRUD_RoundTrip)
{
    const Common::Entities::User newUser{
        .id = "", .username = "repo_user", .password = "hash_1", .roleId = "1"};
    m_userRepository->add(newUser);

    auto createdUsers =
        m_userRepository->getByField(Common::Entities::User::USERNAME_KEY, "repo_user");
    ASSERT_EQ(createdUsers.size(), 1U);
    ASSERT_FALSE(createdUsers.front().id.empty());
    EXPECT_EQ(createdUsers.front().roleId, "1");

    const std::string userId = createdUsers.front().id;
    m_userRepository->update(Common::Entities::User{
        .id = userId, .username = "repo_user_updated", .password = "hash_2", .roleId = "2"});
    const auto updatedUsers =
        m_userRepository->getByField(Common::Entities::User::USERNAME_KEY, "repo_user_updated");
    ASSERT_EQ(updatedUsers.size(), 1U);
    EXPECT_EQ(updatedUsers.front().password, "hash_2");
    EXPECT_EQ(updatedUsers.front().roleId, "2");

    m_userRepository->deleteResource(userId);
    const auto deletedUsers =
        m_userRepository->getByField(Common::Entities::User::USERNAME_KEY, "repo_user_updated");
    EXPECT_TRUE(deletedUsers.empty());
}

TEST_F(RepositoryIntegrationTest, ProductTypeRepository_CRUD_RoundTrip)
{
    const Common::Entities::ProductType newProductType{.id = "",
                                                       .code = "PT-100",
                                                       .barcode = "100200300",
                                                       .name = "Gear oil",
                                                       .description = "GL-5",
                                                       .lastPrice = "55.20",
                                                       .unit = "pcs",
                                                       .isImported = "1"};
    m_productTypeRepository->add(newProductType);

    auto createdProductTypes =
        m_productTypeRepository->getByField(Common::Entities::ProductType::CODE_KEY, "PT-100");
    ASSERT_EQ(createdProductTypes.size(), 1U);
    ASSERT_FALSE(createdProductTypes.front().id.empty());
    EXPECT_EQ(createdProductTypes.front().name, "Gear oil");

    const std::string productTypeId = createdProductTypes.front().id;
    m_productTypeRepository->update(Common::Entities::ProductType{.id = productTypeId,
                                                                  .code = "PT-100",
                                                                  .barcode = "100200300",
                                                                  .name = "Gear oil updated",
                                                                  .description = "GL-5 updated",
                                                                  .lastPrice = "60.00",
                                                                  .unit = "pcs",
                                                                  .isImported = "0"});

    const auto updatedProductTypes =
        m_productTypeRepository->getByField(Common::Entities::ProductType::CODE_KEY, "PT-100");
    ASSERT_EQ(updatedProductTypes.size(), 1U);
    EXPECT_EQ(updatedProductTypes.front().id, productTypeId);
    EXPECT_EQ(updatedProductTypes.front().name, "Gear oil updated");
    EXPECT_EQ(updatedProductTypes.front().isImported, "0");

    m_productTypeRepository->deleteResource(productTypeId);
    const auto deletedProductTypes =
        m_productTypeRepository->getByField(Common::Entities::ProductType::CODE_KEY, "PT-100");
    EXPECT_TRUE(deletedProductTypes.empty());
}

TEST_F(RepositoryIntegrationTest, InventoryRepository_CRUD_RoundTrip)
{
    const std::string productTypeId = createProductType("PT-INVENTORY-CRUD");
    ASSERT_FALSE(productTypeId.empty());

    const Common::Entities::Inventory newStock{
        .id = "", .productTypeId = productTypeId, .quantityAvailable = "11", .employeeId = "1"};
    m_inventoryRepository->add(newStock);

    auto createdStocks =
        m_inventoryRepository->getByField(Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY,
                                          productTypeId);
    ASSERT_FALSE(createdStocks.empty());

    const auto createdIt =
        std::find_if(createdStocks.begin(), createdStocks.end(),
                     [](const Common::Entities::Inventory &stock) {
                         return stock.quantityAvailable == "11" && stock.employeeId == "1";
                     });
    ASSERT_NE(createdIt, createdStocks.end());
    ASSERT_FALSE(createdIt->id.empty());

    const std::string stockId = createdIt->id;
    m_inventoryRepository->update(Common::Entities::Inventory{
        .id = stockId,
        .productTypeId = productTypeId,
        .quantityAvailable = "19",
        .employeeId = "1"});

    const auto updatedStocks =
        m_inventoryRepository->getByField(Common::Entities::Inventory::ID_KEY, stockId);
    ASSERT_EQ(updatedStocks.size(), 1U);
    EXPECT_EQ(updatedStocks.front().quantityAvailable, "19");

    m_inventoryRepository->deleteResource(stockId);
    const auto deletedStocks =
        m_inventoryRepository->getByField(Common::Entities::Inventory::ID_KEY, stockId);
    EXPECT_TRUE(deletedStocks.empty());
}

TEST_F(RepositoryIntegrationTest, InventoryRepository_RejectsUnknownEmployeeReference)
{
    const std::string productTypeId = createProductType("PT-INVENTORY-FK");
    ASSERT_FALSE(productTypeId.empty());

    EXPECT_THROW(m_inventoryRepository->add(Common::Entities::Inventory{
                     .id = "",
                     .productTypeId = productTypeId,
                     .quantityAvailable = "7",
                     .employeeId = "999"}),
                 ServerException);
}

TEST_F(RepositoryIntegrationTest, InventoryRepository_RejectsDuplicateProductTypeStock)
{
    const std::string productTypeId = createProductType("PT-INVENTORY-UNIQUE");
    ASSERT_FALSE(productTypeId.empty());

    m_inventoryRepository->add(Common::Entities::Inventory{
        .id = "", .productTypeId = productTypeId, .quantityAvailable = "7", .employeeId = "1"});

    EXPECT_THROW(m_inventoryRepository->add(Common::Entities::Inventory{
                     .id = "",
                     .productTypeId = productTypeId,
                     .quantityAvailable = "8",
                     .employeeId = "1"}),
                 ServerException);
}

TEST_F(RepositoryIntegrationTest, ContactRepository_CRUD_RoundTrip)
{
    m_contactRepository->add(Common::Entities::Contact{
        .id = "",
        .name = "repo_contact",
        .type = "Customer",
        .email = "repo_contact@example.com",
        .phone = "123456"});

    auto createdContacts =
        m_contactRepository->getByField(Common::Entities::Contact::NAME_KEY, "repo_contact");
    ASSERT_EQ(createdContacts.size(), 1U);
    ASSERT_FALSE(createdContacts.front().id.empty());
    EXPECT_EQ(createdContacts.front().type, "Customer");

    const std::string contactId = createdContacts.front().id;
    m_contactRepository->update(Common::Entities::Contact{
        .id = contactId,
        .name = "repo_contact_updated",
        .type = "Client",
        .email = "updated@example.com",
        .phone = "654321"});
    const auto updatedContacts = m_contactRepository->getByField(
        Common::Entities::Contact::NAME_KEY, "repo_contact_updated");
    ASSERT_EQ(updatedContacts.size(), 1U);
    EXPECT_EQ(updatedContacts.front().type, "Client");

    m_contactRepository->deleteResource(contactId);
    const auto deletedContacts = m_contactRepository->getByField(
        Common::Entities::Contact::NAME_KEY, "repo_contact_updated");
    EXPECT_TRUE(deletedContacts.empty());
}

TEST_F(RepositoryIntegrationTest, SupplierRepository_CRUD_RoundTrip)
{
    m_supplierRepository->add(Common::Entities::Supplier{
        .id = "",
        .name = "repo_supplier",
        .number = "1001",
        .email = "repo_supplier@example.com",
        .address = "Supplier address"});

    auto createdSuppliers =
        m_supplierRepository->getByField(Common::Entities::Supplier::NAME_KEY, "repo_supplier");
    ASSERT_EQ(createdSuppliers.size(), 1U);
    ASSERT_FALSE(createdSuppliers.front().id.empty());

    const std::string supplierId = createdSuppliers.front().id;
    m_supplierRepository->update(Common::Entities::Supplier{
        .id = supplierId,
        .name = "repo_supplier_updated",
        .number = "1002",
        .email = "repo_supplier_updated@example.com",
        .address = "Updated address"});
    const auto updatedSuppliers = m_supplierRepository->getByField(
        Common::Entities::Supplier::NAME_KEY, "repo_supplier_updated");
    ASSERT_EQ(updatedSuppliers.size(), 1U);
    EXPECT_EQ(updatedSuppliers.front().number, "1002");

    m_supplierRepository->deleteResource(supplierId);
    const auto deletedSuppliers = m_supplierRepository->getByField(
        Common::Entities::Supplier::NAME_KEY, "repo_supplier_updated");
    EXPECT_TRUE(deletedSuppliers.empty());
}

TEST_F(RepositoryIntegrationTest, EmployeeRepository_CRUD_RoundTrip)
{
    m_employeeRepository->add(Common::Entities::Employee{
        .id = "",
        .name = "repo_employee",
        .number = "2001",
        .email = "repo_employee@example.com",
        .address = "Employee address"});

    auto createdEmployees =
        m_employeeRepository->getByField(Common::Entities::Employee::NAME_KEY, "repo_employee");
    ASSERT_EQ(createdEmployees.size(), 1U);
    ASSERT_FALSE(createdEmployees.front().id.empty());

    const std::string employeeId = createdEmployees.front().id;
    m_employeeRepository->update(Common::Entities::Employee{
        .id = employeeId,
        .name = "repo_employee_updated",
        .number = "2002",
        .email = "repo_employee_updated@example.com",
        .address = "Updated employee address"});
    const auto updatedEmployees = m_employeeRepository->getByField(
        Common::Entities::Employee::NAME_KEY, "repo_employee_updated");
    ASSERT_EQ(updatedEmployees.size(), 1U);
    EXPECT_EQ(updatedEmployees.front().number, "2002");

    m_employeeRepository->deleteResource(employeeId);
    const auto deletedEmployees = m_employeeRepository->getByField(
        Common::Entities::Employee::NAME_KEY, "repo_employee_updated");
    EXPECT_TRUE(deletedEmployees.empty());
}

TEST_F(RepositoryIntegrationTest, LogRepository_CRUD_RoundTrip)
{
    m_logRepository->add(Common::Entities::Log{
        .id = "", .userId = "", .action = "repo_action", .timestamp = "2026-03-13 10:00:00"});

    auto createdLogs =
        m_logRepository->getByField(Common::Entities::Log::ACTION_KEY, "repo_action");
    ASSERT_EQ(createdLogs.size(), 1U);
    ASSERT_FALSE(createdLogs.front().id.empty());
    EXPECT_TRUE(createdLogs.front().userId.empty());

    const std::string logId = createdLogs.front().id;
    m_logRepository->update(Common::Entities::Log{
        .id = logId,
        .userId = "1",
        .action = "repo_action_updated",
        .timestamp = "2026-03-13 11:00:00"});
    const auto updatedLogs =
        m_logRepository->getByField(Common::Entities::Log::ACTION_KEY, "repo_action_updated");
    ASSERT_EQ(updatedLogs.size(), 1U);
    EXPECT_EQ(updatedLogs.front().userId, "1");

    m_logRepository->deleteResource(logId);
    const auto deletedLogs =
        m_logRepository->getByField(Common::Entities::Log::ACTION_KEY, "repo_action_updated");
    EXPECT_TRUE(deletedLogs.empty());
}

TEST_F(RepositoryIntegrationTest, PurchaseRepositories_CRUD_RoundTrip)
{
    const std::string supplierId = createSupplier("repo_purchase_supplier");
    ASSERT_FALSE(supplierId.empty());
    const std::string productTypeId = createProductType("PT-PURCHASE-CRUD");
    ASSERT_FALSE(productTypeId.empty());

    m_purchaseOrderRepository->add(Common::Entities::PurchaseOrder{
        .id = "",
        .date = "2026-03-13 12:00:00",
        .userId = "1",
        .supplierId = supplierId,
        .status = "Draft"});

    auto createdOrders = m_purchaseOrderRepository->getByField(
        Common::Entities::PurchaseOrder::DATE_KEY, "2026-03-13 12:00:00");
    ASSERT_EQ(createdOrders.size(), 1U);
    const std::string orderId = createdOrders.front().id;

    m_purchaseOrderRecordRepository->add(Common::Entities::PurchaseOrderRecord{
        .id = "", .orderId = orderId, .productTypeId = productTypeId, .quantity = "7", .price = "9.5"});
    auto createdRecords = m_purchaseOrderRecordRepository->getByField(
        Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, orderId);
    ASSERT_EQ(createdRecords.size(), 1U);
    const std::string recordId = createdRecords.front().id;

    m_purchaseOrderRepository->update(Common::Entities::PurchaseOrder{
        .id = orderId,
        .date = "2026-03-13 12:00:00",
        .userId = "1",
        .supplierId = supplierId,
        .status = "Received"});
    m_purchaseOrderRecordRepository->update(Common::Entities::PurchaseOrderRecord{
        .id = recordId,
        .orderId = orderId,
        .productTypeId = productTypeId,
        .quantity = "9",
        .price = "10.5"});

    const auto updatedOrders =
        m_purchaseOrderRepository->getByField(Common::Entities::PurchaseOrder::ID_KEY, orderId);
    const auto updatedRecords = m_purchaseOrderRecordRepository->getByField(
        Common::Entities::PurchaseOrderRecord::ID_KEY, recordId);
    ASSERT_EQ(updatedOrders.size(), 1U);
    ASSERT_EQ(updatedRecords.size(), 1U);
    EXPECT_EQ(updatedOrders.front().status, "Received");
    EXPECT_EQ(updatedRecords.front().quantity, "9");

    m_purchaseOrderRecordRepository->deleteResource(recordId);
    m_purchaseOrderRepository->deleteResource(orderId);
    EXPECT_TRUE(m_purchaseOrderRecordRepository
                    ->getByField(Common::Entities::PurchaseOrderRecord::ID_KEY, recordId)
                    .empty());
    EXPECT_TRUE(m_purchaseOrderRepository
                    ->getByField(Common::Entities::PurchaseOrder::ID_KEY, orderId)
                    .empty());
}

TEST_F(RepositoryIntegrationTest, SalesRepositories_CRUD_RoundTrip)
{
    const std::string contactId = createContact("repo_sales_contact");
    ASSERT_FALSE(contactId.empty());
    const std::string productTypeId = createProductType("PT-SALES-CRUD");
    ASSERT_FALSE(productTypeId.empty());

    m_saleOrderRepository->add(Common::Entities::SaleOrder{
        .id = "",
        .date = "2026-03-13 13:00:00",
        .userId = "1",
        .contactId = contactId,
        .employeeId = "1",
        .status = "Draft"});

    auto createdOrders = m_saleOrderRepository->getByField(
        Common::Entities::SaleOrder::DATE_KEY, "2026-03-13 13:00:00");
    ASSERT_EQ(createdOrders.size(), 1U);
    const std::string orderId = createdOrders.front().id;

    m_salesOrderRecordRepository->add(Common::Entities::SalesOrderRecord{
        .id = "", .orderId = orderId, .productTypeId = productTypeId, .quantity = "2", .price = "15.5"});
    auto createdRecords = m_salesOrderRecordRepository->getByField(
        Common::Entities::SalesOrderRecord::ORDER_ID_KEY, orderId);
    ASSERT_EQ(createdRecords.size(), 1U);
    const std::string recordId = createdRecords.front().id;

    m_saleOrderRepository->update(Common::Entities::SaleOrder{
        .id = orderId,
        .date = "2026-03-13 13:00:00",
        .userId = "1",
        .contactId = contactId,
        .employeeId = "1",
        .status = "Issued"});
    m_salesOrderRecordRepository->update(Common::Entities::SalesOrderRecord{
        .id = recordId,
        .orderId = orderId,
        .productTypeId = productTypeId,
        .quantity = "3",
        .price = "16.5"});

    const auto updatedOrders =
        m_saleOrderRepository->getByField(Common::Entities::SaleOrder::ID_KEY, orderId);
    const auto updatedRecords = m_salesOrderRecordRepository->getByField(
        Common::Entities::SalesOrderRecord::ID_KEY, recordId);
    ASSERT_EQ(updatedOrders.size(), 1U);
    ASSERT_EQ(updatedRecords.size(), 1U);
    EXPECT_EQ(updatedOrders.front().status, "Issued");
    EXPECT_EQ(updatedRecords.front().quantity, "3");

    m_salesOrderRecordRepository->deleteResource(recordId);
    m_saleOrderRepository->deleteResource(orderId);
    EXPECT_TRUE(m_salesOrderRecordRepository
                    ->getByField(Common::Entities::SalesOrderRecord::ID_KEY, recordId)
                    .empty());
    EXPECT_TRUE(m_saleOrderRepository
                    ->getByField(Common::Entities::SaleOrder::ID_KEY, orderId)
                    .empty());
}

TEST_F(RepositoryIntegrationTest, SuppliersProductInfoRepository_CRUD_RoundTrip)
{
    const std::string supplierId = createSupplier("repo_mapping_supplier");
    ASSERT_FALSE(supplierId.empty());
    const std::string productTypeId = createProductType("PT-MAPPING-CRUD");
    ASSERT_FALSE(productTypeId.empty());

    m_supplierProductRepository->add(Common::Entities::SuppliersProductInfo{
        .id = "", .supplierID = supplierId, .productTypeId = productTypeId, .code = "MAP-001"});

    auto createdMappings = m_supplierProductRepository->getByField(
        Common::Entities::SuppliersProductInfo::CODE_KEY, "MAP-001");
    ASSERT_EQ(createdMappings.size(), 1U);
    const std::string mappingId = createdMappings.front().id;

    m_supplierProductRepository->update(Common::Entities::SuppliersProductInfo{
        .id = mappingId, .supplierID = supplierId, .productTypeId = productTypeId, .code = "MAP-002"});
    const auto updatedMappings = m_supplierProductRepository->getByField(
        Common::Entities::SuppliersProductInfo::CODE_KEY, "MAP-002");
    ASSERT_EQ(updatedMappings.size(), 1U);
    EXPECT_EQ(updatedMappings.front().id, mappingId);

    m_supplierProductRepository->deleteResource(mappingId);
    const auto deletedMappings = m_supplierProductRepository->getByField(
        Common::Entities::SuppliersProductInfo::CODE_KEY, "MAP-002");
    EXPECT_TRUE(deletedMappings.empty());
}

TEST_F(RepositoryIntegrationTest, SuppliersProductInfoRepository_RejectsUnknownSupplierReference)
{
    const std::string productTypeId = createProductType("PT-MAPPING-FK");
    ASSERT_FALSE(productTypeId.empty());

    EXPECT_THROW(m_supplierProductRepository->add(Common::Entities::SuppliersProductInfo{
                     .id = "",
                     .supplierID = "999",
                     .productTypeId = productTypeId,
                     .code = "MAP-FK"}),
                 ServerException);
}

int main(int argc, char **argv)
{
    SpdlogConfig::init<SpdlogConfig::LogLevel::Off>();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
