#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "BusinessLogic/InventoryModule.hpp"
#include "ServerException.hpp"
#include "mocks/RepositoryManagerMock.hpp"
#include "mocks/RepositoryMock.hpp"

#include "common/Entities/ProductType.hpp"
#include "common/SpdlogConfig.hpp"

using ::testing::Return;
using ::testing::Truly;

class InventoryModuleTest : public ::testing::Test {
protected:
    std::unique_ptr<InventoryModule> inventoryModule;
    std::shared_ptr<RepositoryManagerMock> repositoryManagerMock;
    std::shared_ptr<RepositoryMock<Common::Entities::ProductType>> productTypesRepositoryMock;

    InventoryModuleTest()
        : repositoryManagerMock(std::make_shared<RepositoryManagerMock>()),
          productTypesRepositoryMock(
              std::make_shared<RepositoryMock<Common::Entities::ProductType>>())
    {
        EXPECT_CALL(*repositoryManagerMock, getProductTypeRepository())
            .WillRepeatedly(Return(productTypesRepositoryMock));

        inventoryModule = std::make_unique<InventoryModule>(*repositoryManagerMock);
    }
};

TEST_F(InventoryModuleTest, getProductTypes)
{
    RequestData requestData{.module = "inventory",
                            .submodule = "product-types",
                            .method = "GET",
                            .resourceId = "",
                            .dataset = {}};

    EXPECT_CALL(*productTypesRepositoryMock, getAll())
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{
            Common::Entities::ProductType{.id = "1",
                                          .code = "PT-001",
                                          .barcode = "123456",
                                          .name = "Oil",
                                          .description = "Synthetic oil",
                                          .lastPrice = "10.50",
                                          .unit = "pcs",
                                          .isImported = "0"}}));

    const auto response = inventoryModule->executeTask(requestData);
    EXPECT_EQ(response.dataset.at(Common::Entities::ProductType::ID_KEY).front(), "1");
    EXPECT_EQ(response.dataset.at(Common::Entities::ProductType::CODE_KEY).front(), "PT-001");
    EXPECT_EQ(response.dataset.at(Common::Entities::ProductType::NAME_KEY).front(), "Oil");
}

TEST_F(InventoryModuleTest, addProductType)
{
    RequestData requestData{
        .module = "inventory",
        .submodule = "product-types",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::ProductType::CODE_KEY, {"PT-002"}},
                    {Common::Entities::ProductType::NAME_KEY, {"Brake fluid"}},
                    {Common::Entities::ProductType::UNIT_KEY, {"bottle"}},
                    {Common::Entities::ProductType::LAST_PRICE_KEY, {"12.30"}}}};

    EXPECT_CALL(*productTypesRepositoryMock,
                getByField(Common::Entities::ProductType::CODE_KEY, "PT-002"))
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{}));
    EXPECT_CALL(*productTypesRepositoryMock,
                add(Truly([](const Common::Entities::ProductType &productType) {
                    return productType.id.empty() && productType.code == "PT-002" &&
                           productType.name == "Brake fluid" &&
                           productType.lastPrice == "12.30" && productType.unit == "bottle";
                }))).WillOnce(Return());

    EXPECT_NO_THROW(inventoryModule->executeTask(requestData));
}

TEST_F(InventoryModuleTest, addProductTypeFailsWhenCodeAlreadyExists)
{
    RequestData requestData{
        .module = "inventory",
        .submodule = "product-types",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::ProductType::CODE_KEY, {"PT-001"}},
                    {Common::Entities::ProductType::NAME_KEY, {"Duplicate"}},
                    {Common::Entities::ProductType::UNIT_KEY, {"pcs"}}}};

    EXPECT_CALL(*productTypesRepositoryMock,
                getByField(Common::Entities::ProductType::CODE_KEY, "PT-001"))
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{
            Common::Entities::ProductType{.id = "1", .code = "PT-001"}}));

    EXPECT_THROW(inventoryModule->executeTask(requestData), ServerException);
}

TEST_F(InventoryModuleTest, editProductType)
{
    RequestData requestData{
        .module = "inventory",
        .submodule = "product-types",
        .method = "PUT",
        .resourceId = "1",
        .dataset = {{Common::Entities::ProductType::CODE_KEY, {"PT-001"}},
                    {Common::Entities::ProductType::NAME_KEY, {"Updated oil"}},
                    {Common::Entities::ProductType::UNIT_KEY, {"pcs"}},
                    {Common::Entities::ProductType::IS_IMPORTED_KEY, {"true"}}}};

    EXPECT_CALL(*productTypesRepositoryMock, getByField(Common::Entities::ProductType::ID_KEY, "1"))
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{
            Common::Entities::ProductType{.id = "1", .code = "PT-001"}}));
    EXPECT_CALL(*productTypesRepositoryMock,
                getByField(Common::Entities::ProductType::CODE_KEY, "PT-001"))
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{
            Common::Entities::ProductType{.id = "1", .code = "PT-001"}}));
    EXPECT_CALL(*productTypesRepositoryMock,
                update(Truly([](const Common::Entities::ProductType &productType) {
                    return productType.id == "1" && productType.name == "Updated oil" &&
                           productType.isImported == "1";
                }))).WillOnce(Return());

    EXPECT_NO_THROW(inventoryModule->executeTask(requestData));
}

TEST_F(InventoryModuleTest, editProductTypeFailsWhenMissing)
{
    RequestData requestData{
        .module = "inventory",
        .submodule = "product-types",
        .method = "PUT",
        .resourceId = "999",
        .dataset = {{Common::Entities::ProductType::CODE_KEY, {"PT-999"}},
                    {Common::Entities::ProductType::NAME_KEY, {"Missing product"}},
                    {Common::Entities::ProductType::UNIT_KEY, {"pcs"}}}};

    EXPECT_CALL(*productTypesRepositoryMock,
                getByField(Common::Entities::ProductType::ID_KEY, "999"))
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{}));
    EXPECT_CALL(*productTypesRepositoryMock,
                update(::testing::An<const Common::Entities::ProductType &>()))
        .Times(0);

    EXPECT_THROW(inventoryModule->executeTask(requestData), ServerException);
}

TEST_F(InventoryModuleTest, deleteProductType)
{
    RequestData requestData{.module = "inventory",
                            .submodule = "product-types",
                            .method = "DELETE",
                            .resourceId = "3",
                            .dataset = {}};

    EXPECT_CALL(*productTypesRepositoryMock, getByField(Common::Entities::ProductType::ID_KEY, "3"))
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{
            Common::Entities::ProductType{.id = "3", .code = "PT-003"}}));
    EXPECT_CALL(*productTypesRepositoryMock, deleteResource("3")).WillOnce(Return());

    EXPECT_NO_THROW(inventoryModule->executeTask(requestData));
}

TEST_F(InventoryModuleTest, deleteProductTypeFailsWhenMissing)
{
    RequestData requestData{.module = "inventory",
                            .submodule = "product-types",
                            .method = "DELETE",
                            .resourceId = "999",
                            .dataset = {}};

    EXPECT_CALL(*productTypesRepositoryMock,
                getByField(Common::Entities::ProductType::ID_KEY, "999"))
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{}));
    EXPECT_CALL(*productTypesRepositoryMock, deleteResource("999")).Times(0);

    EXPECT_THROW(inventoryModule->executeTask(requestData), ServerException);
}

int main(int argc, char **argv)
{
    SpdlogConfig::init<SpdlogConfig::LogLevel::Off>();
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
