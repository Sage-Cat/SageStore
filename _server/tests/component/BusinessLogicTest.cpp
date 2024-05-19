#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

#include "BusinessLogic/BusinessLogic.hpp"
#include "mocks/RepositoryManagerMock.hpp"
#include "mocks/RepositoryMock.hpp"

#include "common/Keys.hpp"
#include "common/SpdlogConfig.hpp"

using ::testing::_;
using ::testing::Return;

class BusinessLogicTest : public ::testing::Test {
protected:
    std::unique_ptr<BusinessLogic> businessLogic;

    std::shared_ptr<RepositoryManagerMock> repositoryManagerMock;
    std::shared_ptr<RepositoryMock<User>> usersRepositoryMock;
    std::shared_ptr<RepositoryMock<Role>> rolesRepositoryMock;

    BusinessLogicTest()
        : repositoryManagerMock(std::make_shared<RepositoryManagerMock>()),
          usersRepositoryMock(std::make_shared<RepositoryMock<User>>()),
          rolesRepositoryMock(std::make_shared<RepositoryMock<Role>>())
    {
    }

    void SetUp() override
    {
        EXPECT_CALL(*repositoryManagerMock, getUserRepository())
            .WillRepeatedly(Return(usersRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getRoleRepository())
            .WillRepeatedly(Return(rolesRepositoryMock));

        businessLogic = std::make_unique<BusinessLogic>(*repositoryManagerMock);
    }
};

TEST_F(BusinessLogicTest, UsersModule_LoginUser)
{
    constexpr char CORRECT_USERNAME[] = "username1";
    constexpr char CORRECT_PASSWORD[] = "password1";

    RequestData requestData{.module     = "users",
                            .submodule  = "login",
                            .method     = "POST",
                            .resourceId = "",
                            .dataset    = {{Keys::User::USERNAME, {CORRECT_USERNAME}},
                                           {Keys::User::PASSWORD, {CORRECT_PASSWORD}}}};

    // Set expectations on the UserRepositoryMock
    const User user{
        .id = "", .username = CORRECT_USERNAME, .password = CORRECT_PASSWORD, .roleId = ""};
    std::vector<User> expectedUsers{user};
    EXPECT_CALL(*usersRepositoryMock, getByField(_, _)).WillOnce(Return(expectedUsers));

    // Prepare the callback and its expectation
    bool callbackInvoked = false;
    auto callback        = [&callbackInvoked](const ResponseData &responseData) {
        callbackInvoked = true;
        ASSERT_TRUE(responseData.dataset.find(Keys::_ERROR) == responseData.dataset.end());
    };

    // Execute the task
    businessLogic->executeTask(requestData, callback);

    // Verify the callback was invoked
    ASSERT_TRUE(callbackInvoked);
}

TEST_F(BusinessLogicTest, UsersModule_addRole)
{
    RequestData requestData{
        .module = "users", .submodule = "roles", .method = "GET", .resourceId = "", .dataset = {}};

    std::vector<Role> expectedUsers{};
    EXPECT_CALL(*rolesRepositoryMock, getAll()).WillOnce(Return(expectedUsers));
    // Prepare the callback and its expectation
    bool callbackInvoked = false;
    auto callback        = [&callbackInvoked](const ResponseData &responseData) {
        callbackInvoked = true;
        ASSERT_TRUE(responseData.dataset.find(Keys::_ERROR) == responseData.dataset.end());
    };

    // Execute the task
    businessLogic->executeTask(requestData, callback);
}

int main(int argc, char **argv)
{
    SpdlogConfig::init<SpdlogConfig::LogLevel::Off>();
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
