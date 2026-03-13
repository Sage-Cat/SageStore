#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <limits>
#include <memory>
#include <thread>

#include "BusinessLogic/BusinessLogic.hpp"
#include "Database/ContactRepository.hpp"
#include "Database/InventoryRepository.hpp"
#include "Database/LogRepository.hpp"
#include "Database/ProductTypeRepository.hpp"
#include "Database/PurchaseOrderRecordRepository.hpp"
#include "Database/RepositoryManager.hpp"
#include "Database/SaleOrderRepository.hpp"
#include "Database/SalesOrderRecordRepository.hpp"
#include "Database/SqliteDatabaseManager.hpp"
#include "Database/SupplierRepository.hpp"
#include "Network/HttpTransaction.hpp"

#include "common/AnalyticsKeys.hpp"
#include "common/Endpoints.hpp"
#include "common/Entities/Contact.hpp"
#include "common/Entities/Inventory.hpp"
#include "common/Entities/Log.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"
#include "common/Entities/SaleOrder.hpp"
#include "common/Entities/SalesOrderRecord.hpp"
#include "common/Entities/Supplier.hpp"
#include "common/Entities/SuppliersProductInfo.hpp"
#include "common/Keys.hpp"
#include "common/Network/JsonSerializer.hpp"
#include "common/SpdlogConfig.hpp"

class HttpContractTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        const auto uniqueSuffix =
            std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        m_dbPath = std::filesystem::temp_directory_path() /
                   ("sagestore_http_contract_" + uniqueSuffix + ".db");

        auto dbManager = std::make_shared<SqliteDatabaseManager>(m_dbPath.string(),
                                                                 TEST_CREATE_DB_SQL_FILE_PATH);
        m_repositoryManager      = std::make_unique<RepositoryManager>(dbManager);
        m_businessLogic          = std::make_unique<BusinessLogic>(*m_repositoryManager);
        m_inventoryRepository    = m_repositoryManager->getInventoryRepository();
        m_productTypeRepository  = m_repositoryManager->getProductTypeRepository();
        m_contactRepository      = m_repositoryManager->getContactRepository();
        m_supplierRepository     = m_repositoryManager->getSupplierRepository();
        m_logRepository          = m_repositoryManager->getLogRepository();
        m_purchaseOrderRepository = m_repositoryManager->getPurchaseOrderRepository();
        m_purchaseOrderRecordRepository =
            m_repositoryManager->getPurchaseOrderRecordRepository();
        m_saleOrderRepository        = m_repositoryManager->getSaleOrderRepository();
        m_salesOrderRecordRepository = m_repositoryManager->getSalesOrderRecordRepository();
        m_supplierProductRepository  = m_repositoryManager->getSuppliersProductInfoRepository();
    }

    void TearDown() override
    {
        m_supplierProductRepository.reset();
        m_salesOrderRecordRepository.reset();
        m_saleOrderRepository.reset();
        m_purchaseOrderRecordRepository.reset();
        m_purchaseOrderRepository.reset();
        m_logRepository.reset();
        m_supplierRepository.reset();
        m_contactRepository.reset();
        m_inventoryRepository.reset();
        m_productTypeRepository.reset();
        m_businessLogic.reset();
        m_repositoryManager.reset();

        std::error_code errorCode;
        std::filesystem::remove(m_dbPath, errorCode);
    }

    http::response<http::string_body> performRequest(http::verb verb, const std::string &target,
                                                     const Dataset &dataset = {})
    {
        asio::io_context serverIoc;
        tcp::acceptor acceptor(serverIoc, tcp::endpoint{asio::ip::make_address("127.0.0.1"), 0});
        const auto endpoint = acceptor.local_endpoint();

        asio::io_context clientIoc;
        tcp::socket clientSocket(clientIoc);
        clientSocket.connect(endpoint);

        tcp::socket serverSocket(serverIoc);
        acceptor.accept(serverSocket);

        auto transaction = std::make_shared<HttpTransaction>(
            1ULL, std::move(serverSocket), std::make_unique<JsonSerializer>(),
            [this](RequestData requestData, BusinessLogicCallback callback) {
                m_businessLogic->executeTask(std::move(requestData), std::move(callback));
            });

        transaction->start();
        std::thread serverThread([&serverIoc]() { serverIoc.run(); });

        JsonSerializer serializer;
        http::request<http::string_body> request{verb, target, 11};
        request.set(http::field::host, "127.0.0.1");
        request.set(http::field::content_type, "application/json");
        if (!dataset.empty()) {
            request.body() = serializer.serialize(dataset);
        }
        request.prepare_payload();

        http::write(clientSocket, request);

        beast::flat_buffer buffer;
        http::response<http::string_body> response;
        http::read(clientSocket, buffer, response);

        boost::system::error_code errorCode;
        clientSocket.shutdown(tcp::socket::shutdown_both, errorCode);
        clientSocket.close(errorCode);

        serverThread.join();
        return response;
    }

    Dataset deserializeResponseBody(const std::string &body) const
    {
        JsonSerializer serializer;
        return serializer.deserialize(body);
    }

    std::string createProductTypeAndReturnId(const std::string &code)
    {
        m_productTypeRepository->add(Common::Entities::ProductType{
            .id          = "",
            .code        = code,
            .barcode     = code + "-BARCODE",
            .name        = code + "-NAME",
            .description = code + "-DESCRIPTION",
            .lastPrice   = "10.00",
            .unit        = "pcs",
            .isImported  = "0"});

        const auto created =
            m_productTypeRepository->getByField(Common::Entities::ProductType::CODE_KEY, code);
        EXPECT_EQ(created.size(), 1U);
        return created.empty() ? std::string{} : created.front().id;
    }

    std::string createSupplierAndReturnId(const std::string &name)
    {
        m_supplierRepository->add(Common::Entities::Supplier{
            .id = "",
            .name = name,
            .number = "NUM-" + name,
            .email = name + "@supplier.test",
            .address = "Warehouse street"});

        const auto created =
            m_supplierRepository->getByField(Common::Entities::Supplier::NAME_KEY, name);
        EXPECT_EQ(created.size(), 1U);
        return created.empty() ? std::string{} : created.front().id;
    }

    std::string createContactAndReturnId(const std::string &name)
    {
        m_contactRepository->add(Common::Entities::Contact{
            .id = "",
            .name = name,
            .type = "Customer",
            .email = name + "@contact.test",
            .phone = "+380000000000"});

        const auto created = m_contactRepository->getByField(Common::Entities::Contact::NAME_KEY,
                                                             name);
        EXPECT_EQ(created.size(), 1U);
        return created.empty() ? std::string{} : created.front().id;
    }

    std::string createPurchaseOrderAndReturnId(const std::string &date, const std::string &supplierId,
                                               const std::string &status)
    {
        m_purchaseOrderRepository->add(Common::Entities::PurchaseOrder{
            .id = "", .date = date, .userId = "1", .supplierId = supplierId, .status = status});

        const auto created =
            m_purchaseOrderRepository->getByField(Common::Entities::PurchaseOrder::DATE_KEY, date);
        EXPECT_FALSE(created.empty());
        return created.empty() ? std::string{} : created.back().id;
    }

    std::string createSaleOrderAndReturnId(const std::string &date, const std::string &contactId,
                                           const std::string &status)
    {
        m_saleOrderRepository->add(Common::Entities::SaleOrder{
            .id = "",
            .date = date,
            .userId = "1",
            .contactId = contactId,
            .employeeId = "1",
            .status = status});

        const auto created =
            m_saleOrderRepository->getByField(Common::Entities::SaleOrder::DATE_KEY, date);
        EXPECT_FALSE(created.empty());
        return created.empty() ? std::string{} : created.back().id;
    }

    std::filesystem::path m_dbPath;
    std::unique_ptr<RepositoryManager> m_repositoryManager;
    std::unique_ptr<BusinessLogic> m_businessLogic;
    std::shared_ptr<IRepository<Common::Entities::Inventory>> m_inventoryRepository;
    std::shared_ptr<IRepository<Common::Entities::ProductType>> m_productTypeRepository;
    std::shared_ptr<IRepository<Common::Entities::Contact>> m_contactRepository;
    std::shared_ptr<IRepository<Common::Entities::Supplier>> m_supplierRepository;
    std::shared_ptr<IRepository<Common::Entities::Log>> m_logRepository;
    std::shared_ptr<IRepository<Common::Entities::PurchaseOrder>> m_purchaseOrderRepository;
    std::shared_ptr<IRepository<Common::Entities::PurchaseOrderRecord>>
        m_purchaseOrderRecordRepository;
    std::shared_ptr<IRepository<Common::Entities::SaleOrder>> m_saleOrderRepository;
    std::shared_ptr<IRepository<Common::Entities::SalesOrderRecord>>
        m_salesOrderRecordRepository;
    std::shared_ptr<IRepository<Common::Entities::SuppliersProductInfo>>
        m_supplierProductRepository;
};

TEST_F(HttpContractTest, GetProductTypes_ReturnsSerializedDataset)
{
    m_productTypeRepository->add(Common::Entities::ProductType{.id = "",
                                                               .code = "PT-HTTP-1",
                                                               .barcode = "111222333",
                                                               .name = "Brake cleaner",
                                                               .description = "Spray",
                                                               .lastPrice = "42.10",
                                                               .unit = "pcs",
                                                               .isImported = "0"});

    const auto response =
        performRequest(http::verb::get, Endpoints::Inventory::PRODUCT_TYPES);

    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Common::Entities::ProductType::CODE_KEY));
    const auto &codes = dataset.at(Common::Entities::ProductType::CODE_KEY);
    const auto codeIt = std::find(codes.begin(), codes.end(), "PT-HTTP-1");
    ASSERT_NE(codeIt, codes.end());

    const auto itemIndex =
        static_cast<std::size_t>(std::distance(codes.begin(), codeIt));
    const auto &names = dataset.at(Common::Entities::ProductType::NAME_KEY);
    ASSERT_LT(itemIndex, names.size());
    EXPECT_EQ(*std::next(names.begin(), static_cast<long>(itemIndex)), "Brake cleaner");
}

TEST_F(HttpContractTest, PostProductType_PersistsEntity)
{
    const Dataset requestBody{
        {Common::Entities::ProductType::CODE_KEY, {"PT-HTTP-2"}},
        {Common::Entities::ProductType::BARCODE_KEY, {"555444333"}},
        {Common::Entities::ProductType::NAME_KEY, {"Transmission oil"}},
        {Common::Entities::ProductType::DESCRIPTION_KEY, {"ATF"}},
        {Common::Entities::ProductType::LAST_PRICE_KEY, {"99.99"}},
        {Common::Entities::ProductType::UNIT_KEY, {"pcs"}},
        {Common::Entities::ProductType::IS_IMPORTED_KEY, {"1"}},
    };

    const auto response =
        performRequest(http::verb::post, Endpoints::Inventory::PRODUCT_TYPES, requestBody);

    EXPECT_EQ(response.result(), http::status::ok);

    const auto created = m_productTypeRepository->getByField(
        Common::Entities::ProductType::CODE_KEY, "PT-HTTP-2");
    ASSERT_EQ(created.size(), 1U);
    EXPECT_EQ(created.front().name, "Transmission oil");
    EXPECT_EQ(created.front().isImported, "1");
}

TEST_F(HttpContractTest, PutProductType_UpdatesExistingEntity)
{
    m_productTypeRepository->add(Common::Entities::ProductType{.id = "",
                                                               .code = "PT-HTTP-3",
                                                               .barcode = "333222111",
                                                               .name = "Coolant",
                                                               .description = "Blue",
                                                               .lastPrice = "12.00",
                                                               .unit = "pcs",
                                                               .isImported = "0"});

    const auto existing = m_productTypeRepository->getByField(
        Common::Entities::ProductType::CODE_KEY, "PT-HTTP-3");
    ASSERT_EQ(existing.size(), 1U);

    const Dataset requestBody{
        {Common::Entities::ProductType::CODE_KEY, {"PT-HTTP-3"}},
        {Common::Entities::ProductType::BARCODE_KEY, {"333222111"}},
        {Common::Entities::ProductType::NAME_KEY, {"Coolant premium"}},
        {Common::Entities::ProductType::DESCRIPTION_KEY, {"Blue premium"}},
        {Common::Entities::ProductType::LAST_PRICE_KEY, {"14.50"}},
        {Common::Entities::ProductType::UNIT_KEY, {"pcs"}},
        {Common::Entities::ProductType::IS_IMPORTED_KEY, {"true"}},
    };

    const auto response = performRequest(
        http::verb::put,
        std::string(Endpoints::Inventory::PRODUCT_TYPES) + "/" + existing.front().id, requestBody);

    EXPECT_EQ(response.result(), http::status::ok);

    const auto updated = m_productTypeRepository->getByField(
        Common::Entities::ProductType::CODE_KEY, "PT-HTTP-3");
    ASSERT_EQ(updated.size(), 1U);
    EXPECT_EQ(updated.front().name, "Coolant premium");
    EXPECT_DOUBLE_EQ(std::stod(updated.front().lastPrice), 14.5);
    EXPECT_EQ(updated.front().isImported, "1");
}

TEST_F(HttpContractTest, DeleteProductType_RemovesExistingEntity)
{
    m_productTypeRepository->add(Common::Entities::ProductType{.id = "",
                                                               .code = "PT-HTTP-4",
                                                               .barcode = "999888777",
                                                               .name = "Washer fluid",
                                                               .description = "Winter",
                                                               .lastPrice = "7.25",
                                                               .unit = "pcs",
                                                               .isImported = "0"});

    const auto existing = m_productTypeRepository->getByField(
        Common::Entities::ProductType::CODE_KEY, "PT-HTTP-4");
    ASSERT_EQ(existing.size(), 1U);

    const auto response = performRequest(
        http::verb::delete_,
        std::string(Endpoints::Inventory::PRODUCT_TYPES) + "/" + existing.front().id);

    EXPECT_EQ(response.result(), http::status::ok);

    const auto deleted = m_productTypeRepository->getByField(
        Common::Entities::ProductType::CODE_KEY, "PT-HTTP-4");
    EXPECT_TRUE(deleted.empty());
}

TEST_F(HttpContractTest, PutProductType_ReturnsErrorWhenEntityMissing)
{
    const Dataset requestBody{
        {Common::Entities::ProductType::CODE_KEY, {"PT-HTTP-MISSING"}},
        {Common::Entities::ProductType::BARCODE_KEY, {"333222111"}},
        {Common::Entities::ProductType::NAME_KEY, {"Missing product"}},
        {Common::Entities::ProductType::DESCRIPTION_KEY, {"Blue premium"}},
        {Common::Entities::ProductType::LAST_PRICE_KEY, {"14.50"}},
        {Common::Entities::ProductType::UNIT_KEY, {"pcs"}},
        {Common::Entities::ProductType::IS_IMPORTED_KEY, {"true"}},
    };

    const auto response = performRequest(
        http::verb::put, std::string(Endpoints::Inventory::PRODUCT_TYPES) + "/9999", requestBody);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Keys::_ERROR));
}

TEST_F(HttpContractTest, DeleteProductType_ReturnsErrorWhenEntityMissing)
{
    const auto response =
        performRequest(http::verb::delete_,
                       std::string(Endpoints::Inventory::PRODUCT_TYPES) + "/9999");

    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Keys::_ERROR));
}

TEST_F(HttpContractTest, GetStocks_ReturnsSerializedDataset)
{
    const auto productTypeId = createProductTypeAndReturnId("PT-STOCK-GET");
    ASSERT_FALSE(productTypeId.empty());

    m_inventoryRepository->add(Common::Entities::Inventory{
        .id = "", .productTypeId = productTypeId, .quantityAvailable = "17", .employeeId = "1"});

    const auto response = performRequest(http::verb::get, Endpoints::Inventory::STOCKS);

    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY));
    const auto &quantities = dataset.at(Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY);
    const auto quantityIt  = std::find(quantities.begin(), quantities.end(), "17");
    ASSERT_NE(quantityIt, quantities.end());
}

TEST_F(HttpContractTest, PostStock_PersistsEntity)
{
    const auto productTypeId = createProductTypeAndReturnId("PT-STOCK-POST");
    ASSERT_FALSE(productTypeId.empty());

    const Dataset requestBody{
        {Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, {productTypeId}},
        {Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY, {"21"}},
        {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"1"}},
    };

    const auto response = performRequest(http::verb::post, Endpoints::Inventory::STOCKS, requestBody);

    EXPECT_EQ(response.result(), http::status::ok);

    const auto created =
        m_inventoryRepository->getByField(Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY, "21");
    ASSERT_FALSE(created.empty());
}

TEST_F(HttpContractTest, PutStock_UpdatesExistingEntity)
{
    const auto productTypeId = createProductTypeAndReturnId("PT-STOCK-PUT");
    ASSERT_FALSE(productTypeId.empty());

    m_inventoryRepository->add(Common::Entities::Inventory{
        .id = "", .productTypeId = productTypeId, .quantityAvailable = "9", .employeeId = "1"});

    const auto existing =
        m_inventoryRepository->getByField(Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY, "9");
    ASSERT_FALSE(existing.empty());

    const Dataset requestBody{
        {Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, {productTypeId}},
        {Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY, {"14"}},
        {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"1"}},
    };

    const auto response =
        performRequest(http::verb::put,
                       std::string(Endpoints::Inventory::STOCKS) + "/" + existing.front().id,
                       requestBody);

    EXPECT_EQ(response.result(), http::status::ok);

    const auto updated =
        m_inventoryRepository->getByField(Common::Entities::Inventory::ID_KEY, existing.front().id);
    ASSERT_EQ(updated.size(), 1U);
    EXPECT_EQ(updated.front().quantityAvailable, "14");
}

TEST_F(HttpContractTest, DeleteStock_RemovesExistingEntity)
{
    const auto productTypeId = createProductTypeAndReturnId("PT-STOCK-DELETE");
    ASSERT_FALSE(productTypeId.empty());

    m_inventoryRepository->add(Common::Entities::Inventory{
        .id = "", .productTypeId = productTypeId, .quantityAvailable = "33", .employeeId = "1"});

    const auto existing =
        m_inventoryRepository->getByField(Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY, "33");
    ASSERT_FALSE(existing.empty());

    const auto response =
        performRequest(http::verb::delete_,
                       std::string(Endpoints::Inventory::STOCKS) + "/" + existing.front().id);

    EXPECT_EQ(response.result(), http::status::ok);

    const auto deleted =
        m_inventoryRepository->getByField(Common::Entities::Inventory::ID_KEY, existing.front().id);
    EXPECT_TRUE(deleted.empty());
}

TEST_F(HttpContractTest, PostStock_ReturnsErrorWhenEmployeeReferenceIsUnknown)
{
    const auto productTypeId = createProductTypeAndReturnId("PT-STOCK-BAD-EMPLOYEE");
    ASSERT_FALSE(productTypeId.empty());

    const Dataset requestBody{
        {Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, {productTypeId}},
        {Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY, {"21"}},
        {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"999"}},
    };

    const auto response = performRequest(http::verb::post, Endpoints::Inventory::STOCKS, requestBody);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Keys::_ERROR));
}

TEST_F(HttpContractTest, PutStock_ReturnsErrorWhenStockDoesNotExist)
{
    const auto productTypeId = createProductTypeAndReturnId("PT-STOCK-MISSING-PUT");
    ASSERT_FALSE(productTypeId.empty());

    const Dataset requestBody{
        {Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, {productTypeId}},
        {Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY, {"14"}},
        {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"1"}},
    };

    const auto response =
        performRequest(http::verb::put, std::string(Endpoints::Inventory::STOCKS) + "/9999",
                       requestBody);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Keys::_ERROR));
}

TEST_F(HttpContractTest, DeleteStock_ReturnsErrorWhenStockDoesNotExist)
{
    const auto response =
        performRequest(http::verb::delete_, std::string(Endpoints::Inventory::STOCKS) + "/9999");
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Keys::_ERROR));
}

TEST_F(HttpContractTest, GetContacts_ReturnsSerializedDataset)
{
    const auto contactId = createContactAndReturnId("ContractTestContact");
    ASSERT_FALSE(contactId.empty());

    const auto response = performRequest(http::verb::get, Endpoints::Management::CONTACTS);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Common::Entities::Contact::NAME_KEY));
    const auto &names = dataset.at(Common::Entities::Contact::NAME_KEY);
    EXPECT_NE(std::find(names.begin(), names.end(), "ContractTestContact"), names.end());
}

TEST_F(HttpContractTest, PostSupplierProduct_PersistsMapping)
{
    const auto productTypeId = createProductTypeAndReturnId("PT-SUPPLIER-MAP");
    ASSERT_FALSE(productTypeId.empty());

    const Dataset requestBody{
        {Common::Entities::SuppliersProductInfo::SUPPLIER_ID_KEY, {"1"}},
        {Common::Entities::SuppliersProductInfo::PRODUCT_TYPE_ID_KEY, {productTypeId}},
        {Common::Entities::SuppliersProductInfo::CODE_KEY, {"SUP-CODE-1"}},
        {Common::Entities::Log::USER_ID_KEY, {"1"}},
    };

    const auto response =
        performRequest(http::verb::post, Endpoints::Inventory::SUPPLIER_PRODUCTS, requestBody);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto created = m_supplierProductRepository->getByField(
        Common::Entities::SuppliersProductInfo::CODE_KEY, "SUP-CODE-1");
    ASSERT_EQ(created.size(), 1U);
    EXPECT_EQ(created.front().supplierID, "1");

    const auto auditEntries =
        m_logRepository->getByField(Common::Entities::Log::ACTION_KEY,
                                    "inventory/supplier-products POST");
    ASSERT_EQ(auditEntries.size(), 1U);
    EXPECT_EQ(auditEntries.front().userId, "1");
}

TEST_F(HttpContractTest, PostPurchaseOrder_PersistsEntityAndAuditLog)
{
    const Dataset requestBody{
        {Common::Entities::PurchaseOrder::DATE_KEY, {"2026-03-13 08:00:00"}},
        {Common::Entities::PurchaseOrder::USER_ID_KEY, {"1"}},
        {Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY, {"1"}},
        {Common::Entities::PurchaseOrder::STATUS_KEY, {"Draft"}},
    };

    const auto response =
        performRequest(http::verb::post, Endpoints::Purchase::ORDERS, requestBody);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto created = m_purchaseOrderRepository->getByField(
        Common::Entities::PurchaseOrder::DATE_KEY, "2026-03-13 08:00:00");
    ASSERT_EQ(created.size(), 1U);
    EXPECT_EQ(created.front().status, "Draft");

    const auto auditEntries =
        m_logRepository->getByField(Common::Entities::Log::ACTION_KEY, "purchase/orders POST");
    ASSERT_EQ(auditEntries.size(), 1U);
    EXPECT_EQ(auditEntries.front().userId, "1");
}

TEST_F(HttpContractTest, PostPurchaseOrder_NormalizesLocalizedStatusToCanonical)
{
    const Dataset requestBody{
        {Common::Entities::PurchaseOrder::DATE_KEY, {"2026-03-13 08:30:00"}},
        {Common::Entities::PurchaseOrder::USER_ID_KEY, {"1"}},
        {Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY, {"1"}},
        {Common::Entities::PurchaseOrder::STATUS_KEY, {"Замовлено"}},
    };

    const auto response =
        performRequest(http::verb::post, Endpoints::Purchase::ORDERS, requestBody);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto created = m_purchaseOrderRepository->getByField(
        Common::Entities::PurchaseOrder::DATE_KEY, "2026-03-13 08:30:00");
    ASSERT_EQ(created.size(), 1U);
    EXPECT_EQ(created.front().status, "Ordered");
}

TEST_F(HttpContractTest, PostPurchaseOrder_NormalizesLegacyPendingStatusToCanonical)
{
    const Dataset requestBody{
        {Common::Entities::PurchaseOrder::DATE_KEY, {"2026-03-13 08:35:00"}},
        {Common::Entities::PurchaseOrder::USER_ID_KEY, {"1"}},
        {Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY, {"1"}},
        {Common::Entities::PurchaseOrder::STATUS_KEY, {"Pending"}},
    };

    const auto response =
        performRequest(http::verb::post, Endpoints::Purchase::ORDERS, requestBody);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto created = m_purchaseOrderRepository->getByField(
        Common::Entities::PurchaseOrder::DATE_KEY, "2026-03-13 08:35:00");
    ASSERT_EQ(created.size(), 1U);
    EXPECT_EQ(created.front().status, "Ordered");
}

TEST_F(HttpContractTest, PostPurchaseOrder_ReturnsErrorWhenCreatedAsReceived)
{
    const Dataset requestBody{
        {Common::Entities::PurchaseOrder::DATE_KEY, {"2026-03-13 08:40:00"}},
        {Common::Entities::PurchaseOrder::USER_ID_KEY, {"1"}},
        {Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY, {"1"}},
        {Common::Entities::PurchaseOrder::STATUS_KEY, {"Received"}},
    };

    const auto response =
        performRequest(http::verb::post, Endpoints::Purchase::ORDERS, requestBody);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Keys::_ERROR));

    const auto created = m_purchaseOrderRepository->getByField(
        Common::Entities::PurchaseOrder::DATE_KEY, "2026-03-13 08:40:00");
    EXPECT_TRUE(created.empty());
}

TEST_F(HttpContractTest, PostPurchaseReceipt_UpdatesInventoryAndOrderStatus)
{
    const auto productTypeId = createProductTypeAndReturnId("PT-RECEIPT-HTTP");
    ASSERT_FALSE(productTypeId.empty());
    const auto supplierId =
        createSupplierAndReturnId("Receipt supplier");
    ASSERT_FALSE(supplierId.empty());
    const auto orderId =
        createPurchaseOrderAndReturnId("2026-03-13 09:00:00", supplierId, "Pending");
    ASSERT_FALSE(orderId.empty());

    m_purchaseOrderRecordRepository->add(Common::Entities::PurchaseOrderRecord{
        .id = "",
        .orderId = orderId,
        .productTypeId = productTypeId,
        .quantity = "6",
        .price = "12.50"});

    const Dataset requestBody{
        {Common::Entities::PurchaseOrder::ID_KEY, {orderId}},
        {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"1"}},
        {Common::Entities::Log::USER_ID_KEY, {"1"}},
    };

    const auto response =
        performRequest(http::verb::post, Endpoints::Purchase::RECEIPTS, requestBody);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto updatedOrder =
        m_purchaseOrderRepository->getByField(Common::Entities::PurchaseOrder::ID_KEY, orderId);
    ASSERT_EQ(updatedOrder.size(), 1U);
    EXPECT_EQ(updatedOrder.front().status, "Received");

    const auto updatedStock = m_inventoryRepository->getByField(
        Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, productTypeId);
    ASSERT_EQ(updatedStock.size(), 1U);
    EXPECT_EQ(updatedStock.front().quantityAvailable, "6");
}

TEST_F(HttpContractTest, PostPurchaseReceipt_ReturnsErrorWhenStockQuantityOverflows)
{
    const auto productTypeId = createProductTypeAndReturnId("PT-RECEIPT-OVERFLOW");
    ASSERT_FALSE(productTypeId.empty());
    const auto supplierId = createSupplierAndReturnId("Overflow supplier");
    ASSERT_FALSE(supplierId.empty());
    const auto orderId =
        createPurchaseOrderAndReturnId("2026-03-13 09:30:00", supplierId, "Pending");
    ASSERT_FALSE(orderId.empty());

    m_purchaseOrderRecordRepository->add(Common::Entities::PurchaseOrderRecord{
        .id = "",
        .orderId = orderId,
        .productTypeId = productTypeId,
        .quantity = "1",
        .price = "12.50"});
    m_inventoryRepository->add(Common::Entities::Inventory{
        .id = "",
        .productTypeId = productTypeId,
        .quantityAvailable = std::to_string(std::numeric_limits<long long>::max()),
        .employeeId = "1"});

    const Dataset requestBody{
        {Common::Entities::PurchaseOrder::ID_KEY, {orderId}},
        {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"1"}},
        {Common::Entities::Log::USER_ID_KEY, {"1"}},
    };

    const auto response =
        performRequest(http::verb::post, Endpoints::Purchase::RECEIPTS, requestBody);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Keys::_ERROR));

    const auto updatedOrder =
        m_purchaseOrderRepository->getByField(Common::Entities::PurchaseOrder::ID_KEY, orderId);
    ASSERT_EQ(updatedOrder.size(), 1U);
    EXPECT_EQ(updatedOrder.front().status, "Pending");
}

TEST_F(HttpContractTest, PostPurchaseReceipt_DoesNotApplyPartialStockWhenLaterLineFails)
{
    const auto firstProductTypeId = createProductTypeAndReturnId("PT-RECEIPT-FIRST");
    const auto secondProductTypeId = createProductTypeAndReturnId("PT-RECEIPT-OVERFLOW-LATE");
    ASSERT_FALSE(firstProductTypeId.empty());
    ASSERT_FALSE(secondProductTypeId.empty());
    const auto supplierId = createSupplierAndReturnId("Atomicity supplier");
    ASSERT_FALSE(supplierId.empty());
    const auto orderId =
        createPurchaseOrderAndReturnId("2026-03-13 09:40:00", supplierId, "Pending");
    ASSERT_FALSE(orderId.empty());

    m_purchaseOrderRecordRepository->add(Common::Entities::PurchaseOrderRecord{
        .id = "",
        .orderId = orderId,
        .productTypeId = firstProductTypeId,
        .quantity = "6",
        .price = "12.50"});
    m_purchaseOrderRecordRepository->add(Common::Entities::PurchaseOrderRecord{
        .id = "",
        .orderId = orderId,
        .productTypeId = secondProductTypeId,
        .quantity = "1",
        .price = "9.90"});
    m_inventoryRepository->add(Common::Entities::Inventory{
        .id = "",
        .productTypeId = secondProductTypeId,
        .quantityAvailable = std::to_string(std::numeric_limits<long long>::max()),
        .employeeId = "1"});

    const Dataset requestBody{
        {Common::Entities::PurchaseOrder::ID_KEY, {orderId}},
        {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"1"}},
        {Common::Entities::Log::USER_ID_KEY, {"1"}},
    };

    const auto response =
        performRequest(http::verb::post, Endpoints::Purchase::RECEIPTS, requestBody);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Keys::_ERROR));

    const auto firstStock = m_inventoryRepository->getByField(
        Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, firstProductTypeId);
    EXPECT_TRUE(firstStock.empty());

    const auto secondStock = m_inventoryRepository->getByField(
        Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, secondProductTypeId);
    ASSERT_EQ(secondStock.size(), 1U);
    EXPECT_EQ(secondStock.front().quantityAvailable,
              std::to_string(std::numeric_limits<long long>::max()));

    const auto updatedOrder =
        m_purchaseOrderRepository->getByField(Common::Entities::PurchaseOrder::ID_KEY, orderId);
    ASSERT_EQ(updatedOrder.size(), 1U);
    EXPECT_EQ(updatedOrder.front().status, "Pending");
}

TEST_F(HttpContractTest, PostPurchaseReceipt_ReturnsErrorForLocalizedReceivedStatus)
{
    const auto supplierId = createSupplierAndReturnId("Localized status supplier");
    ASSERT_FALSE(supplierId.empty());
    const auto orderId =
        createPurchaseOrderAndReturnId("2026-03-13 09:45:00", supplierId, "Отримано");
    ASSERT_FALSE(orderId.empty());

    const Dataset requestBody{
        {Common::Entities::PurchaseOrder::ID_KEY, {orderId}},
        {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"1"}},
        {Common::Entities::Log::USER_ID_KEY, {"1"}},
    };

    const auto response =
        performRequest(http::verb::post, Endpoints::Purchase::RECEIPTS, requestBody);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Keys::_ERROR));

    const auto updatedOrder =
        m_purchaseOrderRepository->getByField(Common::Entities::PurchaseOrder::ID_KEY, orderId);
    ASSERT_EQ(updatedOrder.size(), 1U);
    EXPECT_EQ(updatedOrder.front().status, "Отримано");
}

TEST_F(HttpContractTest, PutPurchaseOrder_ReturnsErrorWhenStatusIsSetToReceivedDirectly)
{
    const auto supplierId = createSupplierAndReturnId("Direct received supplier");
    ASSERT_FALSE(supplierId.empty());
    const auto orderId =
        createPurchaseOrderAndReturnId("2026-03-13 09:47:00", supplierId, "Ordered");
    ASSERT_FALSE(orderId.empty());

    const Dataset requestBody{
        {Common::Entities::PurchaseOrder::DATE_KEY, {"2026-03-13 09:47:00"}},
        {Common::Entities::PurchaseOrder::USER_ID_KEY, {"1"}},
        {Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY, {supplierId}},
        {Common::Entities::PurchaseOrder::STATUS_KEY, {"Received"}},
        {Common::Entities::Log::USER_ID_KEY, {"1"}},
    };

    const auto response = performRequest(http::verb::put,
                                         std::string(Endpoints::Purchase::ORDERS) + "/" + orderId,
                                         requestBody);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Keys::_ERROR));

    const auto updatedOrder =
        m_purchaseOrderRepository->getByField(Common::Entities::PurchaseOrder::ID_KEY, orderId);
    ASSERT_EQ(updatedOrder.size(), 1U);
    EXPECT_EQ(updatedOrder.front().status, "Ordered");
}

TEST_F(HttpContractTest, PutPurchaseOrder_ReturnsErrorWhenEditingReceivedOrder)
{
    const auto supplierId = createSupplierAndReturnId("Locked received supplier");
    ASSERT_FALSE(supplierId.empty());
    const auto orderId =
        createPurchaseOrderAndReturnId("2026-03-13 09:48:00", supplierId, "Received");
    ASSERT_FALSE(orderId.empty());

    const Dataset requestBody{
        {Common::Entities::PurchaseOrder::DATE_KEY, {"2026-03-14 10:00:00"}},
        {Common::Entities::PurchaseOrder::USER_ID_KEY, {"1"}},
        {Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY, {supplierId}},
        {Common::Entities::PurchaseOrder::STATUS_KEY, {"Ordered"}},
        {Common::Entities::Log::USER_ID_KEY, {"1"}},
    };

    const auto response = performRequest(http::verb::put,
                                         std::string(Endpoints::Purchase::ORDERS) + "/" + orderId,
                                         requestBody);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Keys::_ERROR));

    const auto updatedOrder =
        m_purchaseOrderRepository->getByField(Common::Entities::PurchaseOrder::ID_KEY, orderId);
    ASSERT_EQ(updatedOrder.size(), 1U);
    EXPECT_EQ(updatedOrder.front().date, "2026-03-13 09:48:00");
    EXPECT_EQ(updatedOrder.front().status, "Received");
}

TEST_F(HttpContractTest, DeletePurchaseOrder_ReturnsErrorWhenOrderWasAlreadyReceived)
{
    const auto supplierId = createSupplierAndReturnId("Delete locked supplier");
    ASSERT_FALSE(supplierId.empty());
    const auto orderId =
        createPurchaseOrderAndReturnId("2026-03-13 09:49:00", supplierId, "Received");
    ASSERT_FALSE(orderId.empty());

    const auto response = performRequest(http::verb::delete_,
                                         std::string(Endpoints::Purchase::ORDERS) + "/" + orderId,
                                         {{Common::Entities::Log::USER_ID_KEY, {"1"}}});
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Keys::_ERROR));

    const auto updatedOrder =
        m_purchaseOrderRepository->getByField(Common::Entities::PurchaseOrder::ID_KEY, orderId);
    ASSERT_EQ(updatedOrder.size(), 1U);
}

TEST_F(HttpContractTest, PostPurchaseOrderRecord_ReturnsErrorWhenParentOrderWasReceived)
{
    const auto productTypeId = createProductTypeAndReturnId("PT-LOCKED-LINE-ADD");
    ASSERT_FALSE(productTypeId.empty());
    const auto supplierId = createSupplierAndReturnId("Locked line add supplier");
    ASSERT_FALSE(supplierId.empty());
    const auto orderId =
        createPurchaseOrderAndReturnId("2026-03-13 09:50:00", supplierId, "Received");
    ASSERT_FALSE(orderId.empty());

    const Dataset requestBody{
        {Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, {orderId}},
        {Common::Entities::PurchaseOrderRecord::PRODUCT_TYPE_ID_KEY, {productTypeId}},
        {Common::Entities::PurchaseOrderRecord::QUANTITY_KEY, {"5"}},
        {Common::Entities::PurchaseOrderRecord::PRICE_KEY, {"15.25"}},
        {Common::Entities::Log::USER_ID_KEY, {"1"}},
    };

    const auto response =
        performRequest(http::verb::post, Endpoints::Purchase::ORDER_RECORDS, requestBody);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Keys::_ERROR));

    const auto records = m_purchaseOrderRecordRepository->getByField(
        Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, orderId);
    EXPECT_TRUE(records.empty());
}

TEST_F(HttpContractTest, PutPurchaseOrderRecord_ReturnsErrorWhenParentOrderWasReceived)
{
    const auto productTypeId = createProductTypeAndReturnId("PT-LOCKED-LINE-EDIT");
    ASSERT_FALSE(productTypeId.empty());
    const auto supplierId = createSupplierAndReturnId("Locked line edit supplier");
    ASSERT_FALSE(supplierId.empty());
    const auto orderId =
        createPurchaseOrderAndReturnId("2026-03-13 09:51:00", supplierId, "Received");
    ASSERT_FALSE(orderId.empty());

    m_purchaseOrderRecordRepository->add(Common::Entities::PurchaseOrderRecord{
        .id = "",
        .orderId = orderId,
        .productTypeId = productTypeId,
        .quantity = "5",
        .price = "15.25"});
    const auto records = m_purchaseOrderRecordRepository->getByField(
        Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, orderId);
    ASSERT_EQ(records.size(), 1U);

    const Dataset requestBody{
        {Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, {orderId}},
        {Common::Entities::PurchaseOrderRecord::PRODUCT_TYPE_ID_KEY, {productTypeId}},
        {Common::Entities::PurchaseOrderRecord::QUANTITY_KEY, {"8"}},
        {Common::Entities::PurchaseOrderRecord::PRICE_KEY, {"16.75"}},
        {Common::Entities::Log::USER_ID_KEY, {"1"}},
    };

    const auto response =
        performRequest(http::verb::put,
                       std::string(Endpoints::Purchase::ORDER_RECORDS) + "/" + records.front().id,
                       requestBody);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Keys::_ERROR));

    const auto unchanged = m_purchaseOrderRecordRepository->getByField(
        Common::Entities::PurchaseOrderRecord::ID_KEY, records.front().id);
    ASSERT_EQ(unchanged.size(), 1U);
    EXPECT_EQ(unchanged.front().quantity, "5");
    EXPECT_EQ(unchanged.front().price, "15.25");
}

TEST_F(HttpContractTest, DeletePurchaseOrderRecord_ReturnsErrorWhenParentOrderWasReceived)
{
    const auto productTypeId = createProductTypeAndReturnId("PT-LOCKED-LINE-DELETE");
    ASSERT_FALSE(productTypeId.empty());
    const auto supplierId = createSupplierAndReturnId("Locked line delete supplier");
    ASSERT_FALSE(supplierId.empty());
    const auto orderId =
        createPurchaseOrderAndReturnId("2026-03-13 09:52:00", supplierId, "Received");
    ASSERT_FALSE(orderId.empty());

    m_purchaseOrderRecordRepository->add(Common::Entities::PurchaseOrderRecord{
        .id = "",
        .orderId = orderId,
        .productTypeId = productTypeId,
        .quantity = "5",
        .price = "15.25"});
    const auto records = m_purchaseOrderRecordRepository->getByField(
        Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, orderId);
    ASSERT_EQ(records.size(), 1U);

    const auto response = performRequest(
        http::verb::delete_, std::string(Endpoints::Purchase::ORDER_RECORDS) + "/" + records.front().id,
        {{Common::Entities::Log::USER_ID_KEY, {"1"}}});
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Keys::_ERROR));

    const auto unchanged = m_purchaseOrderRecordRepository->getByField(
        Common::Entities::PurchaseOrderRecord::ID_KEY, records.front().id);
    ASSERT_EQ(unchanged.size(), 1U);
}

TEST_F(HttpContractTest, PostSalesOrder_ReturnsErrorWhenContactReferenceIsUnknown)
{
    const Dataset requestBody{
        {Common::Entities::SaleOrder::DATE_KEY, {"2026-03-13 11:00:00"}},
        {Common::Entities::SaleOrder::USER_ID_KEY, {"1"}},
        {Common::Entities::SaleOrder::CONTACT_ID_KEY, {"999"}},
        {Common::Entities::SaleOrder::EMPLOYEE_ID_KEY, {"1"}},
        {Common::Entities::SaleOrder::STATUS_KEY, {"Draft"}},
    };

    const auto response = performRequest(http::verb::post, Endpoints::Sales::ORDERS, requestBody);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Keys::_ERROR));
    EXPECT_EQ(dataset.at(Keys::_ERROR).front(), "SalesModule");
}

TEST_F(HttpContractTest, GetSalesAnalytics_ReturnsComputedMetrics)
{
    const auto response = performRequest(http::verb::get, Endpoints::Analytics::SALES);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(AnalyticsKeys::Sales::TOTAL_ORDERS));
    EXPECT_EQ(dataset.at(AnalyticsKeys::Sales::TOTAL_ORDERS).front(), "1");
    EXPECT_EQ(dataset.at(AnalyticsKeys::Sales::TOTAL_ORDER_LINES).front(), "1");
    EXPECT_EQ(dataset.at(AnalyticsKeys::Sales::UNIQUE_CUSTOMERS).front(), "1");
    EXPECT_DOUBLE_EQ(std::stod(dataset.at(AnalyticsKeys::Sales::TOTAL_REVENUE).front()), 10.99);
}

TEST_F(HttpContractTest, GetLogsEntries_ReturnsSerializedDataset)
{
    m_logRepository->add(Common::Entities::Log{
        .id = "", .userId = "1", .action = "manual-seed", .timestamp = "2026-03-13 12:30:00"});

    const auto response = performRequest(http::verb::get, Endpoints::Logs::ENTRIES);
    EXPECT_EQ(response.result(), http::status::ok);

    const auto dataset = deserializeResponseBody(response.body());
    ASSERT_TRUE(dataset.contains(Common::Entities::Log::ACTION_KEY));
    const auto &actions = dataset.at(Common::Entities::Log::ACTION_KEY);
    EXPECT_NE(std::find(actions.begin(), actions.end(), "manual-seed"), actions.end());
}

int main(int argc, char **argv)
{
    SpdlogConfig::init<SpdlogConfig::LogLevel::Off>();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
