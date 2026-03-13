#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <limits>
#include <memory>
#include <vector>

#include "BusinessLogic/AnalyticsModule.hpp"
#include "BusinessLogic/LogsModule.hpp"
#include "BusinessLogic/ManagementModule.hpp"
#include "BusinessLogic/PurchaseModule.hpp"
#include "BusinessLogic/SalesModule.hpp"
#include "ServerException.hpp"
#include "mocks/RepositoryManagerMock.hpp"
#include "mocks/RepositoryMock.hpp"

#include "common/AnalyticsKeys.hpp"
#include "common/Entities/Contact.hpp"
#include "common/Entities/Employee.hpp"
#include "common/Entities/Inventory.hpp"
#include "common/Entities/Log.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"
#include "common/Entities/SaleOrder.hpp"
#include "common/Entities/SalesOrderRecord.hpp"
#include "common/Entities/Supplier.hpp"
#include "common/Entities/User.hpp"
#include "common/SpdlogConfig.hpp"

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Truly;

class ManagementModuleTest : public ::testing::Test {
protected:
    std::unique_ptr<ManagementModule> module;
    std::shared_ptr<RepositoryManagerMock> repositoryManagerMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Contact>>> contactsRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Supplier>>> suppliersRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Employee>>> employeesRepositoryMock;

    void SetUp() override
    {
        repositoryManagerMock = std::make_shared<RepositoryManagerMock>();
        contactsRepositoryMock =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::Contact>>>();
        suppliersRepositoryMock =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::Supplier>>>();
        employeesRepositoryMock =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::Employee>>>();

        EXPECT_CALL(*repositoryManagerMock, getContactRepository())
            .WillRepeatedly(Return(contactsRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getSupplierRepository())
            .WillRepeatedly(Return(suppliersRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getEmployeeRepository())
            .WillRepeatedly(Return(employeesRepositoryMock));

        module = std::make_unique<ManagementModule>(*repositoryManagerMock);
    }
};

TEST_F(ManagementModuleTest, AddContactDefaultsTypeToCustomer)
{
    const RequestData requestData{
        .module = "management",
        .submodule = "contacts",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::Contact::NAME_KEY, {"Walk-in customer"}},
                    {Common::Entities::Contact::EMAIL_KEY, {"walkin@example.com"}}}};

    EXPECT_CALL(*contactsRepositoryMock,
                add(Truly([](const Common::Entities::Contact &contact) {
                    return contact.id.empty() && contact.name == "Walk-in customer" &&
                           contact.type == "Customer" && contact.email == "walkin@example.com";
                })));

    EXPECT_NO_THROW(module->executeTask(requestData));
}

TEST_F(ManagementModuleTest, EditEmployeeFailsWhenEmployeeDoesNotExist)
{
    const RequestData requestData{
        .module = "management",
        .submodule = "employees",
        .method = "PUT",
        .resourceId = "77",
        .dataset = {{Common::Entities::Employee::NAME_KEY, {"Missing employee"}}}};

    EXPECT_CALL(*employeesRepositoryMock,
                getByField(Common::Entities::Employee::ID_KEY, "77"))
        .WillOnce(Return(std::vector<Common::Entities::Employee>{}));

    EXPECT_THROW(module->executeTask(requestData), ServerException);
}

class PurchaseModuleTest : public ::testing::Test {
protected:
    std::unique_ptr<PurchaseModule> module;
    std::shared_ptr<RepositoryManagerMock> repositoryManagerMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::PurchaseOrder>>> ordersRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::PurchaseOrderRecord>>>
        orderRecordsRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Inventory>>> inventoryRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::User>>> usersRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Supplier>>> suppliersRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Employee>>> employeesRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::ProductType>>>
        productTypesRepositoryMock;

    void SetUp() override
    {
        repositoryManagerMock      = std::make_shared<RepositoryManagerMock>();
        ordersRepositoryMock       =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::PurchaseOrder>>>();
        orderRecordsRepositoryMock =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::PurchaseOrderRecord>>>();
        inventoryRepositoryMock    =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::Inventory>>>();
        usersRepositoryMock        =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::User>>>();
        suppliersRepositoryMock    =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::Supplier>>>();
        employeesRepositoryMock    =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::Employee>>>();
        productTypesRepositoryMock =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::ProductType>>>();

        EXPECT_CALL(*repositoryManagerMock, getPurchaseOrderRepository())
            .WillRepeatedly(Return(ordersRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getPurchaseOrderRecordRepository())
            .WillRepeatedly(Return(orderRecordsRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getInventoryRepository())
            .WillRepeatedly(Return(inventoryRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getUserRepository())
            .WillRepeatedly(Return(usersRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getSupplierRepository())
            .WillRepeatedly(Return(suppliersRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getEmployeeRepository())
            .WillRepeatedly(Return(employeesRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getProductTypeRepository())
            .WillRepeatedly(Return(productTypesRepositoryMock));

        module = std::make_unique<PurchaseModule>(*repositoryManagerMock);
    }
};

TEST_F(PurchaseModuleTest, ReceiveOrderCreatesStockAndMarksOrderReceived)
{
    const RequestData requestData{
        .module = "purchase",
        .submodule = "receipts",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::PurchaseOrder::ID_KEY, {"11"}},
                    {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"3"}}}};

    const auto order = Common::Entities::PurchaseOrder{
        .id = "11", .date = "2026-03-13", .userId = "1", .supplierId = "2", .status = "Pending"};
    const auto record = Common::Entities::PurchaseOrderRecord{
        .id = "12", .orderId = "11", .productTypeId = "5", .quantity = "4", .price = "19.90"};

    EXPECT_CALL(*ordersRepositoryMock,
                getByField(Common::Entities::PurchaseOrder::ID_KEY, "11"))
        .WillOnce(Return(std::vector<Common::Entities::PurchaseOrder>{order}))
        .WillOnce(Return(std::vector<Common::Entities::PurchaseOrder>{order}));
    EXPECT_CALL(*employeesRepositoryMock,
                getByField(Common::Entities::Employee::ID_KEY, "3"))
        .WillOnce(Return(std::vector<Common::Entities::Employee>{
            Common::Entities::Employee{.id = "3", .name = "Receiver"}}));
    EXPECT_CALL(*orderRecordsRepositoryMock,
                getByField(Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, "11"))
        .WillOnce(Return(std::vector<Common::Entities::PurchaseOrderRecord>{record}));
    EXPECT_CALL(*productTypesRepositoryMock,
                getByField(Common::Entities::ProductType::ID_KEY, "5"))
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{
            Common::Entities::ProductType{.id = "5", .code = "PT-5"}}));
    EXPECT_CALL(*inventoryRepositoryMock,
                getByField(Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, "5"))
        .WillOnce(Return(std::vector<Common::Entities::Inventory>{}));
    EXPECT_CALL(*inventoryRepositoryMock,
                add(Truly([](const Common::Entities::Inventory &stock) {
                    return stock.id.empty() && stock.productTypeId == "5" &&
                           stock.quantityAvailable == "4" && stock.employeeId == "3";
                })));
    EXPECT_CALL(*ordersRepositoryMock,
                update(Truly([](const Common::Entities::PurchaseOrder &updatedOrder) {
                    return updatedOrder.id == "11" && updatedOrder.status == "Received";
                })));

    EXPECT_NO_THROW(module->executeTask(requestData));
}

TEST_F(PurchaseModuleTest, ReceiveOrderRejectsStockQuantityOverflow)
{
    const RequestData requestData{
        .module = "purchase",
        .submodule = "receipts",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::PurchaseOrder::ID_KEY, {"11"}},
                    {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"3"}}}};

    const auto order = Common::Entities::PurchaseOrder{
        .id = "11", .date = "2026-03-13", .userId = "1", .supplierId = "2", .status = "Pending"};
    const auto record = Common::Entities::PurchaseOrderRecord{
        .id = "12", .orderId = "11", .productTypeId = "5", .quantity = "1", .price = "19.90"};
    const auto existingStock = Common::Entities::Inventory{
        .id = "6",
        .productTypeId = "5",
        .quantityAvailable = std::to_string(std::numeric_limits<long long>::max()),
        .employeeId = "1"};

    EXPECT_CALL(*ordersRepositoryMock,
                getByField(Common::Entities::PurchaseOrder::ID_KEY, "11"))
        .WillOnce(Return(std::vector<Common::Entities::PurchaseOrder>{order}))
        .WillOnce(Return(std::vector<Common::Entities::PurchaseOrder>{order}));
    EXPECT_CALL(*employeesRepositoryMock,
                getByField(Common::Entities::Employee::ID_KEY, "3"))
        .WillOnce(Return(std::vector<Common::Entities::Employee>{
            Common::Entities::Employee{.id = "3", .name = "Receiver"}}));
    EXPECT_CALL(*orderRecordsRepositoryMock,
                getByField(Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, "11"))
        .WillOnce(Return(std::vector<Common::Entities::PurchaseOrderRecord>{record}));
    EXPECT_CALL(*productTypesRepositoryMock,
                getByField(Common::Entities::ProductType::ID_KEY, "5"))
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{
            Common::Entities::ProductType{.id = "5", .code = "PT-5"}}));
    EXPECT_CALL(*inventoryRepositoryMock,
                getByField(Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, "5"))
        .WillOnce(Return(std::vector<Common::Entities::Inventory>{existingStock}));
    EXPECT_CALL(*inventoryRepositoryMock,
                update(::testing::An<const Common::Entities::Inventory &>()))
        .Times(0);
    EXPECT_CALL(*ordersRepositoryMock,
                update(::testing::An<const Common::Entities::PurchaseOrder &>()))
        .Times(0);

    EXPECT_THROW(module->executeTask(requestData), ServerException);
}

TEST_F(PurchaseModuleTest, ReceiveOrderStaysAtomicWhenLaterRecordOverflows)
{
    const RequestData requestData{
        .module = "purchase",
        .submodule = "receipts",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::PurchaseOrder::ID_KEY, {"11"}},
                    {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"3"}}}};

    const auto order = Common::Entities::PurchaseOrder{
        .id = "11", .date = "2026-03-13", .userId = "1", .supplierId = "2", .status = "Pending"};
    const auto firstRecord = Common::Entities::PurchaseOrderRecord{
        .id = "12", .orderId = "11", .productTypeId = "5", .quantity = "4", .price = "19.90"};
    const auto secondRecord = Common::Entities::PurchaseOrderRecord{
        .id = "13", .orderId = "11", .productTypeId = "6", .quantity = "1", .price = "5.10"};
    const auto overflowingStock = Common::Entities::Inventory{
        .id = "8",
        .productTypeId = "6",
        .quantityAvailable = std::to_string(std::numeric_limits<long long>::max()),
        .employeeId = "1"};

    EXPECT_CALL(*ordersRepositoryMock,
                getByField(Common::Entities::PurchaseOrder::ID_KEY, "11"))
        .WillOnce(Return(std::vector<Common::Entities::PurchaseOrder>{order}))
        .WillOnce(Return(std::vector<Common::Entities::PurchaseOrder>{order}));
    EXPECT_CALL(*employeesRepositoryMock,
                getByField(Common::Entities::Employee::ID_KEY, "3"))
        .WillOnce(Return(std::vector<Common::Entities::Employee>{
            Common::Entities::Employee{.id = "3", .name = "Receiver"}}));
    EXPECT_CALL(*orderRecordsRepositoryMock,
                getByField(Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, "11"))
        .WillOnce(Return(std::vector<Common::Entities::PurchaseOrderRecord>{firstRecord, secondRecord}));
    EXPECT_CALL(*productTypesRepositoryMock,
                getByField(Common::Entities::ProductType::ID_KEY, "5"))
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{
            Common::Entities::ProductType{.id = "5", .code = "PT-5"}}));
    EXPECT_CALL(*productTypesRepositoryMock,
                getByField(Common::Entities::ProductType::ID_KEY, "6"))
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{
            Common::Entities::ProductType{.id = "6", .code = "PT-6"}}));
    EXPECT_CALL(*inventoryRepositoryMock,
                getByField(Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, "5"))
        .WillOnce(Return(std::vector<Common::Entities::Inventory>{}));
    EXPECT_CALL(*inventoryRepositoryMock,
                getByField(Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, "6"))
        .WillOnce(Return(std::vector<Common::Entities::Inventory>{overflowingStock}));
    EXPECT_CALL(*inventoryRepositoryMock,
                add(::testing::An<const Common::Entities::Inventory &>()))
        .Times(0);
    EXPECT_CALL(*inventoryRepositoryMock,
                update(::testing::An<const Common::Entities::Inventory &>()))
        .Times(0);
    EXPECT_CALL(*ordersRepositoryMock,
                update(::testing::An<const Common::Entities::PurchaseOrder &>()))
        .Times(0);

    EXPECT_THROW(module->executeTask(requestData), ServerException);
}

TEST_F(PurchaseModuleTest, ReceiveOrderRejectsLocalizedReceivedStatus)
{
    const RequestData requestData{
        .module = "purchase",
        .submodule = "receipts",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::PurchaseOrder::ID_KEY, {"11"}},
                    {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"3"}}}};

    const auto order = Common::Entities::PurchaseOrder{
        .id = "11", .date = "2026-03-13", .userId = "1", .supplierId = "2", .status = "Отримано"};

    EXPECT_CALL(*ordersRepositoryMock,
                getByField(Common::Entities::PurchaseOrder::ID_KEY, "11"))
        .WillOnce(Return(std::vector<Common::Entities::PurchaseOrder>{order}))
        .WillOnce(Return(std::vector<Common::Entities::PurchaseOrder>{order}));
    EXPECT_CALL(*employeesRepositoryMock,
                getByField(Common::Entities::Employee::ID_KEY, "3"))
        .WillOnce(Return(std::vector<Common::Entities::Employee>{
            Common::Entities::Employee{.id = "3", .name = "Receiver"}}));
    EXPECT_CALL(*orderRecordsRepositoryMock,
                getByField(Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, "11"))
        .Times(0);
    EXPECT_CALL(*inventoryRepositoryMock,
                update(::testing::An<const Common::Entities::Inventory &>()))
        .Times(0);
    EXPECT_CALL(*ordersRepositoryMock,
                update(::testing::An<const Common::Entities::PurchaseOrder &>()))
        .Times(0);

    EXPECT_THROW(module->executeTask(requestData), ServerException);
}

TEST_F(PurchaseModuleTest, AddOrderRejectsUnknownSupplier)
{
    const RequestData requestData{
        .module = "purchase",
        .submodule = "orders",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::PurchaseOrder::DATE_KEY, {"2026-03-13"}},
                    {Common::Entities::PurchaseOrder::USER_ID_KEY, {"1"}},
                    {Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY, {"99"}},
                    {Common::Entities::PurchaseOrder::STATUS_KEY, {"Draft"}}}};

    EXPECT_CALL(*usersRepositoryMock, getByField(Common::Entities::User::ID_KEY, "1"))
        .WillOnce(Return(std::vector<Common::Entities::User>{
            Common::Entities::User{.id = "1", .username = "admin"}}));
    EXPECT_CALL(*suppliersRepositoryMock,
                getByField(Common::Entities::Supplier::ID_KEY, "99"))
        .WillOnce(Return(std::vector<Common::Entities::Supplier>{}));

    EXPECT_THROW(module->executeTask(requestData), ServerException);
}

TEST_F(PurchaseModuleTest, AddOrderNormalizesLocalizedStatusToCanonical)
{
    const RequestData requestData{
        .module = "purchase",
        .submodule = "orders",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::PurchaseOrder::DATE_KEY, {"2026-03-13"}},
                    {Common::Entities::PurchaseOrder::USER_ID_KEY, {"1"}},
                    {Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY, {"2"}},
                    {Common::Entities::PurchaseOrder::STATUS_KEY, {"Замовлено"}}}};

    EXPECT_CALL(*usersRepositoryMock, getByField(Common::Entities::User::ID_KEY, "1"))
        .WillOnce(Return(std::vector<Common::Entities::User>{
            Common::Entities::User{.id = "1", .username = "admin"}}));
    EXPECT_CALL(*suppliersRepositoryMock,
                getByField(Common::Entities::Supplier::ID_KEY, "2"))
        .WillOnce(Return(std::vector<Common::Entities::Supplier>{
            Common::Entities::Supplier{.id = "2", .name = "Supplier"}}));
    EXPECT_CALL(*ordersRepositoryMock,
                add(Truly([](const Common::Entities::PurchaseOrder &order) {
                    return order.status == "Ordered" && order.supplierId == "2";
                })));

    EXPECT_NO_THROW(module->executeTask(requestData));
}

TEST_F(PurchaseModuleTest, AddOrderNormalizesLegacyPendingStatusToCanonical)
{
    const RequestData requestData{
        .module = "purchase",
        .submodule = "orders",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::PurchaseOrder::DATE_KEY, {"2026-03-13"}},
                    {Common::Entities::PurchaseOrder::USER_ID_KEY, {"1"}},
                    {Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY, {"2"}},
                    {Common::Entities::PurchaseOrder::STATUS_KEY, {"Pending"}}}};

    EXPECT_CALL(*usersRepositoryMock, getByField(Common::Entities::User::ID_KEY, "1"))
        .WillOnce(Return(std::vector<Common::Entities::User>{
            Common::Entities::User{.id = "1", .username = "admin"}}));
    EXPECT_CALL(*suppliersRepositoryMock,
                getByField(Common::Entities::Supplier::ID_KEY, "2"))
        .WillOnce(Return(std::vector<Common::Entities::Supplier>{
            Common::Entities::Supplier{.id = "2", .name = "Supplier"}}));
    EXPECT_CALL(*ordersRepositoryMock,
                add(Truly([](const Common::Entities::PurchaseOrder &order) {
                    return order.status == "Ordered" && order.supplierId == "2";
                })));

    EXPECT_NO_THROW(module->executeTask(requestData));
}

TEST_F(PurchaseModuleTest, AddOrderRejectsReceivedStatus)
{
    const RequestData requestData{
        .module = "purchase",
        .submodule = "orders",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::PurchaseOrder::DATE_KEY, {"2026-03-13"}},
                    {Common::Entities::PurchaseOrder::USER_ID_KEY, {"1"}},
                    {Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY, {"2"}},
                    {Common::Entities::PurchaseOrder::STATUS_KEY, {"Received"}}}};

    EXPECT_CALL(*usersRepositoryMock, getByField(::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(*suppliersRepositoryMock, getByField(::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(*ordersRepositoryMock, add(::testing::An<const Common::Entities::PurchaseOrder &>()))
        .Times(0);

    EXPECT_THROW(module->executeTask(requestData), ServerException);
}

TEST_F(PurchaseModuleTest, EditOrderRejectsDirectReceivedTransition)
{
    const RequestData requestData{
        .module = "purchase",
        .submodule = "orders",
        .method = "PUT",
        .resourceId = "11",
        .dataset = {{Common::Entities::PurchaseOrder::DATE_KEY, {"2026-03-13"}},
                    {Common::Entities::PurchaseOrder::USER_ID_KEY, {"1"}},
                    {Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY, {"2"}},
                    {Common::Entities::PurchaseOrder::STATUS_KEY, {"Received"}}}};

    const auto existingOrder = Common::Entities::PurchaseOrder{
        .id = "11", .date = "2026-03-13", .userId = "1", .supplierId = "2", .status = "Ordered"};

    EXPECT_CALL(*ordersRepositoryMock,
                getByField(Common::Entities::PurchaseOrder::ID_KEY, "11"))
        .Times(2)
        .WillRepeatedly(Return(std::vector<Common::Entities::PurchaseOrder>{existingOrder}));
    EXPECT_CALL(*usersRepositoryMock, getByField(::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(*suppliersRepositoryMock, getByField(::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(*ordersRepositoryMock,
                update(::testing::An<const Common::Entities::PurchaseOrder &>()))
        .Times(0);

    EXPECT_THROW(module->executeTask(requestData), ServerException);
}

TEST_F(PurchaseModuleTest, EditOrderRejectsChangesAfterReceipt)
{
    const RequestData requestData{
        .module = "purchase",
        .submodule = "orders",
        .method = "PUT",
        .resourceId = "11",
        .dataset = {{Common::Entities::PurchaseOrder::DATE_KEY, {"2026-03-14"}},
                    {Common::Entities::PurchaseOrder::USER_ID_KEY, {"1"}},
                    {Common::Entities::PurchaseOrder::SUPPLIER_ID_KEY, {"2"}},
                    {Common::Entities::PurchaseOrder::STATUS_KEY, {"Ordered"}}}};

    const auto receivedOrder = Common::Entities::PurchaseOrder{
        .id = "11", .date = "2026-03-13", .userId = "1", .supplierId = "2", .status = "Received"};

    EXPECT_CALL(*ordersRepositoryMock,
                getByField(Common::Entities::PurchaseOrder::ID_KEY, "11"))
        .Times(2)
        .WillRepeatedly(Return(std::vector<Common::Entities::PurchaseOrder>{receivedOrder}));
    EXPECT_CALL(*ordersRepositoryMock,
                update(::testing::An<const Common::Entities::PurchaseOrder &>()))
        .Times(0);

    EXPECT_THROW(module->executeTask(requestData), ServerException);
}

TEST_F(PurchaseModuleTest, DeleteOrderRejectsReceivedOrder)
{
    const RequestData requestData{
        .module = "purchase",
        .submodule = "orders",
        .method = "DELETE",
        .resourceId = "11",
        .dataset = {}};

    const auto receivedOrder = Common::Entities::PurchaseOrder{
        .id = "11", .date = "2026-03-13", .userId = "1", .supplierId = "2", .status = "Received"};

    EXPECT_CALL(*ordersRepositoryMock,
                getByField(Common::Entities::PurchaseOrder::ID_KEY, "11"))
        .Times(2)
        .WillRepeatedly(Return(std::vector<Common::Entities::PurchaseOrder>{receivedOrder}));
    EXPECT_CALL(*orderRecordsRepositoryMock, getByField(::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(*ordersRepositoryMock, deleteResource("11")).Times(0);

    EXPECT_THROW(module->executeTask(requestData), ServerException);
}

TEST_F(PurchaseModuleTest, AddOrderRecordRejectsReceivedParentOrder)
{
    const RequestData requestData{
        .module = "purchase",
        .submodule = "order-records",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, {"11"}},
                    {Common::Entities::PurchaseOrderRecord::PRODUCT_TYPE_ID_KEY, {"5"}},
                    {Common::Entities::PurchaseOrderRecord::QUANTITY_KEY, {"4"}},
                    {Common::Entities::PurchaseOrderRecord::PRICE_KEY, {"19.90"}}}};

    const auto receivedOrder = Common::Entities::PurchaseOrder{
        .id = "11", .date = "2026-03-13", .userId = "1", .supplierId = "2", .status = "Received"};

    EXPECT_CALL(*ordersRepositoryMock,
                getByField(Common::Entities::PurchaseOrder::ID_KEY, "11"))
        .Times(2)
        .WillRepeatedly(Return(std::vector<Common::Entities::PurchaseOrder>{receivedOrder}));
    EXPECT_CALL(*productTypesRepositoryMock, getByField(::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(*orderRecordsRepositoryMock,
                add(::testing::An<const Common::Entities::PurchaseOrderRecord &>()))
        .Times(0);

    EXPECT_THROW(module->executeTask(requestData), ServerException);
}

TEST_F(PurchaseModuleTest, EditOrderRecordRejectsReceivedParentOrder)
{
    const RequestData requestData{
        .module = "purchase",
        .submodule = "order-records",
        .method = "PUT",
        .resourceId = "15",
        .dataset = {{Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, {"11"}},
                    {Common::Entities::PurchaseOrderRecord::PRODUCT_TYPE_ID_KEY, {"5"}},
                    {Common::Entities::PurchaseOrderRecord::QUANTITY_KEY, {"6"}},
                    {Common::Entities::PurchaseOrderRecord::PRICE_KEY, {"21.00"}}}};

    const auto existingRecord = Common::Entities::PurchaseOrderRecord{
        .id = "15", .orderId = "11", .productTypeId = "5", .quantity = "4", .price = "19.90"};
    const auto receivedOrder = Common::Entities::PurchaseOrder{
        .id = "11", .date = "2026-03-13", .userId = "1", .supplierId = "2", .status = "Received"};

    EXPECT_CALL(*orderRecordsRepositoryMock,
                getByField(Common::Entities::PurchaseOrderRecord::ID_KEY, "15"))
        .Times(2)
        .WillRepeatedly(
            Return(std::vector<Common::Entities::PurchaseOrderRecord>{existingRecord}));
    EXPECT_CALL(*ordersRepositoryMock,
                getByField(Common::Entities::PurchaseOrder::ID_KEY, "11"))
        .Times(2)
        .WillRepeatedly(Return(std::vector<Common::Entities::PurchaseOrder>{receivedOrder}));
    EXPECT_CALL(*productTypesRepositoryMock, getByField(::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(*orderRecordsRepositoryMock,
                update(::testing::An<const Common::Entities::PurchaseOrderRecord &>()))
        .Times(0);

    EXPECT_THROW(module->executeTask(requestData), ServerException);
}

TEST_F(PurchaseModuleTest, DeleteOrderRecordRejectsReceivedParentOrder)
{
    const RequestData requestData{
        .module = "purchase",
        .submodule = "order-records",
        .method = "DELETE",
        .resourceId = "15",
        .dataset = {}};

    const auto existingRecord = Common::Entities::PurchaseOrderRecord{
        .id = "15", .orderId = "11", .productTypeId = "5", .quantity = "4", .price = "19.90"};
    const auto receivedOrder = Common::Entities::PurchaseOrder{
        .id = "11", .date = "2026-03-13", .userId = "1", .supplierId = "2", .status = "Received"};

    EXPECT_CALL(*orderRecordsRepositoryMock,
                getByField(Common::Entities::PurchaseOrderRecord::ID_KEY, "15"))
        .Times(2)
        .WillRepeatedly(
            Return(std::vector<Common::Entities::PurchaseOrderRecord>{existingRecord}));
    EXPECT_CALL(*ordersRepositoryMock,
                getByField(Common::Entities::PurchaseOrder::ID_KEY, "11"))
        .Times(2)
        .WillRepeatedly(Return(std::vector<Common::Entities::PurchaseOrder>{receivedOrder}));
    EXPECT_CALL(*orderRecordsRepositoryMock, deleteResource("15")).Times(0);

    EXPECT_THROW(module->executeTask(requestData), ServerException);
}

TEST_F(PurchaseModuleTest, AddOrderRecordRejectsOutOfRangeQuantity)
{
    const RequestData requestData{
        .module = "purchase",
        .submodule = "order-records",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::PurchaseOrderRecord::ORDER_ID_KEY, {"11"}},
                    {Common::Entities::PurchaseOrderRecord::PRODUCT_TYPE_ID_KEY, {"5"}},
                    {Common::Entities::PurchaseOrderRecord::QUANTITY_KEY, {"9223372036854775808"}},
                    {Common::Entities::PurchaseOrderRecord::PRICE_KEY, {"19.90"}}}};

    EXPECT_CALL(*orderRecordsRepositoryMock,
                add(::testing::An<const Common::Entities::PurchaseOrderRecord &>()))
        .Times(0);

    EXPECT_THROW(module->executeTask(requestData), ServerException);
}

class SalesModuleTest : public ::testing::Test {
protected:
    std::unique_ptr<SalesModule> module;
    std::shared_ptr<RepositoryManagerMock> repositoryManagerMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::SaleOrder>>> ordersRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::SalesOrderRecord>>>
        orderRecordsRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::User>>> usersRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Contact>>> contactsRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Employee>>> employeesRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::ProductType>>>
        productTypesRepositoryMock;

    void SetUp() override
    {
        repositoryManagerMock      = std::make_shared<RepositoryManagerMock>();
        ordersRepositoryMock       =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::SaleOrder>>>();
        orderRecordsRepositoryMock =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::SalesOrderRecord>>>();
        usersRepositoryMock        =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::User>>>();
        contactsRepositoryMock     =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::Contact>>>();
        employeesRepositoryMock    =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::Employee>>>();
        productTypesRepositoryMock =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::ProductType>>>();

        EXPECT_CALL(*repositoryManagerMock, getSaleOrderRepository())
            .WillRepeatedly(Return(ordersRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getSalesOrderRecordRepository())
            .WillRepeatedly(Return(orderRecordsRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getUserRepository())
            .WillRepeatedly(Return(usersRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getContactRepository())
            .WillRepeatedly(Return(contactsRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getEmployeeRepository())
            .WillRepeatedly(Return(employeesRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getProductTypeRepository())
            .WillRepeatedly(Return(productTypesRepositoryMock));

        module = std::make_unique<SalesModule>(*repositoryManagerMock);
    }
};

TEST_F(SalesModuleTest, AddOrderRejectsUnknownContact)
{
    const RequestData requestData{
        .module = "sales",
        .submodule = "orders",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::SaleOrder::DATE_KEY, {"2026-03-13"}},
                    {Common::Entities::SaleOrder::USER_ID_KEY, {"1"}},
                    {Common::Entities::SaleOrder::CONTACT_ID_KEY, {"91"}},
                    {Common::Entities::SaleOrder::EMPLOYEE_ID_KEY, {"2"}},
                    {Common::Entities::SaleOrder::STATUS_KEY, {"Draft"}}}};

    EXPECT_CALL(*usersRepositoryMock, getByField(Common::Entities::User::ID_KEY, "1"))
        .WillOnce(Return(std::vector<Common::Entities::User>{
            Common::Entities::User{.id = "1", .username = "admin"}}));
    EXPECT_CALL(*contactsRepositoryMock,
                getByField(Common::Entities::Contact::ID_KEY, "91"))
        .WillOnce(Return(std::vector<Common::Entities::Contact>{}));

    EXPECT_THROW(module->executeTask(requestData), ServerException);
}

class LogsModuleTest : public ::testing::Test {
protected:
    std::unique_ptr<LogsModule> module;
    std::shared_ptr<RepositoryManagerMock> repositoryManagerMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Log>>> logsRepositoryMock;

    void SetUp() override
    {
        repositoryManagerMock = std::make_shared<RepositoryManagerMock>();
        logsRepositoryMock    = std::make_shared<NiceMock<RepositoryMock<Common::Entities::Log>>>();

        EXPECT_CALL(*repositoryManagerMock, getLogRepository())
            .WillRepeatedly(Return(logsRepositoryMock));

        module = std::make_unique<LogsModule>(*repositoryManagerMock);
    }
};

TEST_F(LogsModuleTest, AddLogRejectsNonNumericUserId)
{
    const RequestData requestData{
        .module = "logs",
        .submodule = "entries",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::Log::USER_ID_KEY, {"admin"}},
                    {Common::Entities::Log::ACTION_KEY, {"manual entry"}},
                    {Common::Entities::Log::TIMESTAMP_KEY, {"2026-03-13 12:00:00"}}}};

    EXPECT_THROW(module->executeTask(requestData), ServerException);
}

TEST_F(LogsModuleTest, AddLogRejectsOutOfRangeUserId)
{
    const RequestData requestData{
        .module = "logs",
        .submodule = "entries",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::Log::USER_ID_KEY, {"9223372036854775808"}},
                    {Common::Entities::Log::ACTION_KEY, {"oversized"}},
                    {Common::Entities::Log::TIMESTAMP_KEY, {"2026-03-13 12:00:00"}}}};

    EXPECT_CALL(*logsRepositoryMock, add(::testing::An<const Common::Entities::Log &>()))
        .Times(0);

    EXPECT_THROW(module->executeTask(requestData), ServerException);
}

class AnalyticsModuleTest : public ::testing::Test {
protected:
    std::unique_ptr<AnalyticsModule> module;
    std::shared_ptr<RepositoryManagerMock> repositoryManagerMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::SaleOrder>>> salesOrdersRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::SalesOrderRecord>>>
        salesOrderRecordsRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Inventory>>> inventoryRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::ProductType>>>
        productTypesRepositoryMock;

    void SetUp() override
    {
        repositoryManagerMock         = std::make_shared<RepositoryManagerMock>();
        salesOrdersRepositoryMock     =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::SaleOrder>>>();
        salesOrderRecordsRepositoryMock =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::SalesOrderRecord>>>();
        inventoryRepositoryMock       =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::Inventory>>>();
        productTypesRepositoryMock    =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::ProductType>>>();

        EXPECT_CALL(*repositoryManagerMock, getSaleOrderRepository())
            .WillRepeatedly(Return(salesOrdersRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getSalesOrderRecordRepository())
            .WillRepeatedly(Return(salesOrderRecordsRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getInventoryRepository())
            .WillRepeatedly(Return(inventoryRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getProductTypeRepository())
            .WillRepeatedly(Return(productTypesRepositoryMock));

        module = std::make_unique<AnalyticsModule>(*repositoryManagerMock);
    }
};

TEST_F(AnalyticsModuleTest, GetSalesAnalyticsAggregatesMetrics)
{
    const RequestData requestData{
        .module = "analytics", .submodule = "sales", .method = "GET", .resourceId = "", .dataset = {}};

    EXPECT_CALL(*salesOrdersRepositoryMock, getAll())
        .WillOnce(Return(std::vector<Common::Entities::SaleOrder>{
            Common::Entities::SaleOrder{
                .id = "1", .date = "2026-03-13", .userId = "1", .contactId = "10",
                .employeeId = "2", .status = "Issued"},
            Common::Entities::SaleOrder{
                .id = "2", .date = "2026-03-13", .userId = "1", .contactId = "11",
                .employeeId = "2", .status = "Issued"}}));
    EXPECT_CALL(*salesOrderRecordsRepositoryMock, getAll())
        .WillOnce(Return(std::vector<Common::Entities::SalesOrderRecord>{
            Common::Entities::SalesOrderRecord{
                .id = "1", .orderId = "1", .productTypeId = "5", .quantity = "2", .price = "10.5"},
            Common::Entities::SalesOrderRecord{
                .id = "2", .orderId = "2", .productTypeId = "6", .quantity = "1", .price = "4.0"}}));

    const auto response = module->executeTask(requestData);
    ASSERT_TRUE(response.dataset.contains(AnalyticsKeys::Sales::TOTAL_ORDERS));
    EXPECT_EQ(response.dataset.at(AnalyticsKeys::Sales::TOTAL_ORDERS).front(), "2");
    EXPECT_EQ(response.dataset.at(AnalyticsKeys::Sales::TOTAL_ORDER_LINES).front(), "2");
    EXPECT_EQ(response.dataset.at(AnalyticsKeys::Sales::UNIQUE_CUSTOMERS).front(), "2");
    EXPECT_DOUBLE_EQ(std::stod(response.dataset.at(AnalyticsKeys::Sales::TOTAL_REVENUE).front()),
                     25.0);
    EXPECT_DOUBLE_EQ(
        std::stod(response.dataset.at(AnalyticsKeys::Sales::AVERAGE_ORDER_VALUE).front()), 12.5);
}

int main(int argc, char **argv)
{
    SpdlogConfig::init<SpdlogConfig::LogLevel::Off>();
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
