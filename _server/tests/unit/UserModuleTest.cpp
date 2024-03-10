#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

#include "mocks/RepositoryManagerMock.hpp"
#include "mocks/DatabaseManagerMock.hpp"

#include "BusinessLogic/UsersModule.hpp"
#include "src/ServerException.hpp"

#include "SpdlogConfig.hpp"

using ::testing::_;
using ::testing::Return;

class UserModuleTest : public ::testing::Test
{
protected:
    std::unique_ptr<RepositoryManagerMock> repositoryMock;
    std::shared_ptr<DatabaseManagerMock> databaseMock;
    std::unique_ptr<UsersModule> usersModule;

    void SetUp() override
    {
        repositoryMock = std::make_unique<RepositoryManagerMock>();
        usersModule = std::make_unique<UsersModule>(repositoryMock);
    }

    // void TearDown() override
    // {
    // }
};

TEST_F(UserModuleTest, logUser)
{
    Dataset dataset;
    dataset[Keys::User::USERNAME] = {"name"};
    dataset[Keys::User::PASSWORD] = {"pass"};
    RequestData requestData;
    requestData.dataset = dataset;
    requestData.method = "POST";
    requestData.submodule = "login";
    EXPECT_CALL(*repositoryMock, getUsersRepository())
        .WillOnce(testing::Return());

    ResponseData response = usersModule->executeTask(requestData);
    EXPECT_THROW(usersModule->executeTask(requestData), ServerException::exception);
}

// TEST_F(UserModuleTest, regUser)
// {
// }

int main(int argc, char **argv)
{
    SpdlogConfig::init<SpdlogConfig::LogLevel::Off>();
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}