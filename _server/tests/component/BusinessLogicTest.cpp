#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

#include "BusinessLogic/BusinessLogic.hpp"
#include "mocks/RepositoryManagerMock.hpp"
#include "mocks/RepositoryMock.hpp"

#include "common/Entities/ProductType.hpp"
#include "common/Keys.hpp"
#include "common/SpdlogConfig.hpp"

using ::testing::_;
using ::testing::Return;

class BusinessLogicTest : public ::testing::Test {
protected:
    std::unique_ptr<BusinessLogic> businessLogic;

    std::shared_ptr<RepositoryManagerMock> repositoryManagerMock;
    std::shared_ptr<RepositoryMock<Common::Entities::User>> usersRepositoryMock;
    std::shared_ptr<RepositoryMock<Common::Entities::Role>> rolesRepositoryMock;
    std::shared_ptr<RepositoryMock<Common::Entities::ProductType>> productTypesRepositoryMock;

    BusinessLogicTest()
        : repositoryManagerMock(std::make_shared<RepositoryManagerMock>()),
          usersRepositoryMock(std::make_shared<RepositoryMock<Common::Entities::User>>()),
          rolesRepositoryMock(std::make_shared<RepositoryMock<Common::Entities::Role>>()),
          productTypesRepositoryMock(
              std::make_shared<RepositoryMock<Common::Entities::ProductType>>())
    {
    }

    void SetUp() override
    {
        EXPECT_CALL(*repositoryManagerMock, getUserRepository())
            .WillRepeatedly(Return(usersRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getRoleRepository())
            .WillRepeatedly(Return(rolesRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getProductTypeRepository())
            .WillRepeatedly(Return(productTypesRepositoryMock));

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

TEST_F(BusinessLogicTest, UsersModule_LoginUser)
{
    constexpr char CORRECT_USERNAME[] = "username1";
    constexpr char CORRECT_PASSWORD[] = "password1";

    RequestData requestData{
        .module     = "users",
        .submodule  = "login",
        .method     = "POST",
        .resourceId = "",
        .dataset    = {{Common::Entities::User::USERNAME_KEY, {CORRECT_USERNAME}},
                       {Common::Entities::User::PASSWORD_KEY, {CORRECT_PASSWORD}}}};

    // Set expectations on the UserRepositoryMock
    const Common::Entities::User user{
        .id = "", .username = CORRECT_USERNAME, .password = CORRECT_PASSWORD, .roleId = ""};
    std::vector<Common::Entities::User> expectedUsers{user};
    EXPECT_CALL(*usersRepositoryMock, getByField(_, _)).WillOnce(Return(expectedUsers));

    const ResponseData response = executeTask(requestData);
    ASSERT_TRUE(response.dataset.find(Keys::_ERROR) == response.dataset.end());
}

TEST_F(BusinessLogicTest, UsersModule_GetRoles)
{
    RequestData requestData{
        .module = "users", .submodule = "roles", .method = "GET", .resourceId = "", .dataset = {}};

    std::vector<Common::Entities::Role> expectedRoles{};
    EXPECT_CALL(*rolesRepositoryMock, getAll()).WillOnce(Return(expectedRoles));
    const ResponseData response = executeTask(requestData);
    ASSERT_TRUE(response.dataset.find(Keys::_ERROR) == response.dataset.end());
}

TEST_F(BusinessLogicTest, InventoryModule_GetProductTypes)
{
    RequestData requestData{.module     = "inventory",
                            .submodule  = "product-types",
                            .method     = "GET",
                            .resourceId = "",
                            .dataset    = {}};

    EXPECT_CALL(*productTypesRepositoryMock, getAll())
        .WillOnce(Return(std::vector<Common::Entities::ProductType>{
            Common::Entities::ProductType{.id = "1",
                                          .code = "PT-001",
                                          .barcode = "123",
                                          .name = "Oil",
                                          .description = "Synthetic oil",
                                          .lastPrice = "10.50",
                                          .unit = "pcs",
                                          .isImported = "0"}}));

    const ResponseData response = executeTask(requestData);
    ASSERT_TRUE(response.dataset.find(Keys::_ERROR) == response.dataset.end());
    ASSERT_TRUE(response.dataset.find(Common::Entities::ProductType::CODE_KEY) !=
                response.dataset.end());
}

TEST_F(BusinessLogicTest, PlannedModule_ReturnsNotImplementedErrorResponse)
{
    RequestData requestData{.module     = "purchase",
                            .submodule  = "orders",
                            .method     = "GET",
                            .resourceId = "",
                            .dataset    = {}};

    const ResponseData response = executeTask(requestData);
    ASSERT_TRUE(response.dataset.find(Keys::_ERROR) != response.dataset.end());
    ASSERT_FALSE(response.dataset.at(Keys::_ERROR).empty());
    EXPECT_EQ(response.dataset.at(Keys::_ERROR).front(), "PurchaseModule");
}

TEST_F(BusinessLogicTest, UnknownModule_ReturnsErrorResponse)
{
    RequestData requestData{
        .module = "billing", .submodule = "list", .method = "GET", .resourceId = "", .dataset = {}};

    const ResponseData response = executeTask(requestData);
    ASSERT_TRUE(response.dataset.find(Keys::_ERROR) != response.dataset.end());
    ASSERT_FALSE(response.dataset.at(Keys::_ERROR).empty());
    EXPECT_EQ(response.dataset.at(Keys::_ERROR).front(), "BusinessLogic");
}

TEST_F(BusinessLogicTest, EmptyModule_ReturnsErrorResponse)
{
    RequestData requestData{
        .module = "", .submodule = "users", .method = "GET", .resourceId = "", .dataset = {}};

    const ResponseData response = executeTask(requestData);
    ASSERT_TRUE(response.dataset.find(Keys::_ERROR) != response.dataset.end());
    ASSERT_FALSE(response.dataset.at(Keys::_ERROR).empty());
    EXPECT_EQ(response.dataset.at(Keys::_ERROR).front(), "BusinessLogic");
}

TEST_F(BusinessLogicTest, ModuleException_IsConvertedToErrorDataset)
{
    constexpr char UNKNOWN_USERNAME[] = "missing-user";
    constexpr char PASSWORD[]         = "password1";

    RequestData requestData{.module     = "users",
                            .submodule  = "login",
                            .method     = "POST",
                            .resourceId = "",
                            .dataset = {{Common::Entities::User::USERNAME_KEY, {UNKNOWN_USERNAME}},
                                        {Common::Entities::User::PASSWORD_KEY, {PASSWORD}}}};

    EXPECT_CALL(*usersRepositoryMock, getByField(_, _))
        .WillOnce(Return(std::vector<Common::Entities::User>{}));
    const ResponseData response = executeTask(requestData);

    ASSERT_TRUE(response.dataset.find(Keys::_ERROR) != response.dataset.end());
    EXPECT_EQ(response.dataset.at(Keys::_ERROR).front(), "UsersModule");
}

int main(int argc, char **argv)
{
    SpdlogConfig::init<SpdlogConfig::LogLevel::Off>();
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
