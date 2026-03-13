#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "BusinessLogic/BusinessLogic.hpp"
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
#include "common/Entities/Role.hpp"
#include "common/Entities/SaleOrder.hpp"
#include "common/Entities/SalesOrderRecord.hpp"
#include "common/Entities/Supplier.hpp"
#include "common/Entities/SuppliersProductInfo.hpp"
#include "common/Entities/User.hpp"
#include "common/Keys.hpp"
#include "common/SpdlogConfig.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Truly;

class BusinessLogicTest : public ::testing::Test {
protected:
    std::unique_ptr<BusinessLogic> businessLogic;

    std::shared_ptr<RepositoryManagerMock> repositoryManagerMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::User>>> usersRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Role>>> rolesRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Inventory>>> inventoryRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Contact>>> contactsRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Employee>>> employeesRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Log>>> logsRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::PurchaseOrder>>>
        purchaseOrdersRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::PurchaseOrderRecord>>>
        purchaseOrderRecordsRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::ProductType>>>
        productTypesRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::SaleOrder>>> salesOrdersRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::SalesOrderRecord>>>
        salesOrderRecordsRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::Supplier>>> suppliersRepositoryMock;
    std::shared_ptr<NiceMock<RepositoryMock<Common::Entities::SuppliersProductInfo>>>
        supplierProductsRepositoryMock;

    void SetUp() override
    {
        repositoryManagerMock           = std::make_shared<RepositoryManagerMock>();
        usersRepositoryMock             =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::User>>>();
        rolesRepositoryMock             =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::Role>>>();
        inventoryRepositoryMock         =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::Inventory>>>();
        contactsRepositoryMock          =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::Contact>>>();
        employeesRepositoryMock         =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::Employee>>>();
        logsRepositoryMock              =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::Log>>>();
        purchaseOrdersRepositoryMock    =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::PurchaseOrder>>>();
        purchaseOrderRecordsRepositoryMock =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::PurchaseOrderRecord>>>();
        productTypesRepositoryMock      =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::ProductType>>>();
        salesOrdersRepositoryMock       =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::SaleOrder>>>();
        salesOrderRecordsRepositoryMock =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::SalesOrderRecord>>>();
        suppliersRepositoryMock         =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::Supplier>>>();
        supplierProductsRepositoryMock  =
            std::make_shared<NiceMock<RepositoryMock<Common::Entities::SuppliersProductInfo>>>();

        EXPECT_CALL(*repositoryManagerMock, getUserRepository())
            .WillRepeatedly(Return(usersRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getRoleRepository())
            .WillRepeatedly(Return(rolesRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getInventoryRepository())
            .WillRepeatedly(Return(inventoryRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getContactRepository())
            .WillRepeatedly(Return(contactsRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getEmployeeRepository())
            .WillRepeatedly(Return(employeesRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getLogRepository())
            .WillRepeatedly(Return(logsRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getPurchaseOrderRepository())
            .WillRepeatedly(Return(purchaseOrdersRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getPurchaseOrderRecordRepository())
            .WillRepeatedly(Return(purchaseOrderRecordsRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getProductTypeRepository())
            .WillRepeatedly(Return(productTypesRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getSaleOrderRepository())
            .WillRepeatedly(Return(salesOrdersRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getSalesOrderRecordRepository())
            .WillRepeatedly(Return(salesOrderRecordsRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getSupplierRepository())
            .WillRepeatedly(Return(suppliersRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getSuppliersProductInfoRepository())
            .WillRepeatedly(Return(supplierProductsRepositoryMock));

        businessLogic = std::make_unique<BusinessLogic>(*repositoryManagerMock);
    }

    ResponseData executeTask(const RequestData &requestData)
    {
        ResponseData responseData;
        bool callbackInvoked = false;

        businessLogic->executeTask(requestData, [&](ResponseData response) {
            callbackInvoked = true;
            responseData    = std::move(response);
        });

        EXPECT_TRUE(callbackInvoked);
        return responseData;
    }
};

TEST_F(BusinessLogicTest, UsersModuleLoginUser)
{
    constexpr char CORRECT_USERNAME[] = "username1";
    constexpr char CORRECT_PASSWORD[] = "password1";

    const RequestData requestData{
        .module = "users",
        .submodule = "login",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::User::USERNAME_KEY, {CORRECT_USERNAME}},
                    {Common::Entities::User::PASSWORD_KEY, {CORRECT_PASSWORD}}}};

    EXPECT_CALL(*usersRepositoryMock, getByField(_, _))
        .WillOnce(Return(std::vector<Common::Entities::User>{
            Common::Entities::User{
                .id = "1",
                .username = CORRECT_USERNAME,
                .password = CORRECT_PASSWORD,
                .roleId = "1"}}));
    EXPECT_CALL(*logsRepositoryMock, add(Truly([](const Common::Entities::Log &entry) {
                    return entry.action == "users/login POST" && entry.userId.empty() &&
                           !entry.timestamp.empty();
                })));

    const auto response = executeTask(requestData);
    ASSERT_FALSE(response.dataset.contains(Keys::_ERROR));
}

TEST_F(BusinessLogicTest, UsersModuleGetRoles)
{
    const RequestData requestData{
        .module = "users", .submodule = "roles", .method = "GET", .resourceId = "", .dataset = {}};

    EXPECT_CALL(*rolesRepositoryMock, getAll())
        .WillOnce(Return(std::vector<Common::Entities::Role>{
            Common::Entities::Role{.id = "2", .name = "admin"}}));
    EXPECT_CALL(*logsRepositoryMock, add(_)).Times(0);

    const auto response = executeTask(requestData);
    ASSERT_FALSE(response.dataset.contains(Keys::_ERROR));
    ASSERT_TRUE(response.dataset.contains(Common::Entities::Role::NAME_KEY));
}

TEST_F(BusinessLogicTest, ManagementModuleGetContacts)
{
    const RequestData requestData{
        .module = "management",
        .submodule = "contacts",
        .method = "GET",
        .resourceId = "",
        .dataset = {}};

    EXPECT_CALL(*contactsRepositoryMock, getAll())
        .WillOnce(Return(std::vector<Common::Entities::Contact>{
            Common::Entities::Contact{
                .id = "4", .name = "Retail customer", .type = "Customer", .email = "a@b.c"}}));
    EXPECT_CALL(*logsRepositoryMock, add(_)).Times(0);

    const auto response = executeTask(requestData);
    ASSERT_FALSE(response.dataset.contains(Keys::_ERROR));
    EXPECT_EQ(response.dataset.at(Common::Entities::Contact::NAME_KEY).front(), "Retail customer");
}

TEST_F(BusinessLogicTest, PurchaseModuleGetOrders)
{
    const RequestData requestData{
        .module = "purchase", .submodule = "orders", .method = "GET", .resourceId = "", .dataset = {}};

    EXPECT_CALL(*purchaseOrdersRepositoryMock, getAll())
        .WillOnce(Return(std::vector<Common::Entities::PurchaseOrder>{
            Common::Entities::PurchaseOrder{
                .id = "7", .date = "2026-03-13", .userId = "1", .supplierId = "2", .status = "Draft"}}));
    EXPECT_CALL(*logsRepositoryMock, add(_)).Times(0);

    const auto response = executeTask(requestData);
    ASSERT_FALSE(response.dataset.contains(Keys::_ERROR));
    EXPECT_EQ(response.dataset.at(Common::Entities::PurchaseOrder::STATUS_KEY).front(), "Draft");
}

TEST_F(BusinessLogicTest, SalesModuleGetOrders)
{
    const RequestData requestData{
        .module = "sales", .submodule = "orders", .method = "GET", .resourceId = "", .dataset = {}};

    EXPECT_CALL(*salesOrdersRepositoryMock, getAll())
        .WillOnce(Return(std::vector<Common::Entities::SaleOrder>{
            Common::Entities::SaleOrder{.id = "8",
                                        .date = "2026-03-13",
                                        .userId = "1",
                                        .contactId = "5",
                                        .employeeId = "3",
                                        .status = "Issued"}}));
    EXPECT_CALL(*logsRepositoryMock, add(_)).Times(0);

    const auto response = executeTask(requestData);
    ASSERT_FALSE(response.dataset.contains(Keys::_ERROR));
    EXPECT_EQ(response.dataset.at(Common::Entities::SaleOrder::CONTACT_ID_KEY).front(), "5");
}

TEST_F(BusinessLogicTest, AnalyticsModuleGetSalesMetrics)
{
    const RequestData requestData{
        .module = "analytics", .submodule = "sales", .method = "GET", .resourceId = "", .dataset = {}};

    EXPECT_CALL(*salesOrdersRepositoryMock, getAll())
        .WillOnce(Return(std::vector<Common::Entities::SaleOrder>{
            Common::Entities::SaleOrder{
                .id = "8", .date = "2026-03-13", .userId = "1", .contactId = "5",
                .employeeId = "3", .status = "Issued"}}));
    EXPECT_CALL(*salesOrderRecordsRepositoryMock, getAll())
        .WillOnce(Return(std::vector<Common::Entities::SalesOrderRecord>{
            Common::Entities::SalesOrderRecord{
                .id = "9", .orderId = "8", .productTypeId = "1", .quantity = "2", .price = "5.5"}}));
    EXPECT_CALL(*logsRepositoryMock, add(_)).Times(0);

    const auto response = executeTask(requestData);
    ASSERT_FALSE(response.dataset.contains(Keys::_ERROR));
    EXPECT_EQ(response.dataset.at(AnalyticsKeys::Sales::TOTAL_ORDERS).front(), "1");
    EXPECT_DOUBLE_EQ(std::stod(response.dataset.at(AnalyticsKeys::Sales::TOTAL_REVENUE).front()),
                     11.0);
}

TEST_F(BusinessLogicTest, LogsModuleGetEntries)
{
    const RequestData requestData{
        .module = "logs", .submodule = "entries", .method = "GET", .resourceId = "", .dataset = {}};

    EXPECT_CALL(*logsRepositoryMock, getAll())
        .WillOnce(Return(std::vector<Common::Entities::Log>{
            Common::Entities::Log{
                .id = "10", .userId = "1", .action = "seed", .timestamp = "2026-03-13 10:00:00"}}));
    EXPECT_CALL(*logsRepositoryMock, add(_)).Times(0);

    const auto response = executeTask(requestData);
    ASSERT_FALSE(response.dataset.contains(Keys::_ERROR));
    EXPECT_EQ(response.dataset.at(Common::Entities::Log::ACTION_KEY).front(), "seed");
}

TEST_F(BusinessLogicTest, MutatingNonLogsRequestWritesAuditLog)
{
    const RequestData requestData{
        .module = "management",
        .submodule = "contacts",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::Contact::NAME_KEY, {"New customer"}},
                    {Common::Entities::Log::USER_ID_KEY, {"7"}}}};

    EXPECT_CALL(*contactsRepositoryMock,
                add(Truly([](const Common::Entities::Contact &contact) {
                    return contact.name == "New customer" && contact.type == "Customer";
                })));
    EXPECT_CALL(*logsRepositoryMock,
                add(Truly([](const Common::Entities::Log &entry) {
                    return entry.userId == "7" && entry.action == "management/contacts POST" &&
                           !entry.timestamp.empty();
                })));

    const auto response = executeTask(requestData);
    ASSERT_FALSE(response.dataset.contains(Keys::_ERROR));
}

TEST_F(BusinessLogicTest, LogsModulePostDoesNotCreateExtraAuditLog)
{
    const RequestData requestData{
        .module = "logs",
        .submodule = "entries",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::Log::USER_ID_KEY, {"1"}},
                    {Common::Entities::Log::ACTION_KEY, {"manual correction"}},
                    {Common::Entities::Log::TIMESTAMP_KEY, {"2026-03-13 12:00:00"}}}};

    EXPECT_CALL(*logsRepositoryMock,
                add(Truly([](const Common::Entities::Log &entry) {
                    return entry.userId == "1" && entry.action == "manual correction" &&
                           entry.timestamp == "2026-03-13 12:00:00";
                })))
        .Times(1);

    const auto response = executeTask(requestData);
    ASSERT_FALSE(response.dataset.contains(Keys::_ERROR));
}

TEST_F(BusinessLogicTest, UnknownModuleReturnsErrorResponse)
{
    const RequestData requestData{
        .module = "billing", .submodule = "list", .method = "GET", .resourceId = "", .dataset = {}};

    const auto response = executeTask(requestData);
    ASSERT_TRUE(response.dataset.contains(Keys::_ERROR));
    EXPECT_EQ(response.dataset.at(Keys::_ERROR).front(), "BusinessLogic");
}

TEST_F(BusinessLogicTest, EmptyModuleReturnsErrorResponse)
{
    const RequestData requestData{
        .module = "", .submodule = "users", .method = "GET", .resourceId = "", .dataset = {}};

    const auto response = executeTask(requestData);
    ASSERT_TRUE(response.dataset.contains(Keys::_ERROR));
    EXPECT_EQ(response.dataset.at(Keys::_ERROR).front(), "BusinessLogic");
}

TEST_F(BusinessLogicTest, ModuleExceptionIsConvertedToErrorDataset)
{
    const RequestData requestData{
        .module = "sales",
        .submodule = "orders",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Common::Entities::SaleOrder::DATE_KEY, {"2026-03-13"}},
                    {Common::Entities::SaleOrder::USER_ID_KEY, {"1"}},
                    {Common::Entities::SaleOrder::CONTACT_ID_KEY, {"99"}},
                    {Common::Entities::SaleOrder::EMPLOYEE_ID_KEY, {"3"}},
                    {Common::Entities::SaleOrder::STATUS_KEY, {"Draft"}}}};

    EXPECT_CALL(*usersRepositoryMock, getByField(Common::Entities::User::ID_KEY, "1"))
        .WillOnce(Return(std::vector<Common::Entities::User>{
            Common::Entities::User{.id = "1", .username = "admin"}}));
    EXPECT_CALL(*contactsRepositoryMock, getByField(Common::Entities::Contact::ID_KEY, "99"))
        .WillOnce(Return(std::vector<Common::Entities::Contact>{}));
    EXPECT_CALL(*logsRepositoryMock, add(_)).Times(0);

    const auto response = executeTask(requestData);

    ASSERT_TRUE(response.dataset.contains(Keys::_ERROR));
    EXPECT_EQ(response.dataset.at(Keys::_ERROR).front(), "SalesModule");
}

int main(int argc, char **argv)
{
    SpdlogConfig::init<SpdlogConfig::LogLevel::Off>();
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
