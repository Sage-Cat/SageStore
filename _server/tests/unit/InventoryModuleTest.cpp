#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "BusinessLogic/InventoryModule.hpp"
#include "ServerException.hpp"
#include "mocks/RepositoryManagerMock.hpp"
#include "mocks/RepositoryMock.hpp"

#include "common/Entities/Inventory.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Entities/Supplier.hpp"
#include "common/Entities/SuppliersProductInfo.hpp"
#include "common/SpdlogConfig.hpp"

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Truly;

class InventoryModuleTest : public ::testing::Test {
protected:
    std::unique_ptr<InventoryModule> inventoryModule;
    std::shared_ptr<RepositoryManagerMock> repositoryManagerMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Inventory>>> inventoryRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::ProductType>>>
        productTypesRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Supplier>>> suppliersRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::SuppliersProductInfo>>>
        supplierProductsRepositoryMock;

    void SetUp() override
    {
        repositoryManagerMock        = std::make_shared<RepositoryManagerMock>();
        inventoryRepositoryMock      =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::Inventory>>>();
        productTypesRepositoryMock   =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::ProductType>>>();
        suppliersRepositoryMock      =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::Supplier>>>();
        supplierProductsRepositoryMock =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::SuppliersProductInfo>>>();

        EXPECT_CALL(*repositoryManagerMock, getInventoryRepository())
            .WillRepeatedly(Return(inventoryRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getProductTypeRepository())
            .WillRepeatedly(Return(productTypesRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getSupplierRepository())
            .WillRepeatedly(Return(suppliersRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getSuppliersProductInfoRepository())
            .WillRepeatedly(Return(supplierProductsRepositoryMock));

        inventoryModule = std::make_unique<InventoryModule>(*repositoryManagerMock);
    }
};

TEST_F(InventoryModuleTest, GetProductTypes)
{
    const RequestData requestData{.module = "inventory",
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

TEST_F(InventoryModuleTest, AddProductType)
{
    const RequestData requestData{
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
                })));

    EXPECT_NO_THROW(inventoryModule->executeTask(requestData));
}

TEST_F(InventoryModuleTest, AddProductTypeFailsWhenCodeAlreadyExists)
{
    const RequestData requestData{
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

TEST_F(InventoryModuleTest, EditProductType)
{
    const RequestData requestData{
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
                })));

    EXPECT_NO_THROW(inventoryModule->executeTask(requestData));
}

TEST_F(InventoryModuleTest, EditProductTypeFailsWhenMissing)
{
    const RequestData requestData{
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

TEST_F(InventoryModuleTest, DeleteProductType)
{
    const RequestData requestData{.module = "inventory",
                                  .submodule = "product-types",
                                  .method = "DELETE",
                                  .resourceId = "3",
                                  .dataset = {}};

    EXPECT_CALL(*productTypesRepositoryMock,
                getByField(Common::Entities::ProductType::ID_KEY, "3"))
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{
            Common::Entities::ProductType{.id = "3", .code = "PT-003"}}));
    EXPECT_CALL(*productTypesRepositoryMock, deleteResource("3"));

    EXPECT_NO_THROW(inventoryModule->executeTask(requestData));
}

TEST_F(InventoryModuleTest, DeleteProductTypeFailsWhenMissing)
{
    const RequestData requestData{.module = "inventory",
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

TEST_F(InventoryModuleTest, GetStocks)
{
    const RequestData requestData{.module = "inventory",
                                  .submodule = "stocks",
                                  .method = "GET",
                                  .resourceId = "",
                                  .dataset = {}};

    EXPECT_CALL(*inventoryRepositoryMock, getAll())
        .WillOnce(Return(std::vector<Common::Entities::Inventory>{
            Common::Entities::Inventory{.id = "1",
                                        .productTypeId = "2",
                                        .quantityAvailable = "15",
                                        .employeeId = "1"}}));

    const auto response = inventoryModule->executeTask(requestData);
    EXPECT_EQ(response.dataset.at(Common::Entities::Inventory::ID_KEY).front(), "1");
    EXPECT_EQ(response.dataset.at(Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY).front(), "2");
    EXPECT_EQ(response.dataset.at(Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY).front(),
              "15");
}

TEST_F(InventoryModuleTest, AddStock)
{
    const RequestData requestData{
        .module = "inventory",
        .submodule = "stocks",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, {"2"}},
                    {Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY, {"25"}},
                    {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"1"}}}};

    EXPECT_CALL(*productTypesRepositoryMock,
                getByField(Common::Entities::ProductType::ID_KEY, "2"))
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{
            Common::Entities::ProductType{.id = "2", .code = "PT-002"}}));
    EXPECT_CALL(*inventoryRepositoryMock,
                getByField(Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, "2"))
        .WillOnce(Return(std::vector<Common::Entities::Inventory>{}));
    EXPECT_CALL(*inventoryRepositoryMock,
                add(Truly([](const Common::Entities::Inventory &stock) {
                    return stock.id.empty() && stock.productTypeId == "2" &&
                           stock.quantityAvailable == "25" && stock.employeeId == "1";
                })));

    EXPECT_NO_THROW(inventoryModule->executeTask(requestData));
}

TEST_F(InventoryModuleTest, AddStockFailsWhenProductTypeMissing)
{
    const RequestData requestData{
        .module = "inventory",
        .submodule = "stocks",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, {"99"}},
                    {Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY, {"25"}},
                    {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"1"}}}};

    EXPECT_CALL(*productTypesRepositoryMock,
                getByField(Common::Entities::ProductType::ID_KEY, "99"))
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{}));

    EXPECT_THROW(inventoryModule->executeTask(requestData), ServerException);
}

TEST_F(InventoryModuleTest, EditStock)
{
    const RequestData requestData{
        .module = "inventory",
        .submodule = "stocks",
        .method = "PUT",
        .resourceId = "3",
        .dataset = {{Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, {"2"}},
                    {Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY, {"30"}},
                    {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"1"}}}};

    EXPECT_CALL(*inventoryRepositoryMock,
                getByField(Common::Entities::Inventory::ID_KEY, "3"))
        .WillOnce(Return(std::vector<Common::Entities::Inventory>{
            Common::Entities::Inventory{.id = "3",
                                        .productTypeId = "2",
                                        .quantityAvailable = "20",
                                        .employeeId = "1"}}));
    EXPECT_CALL(*productTypesRepositoryMock,
                getByField(Common::Entities::ProductType::ID_KEY, "2"))
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{
            Common::Entities::ProductType{.id = "2", .code = "PT-002"}}));
    EXPECT_CALL(*inventoryRepositoryMock,
                getByField(Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, "2"))
        .WillOnce(Return(std::vector<Common::Entities::Inventory>{
            Common::Entities::Inventory{.id = "3",
                                        .productTypeId = "2",
                                        .quantityAvailable = "20",
                                        .employeeId = "1"}}));
    EXPECT_CALL(*inventoryRepositoryMock,
                update(Truly([](const Common::Entities::Inventory &stock) {
                    return stock.id == "3" && stock.quantityAvailable == "30";
                })));

    EXPECT_NO_THROW(inventoryModule->executeTask(requestData));
}

TEST_F(InventoryModuleTest, EditStockFailsWhenStockDoesNotExist)
{
    const RequestData requestData{
        .module = "inventory",
        .submodule = "stocks",
        .method = "PUT",
        .resourceId = "99",
        .dataset = {{Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, {"2"}},
                    {Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY, {"30"}},
                    {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"1"}}}};

    EXPECT_CALL(*inventoryRepositoryMock,
                getByField(Common::Entities::Inventory::ID_KEY, "99"))
        .WillOnce(Return(std::vector<Common::Entities::Inventory>{}));

    EXPECT_THROW(inventoryModule->executeTask(requestData), ServerException);
}

TEST_F(InventoryModuleTest, DeleteStock)
{
    const RequestData requestData{.module = "inventory",
                                  .submodule = "stocks",
                                  .method = "DELETE",
                                  .resourceId = "3",
                                  .dataset = {}};

    EXPECT_CALL(*inventoryRepositoryMock,
                getByField(Common::Entities::Inventory::ID_KEY, "3"))
        .WillOnce(Return(std::vector<Common::Entities::Inventory>{
            Common::Entities::Inventory{.id = "3",
                                        .productTypeId = "2",
                                        .quantityAvailable = "20",
                                        .employeeId = "1"}}));
    EXPECT_CALL(*inventoryRepositoryMock, deleteResource("3"));

    EXPECT_NO_THROW(inventoryModule->executeTask(requestData));
}

TEST_F(InventoryModuleTest, DeleteStockFailsWhenStockDoesNotExist)
{
    const RequestData requestData{.module = "inventory",
                                  .submodule = "stocks",
                                  .method = "DELETE",
                                  .resourceId = "99",
                                  .dataset = {}};

    EXPECT_CALL(*inventoryRepositoryMock,
                getByField(Common::Entities::Inventory::ID_KEY, "99"))
        .WillOnce(Return(std::vector<Common::Entities::Inventory>{}));

    EXPECT_THROW(inventoryModule->executeTask(requestData), ServerException);
}

TEST_F(InventoryModuleTest, AddStockFailsWhenEmployeeIdIsNotPositive)
{
    const RequestData requestData{
        .module = "inventory",
        .submodule = "stocks",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, {"2"}},
                    {Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY, {"25"}},
                    {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"0"}}}};

    EXPECT_THROW(inventoryModule->executeTask(requestData), ServerException);
}

TEST_F(InventoryModuleTest, AddStockFailsWhenEmployeeIdIsOutOfRange)
{
    const RequestData requestData{
        .module = "inventory",
        .submodule = "stocks",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, {"2"}},
                    {Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY, {"25"}},
                    {Common::Entities::Inventory::EMPLOYEE_ID_KEY,
                     {"9223372036854775808"}}}};

    EXPECT_CALL(*inventoryRepositoryMock,
                add(::testing::An<const Common::Entities::Inventory &>()))
        .Times(0);

    EXPECT_THROW(inventoryModule->executeTask(requestData), ServerException);
}

TEST_F(InventoryModuleTest, GetSupplierProducts)
{
    const RequestData requestData{.module = "inventory",
                                  .submodule = "supplier-products",
                                  .method = "GET",
                                  .resourceId = "",
                                  .dataset = {}};

    EXPECT_CALL(*supplierProductsRepositoryMock, getAll())
        .WillOnce(Return(std::vector<Common::Entities::SuppliersProductInfo>{
            Common::Entities::SuppliersProductInfo{
                .id = "4", .supplierID = "7", .productTypeId = "2", .code = "SUP-002"}}));

    const auto response = inventoryModule->executeTask(requestData);
    EXPECT_EQ(response.dataset.at(Common::Entities::SuppliersProductInfo::ID_KEY).front(), "4");
    EXPECT_EQ(response.dataset.at(Common::Entities::SuppliersProductInfo::SUPPLIER_ID_KEY).front(),
              "7");
    EXPECT_EQ(
        response.dataset.at(Common::Entities::SuppliersProductInfo::PRODUCT_TYPE_ID_KEY).front(),
        "2");
}

TEST_F(InventoryModuleTest, AddSupplierProduct)
{
    const RequestData requestData{
        .module = "inventory",
        .submodule = "supplier-products",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::SuppliersProductInfo::SUPPLIER_ID_KEY, {"7"}},
                    {Common::Entities::SuppliersProductInfo::PRODUCT_TYPE_ID_KEY, {"2"}},
                    {Common::Entities::SuppliersProductInfo::CODE_KEY, {"SUP-002"}}}};

    EXPECT_CALL(*suppliersRepositoryMock,
                getByField(Common::Entities::Supplier::ID_KEY, "7"))
        .WillOnce(Return(std::vector<Common::Entities::Supplier>{
            Common::Entities::Supplier{.id = "7", .name = "Parts Ltd"}}));
    EXPECT_CALL(*productTypesRepositoryMock,
                getByField(Common::Entities::ProductType::ID_KEY, "2"))
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{
            Common::Entities::ProductType{.id = "2", .code = "PT-002"}}));
    EXPECT_CALL(*supplierProductsRepositoryMock,
                add(Truly([](const Common::Entities::SuppliersProductInfo &mapping) {
                    return mapping.id.empty() && mapping.supplierID == "7" &&
                           mapping.productTypeId == "2" && mapping.code == "SUP-002";
                })));

    EXPECT_NO_THROW(inventoryModule->executeTask(requestData));
}

TEST_F(InventoryModuleTest, AddSupplierProductFailsWhenSupplierMissing)
{
    const RequestData requestData{
        .module = "inventory",
        .submodule = "supplier-products",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::SuppliersProductInfo::SUPPLIER_ID_KEY, {"7"}},
                    {Common::Entities::SuppliersProductInfo::PRODUCT_TYPE_ID_KEY, {"2"}},
                    {Common::Entities::SuppliersProductInfo::CODE_KEY, {"SUP-002"}}}};

    EXPECT_CALL(*suppliersRepositoryMock,
                getByField(Common::Entities::Supplier::ID_KEY, "7"))
        .WillOnce(Return(std::vector<Common::Entities::Supplier>{}));

    EXPECT_THROW(inventoryModule->executeTask(requestData), ServerException);
}

TEST_F(InventoryModuleTest, EditSupplierProductFailsWhenMappingDoesNotExist)
{
    const RequestData requestData{
        .module = "inventory",
        .submodule = "supplier-products",
        .method = "PUT",
        .resourceId = "5",
        .dataset = {{Common::Entities::SuppliersProductInfo::SUPPLIER_ID_KEY, {"7"}},
                    {Common::Entities::SuppliersProductInfo::PRODUCT_TYPE_ID_KEY, {"2"}},
                    {Common::Entities::SuppliersProductInfo::CODE_KEY, {"SUP-002"}}}};

    EXPECT_CALL(*supplierProductsRepositoryMock,
                getByField(Common::Entities::SuppliersProductInfo::ID_KEY, "5"))
        .WillOnce(Return(std::vector<Common::Entities::SuppliersProductInfo>{}));

    EXPECT_THROW(inventoryModule->executeTask(requestData), ServerException);
}

int main(int argc, char **argv)
{
    SpdlogConfig::init<SpdlogConfig::LogLevel::Off>();
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
