#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <memory>
#include <thread>

#include "BusinessLogic/BusinessLogic.hpp"
#include "Database/InventoryRepository.hpp"
#include "Database/ProductTypeRepository.hpp"
#include "Database/RepositoryManager.hpp"
#include "Database/SqliteDatabaseManager.hpp"
#include "Network/HttpTransaction.hpp"

#include "common/Endpoints.hpp"
#include "common/Entities/Inventory.hpp"
#include "common/Entities/ProductType.hpp"
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
    }

    void TearDown() override
    {
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

    std::filesystem::path m_dbPath;
    std::unique_ptr<RepositoryManager> m_repositoryManager;
    std::unique_ptr<BusinessLogic> m_businessLogic;
    std::shared_ptr<IRepository<Common::Entities::Inventory>> m_inventoryRepository;
    std::shared_ptr<IRepository<Common::Entities::ProductType>> m_productTypeRepository;
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

int main(int argc, char **argv)
{
    SpdlogConfig::init<SpdlogConfig::LogLevel::Off>();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
