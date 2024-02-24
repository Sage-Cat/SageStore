#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "BusinessLogic.hpp"
#include "RepositoryManagerMock.hpp"
#include "UsersRepositoryMock.hpp"

using ::testing::_;
using ::testing::Return;

class BusinessLogicTest : public ::testing::Test
{
protected:
    RepositoryManagerMock *repositoryManagerMock;
    BusinessLogic *businessLogic;

    BusinessLogicTest()
    {
        repositoryManagerMock = new RepositoryManagerMock;
        businessLogic = new BusinessLogic(repositoryManagerMock);
    }

    virtual ~BusinessLogicTest()
    {
        delete businessLogic;
    }
};

TEST_F(BusinessLogicTest, ExecuteTask_UsersModule_Success)
{
    // Prepare the mock users repository
    auto usersRepoMock = std::make_shared<UsersRepositoryMock>(nullptr); // No SqliteDatabaseManager needed
    EXPECT_CALL(repoManagerMock, getUsersRepository())
        .WillOnce(Return(usersRepoMock));

    // Set expectations on the UsersRepositoryMock
    User testUser{"userId", "testUsername"};
    EXPECT_CALL(*usersRepoMock, getByUsername(_))
        .WillOnce(Return(std::make_optional(testUser)));

    // Prepare requestData
    RequestData requestData{"users", /* other fields */};

    // Prepare the callback and its expectation
    bool callbackInvoked = false;
    auto callback = [&callbackInvoked](const ResponseData &responseData)
    {
        callbackInvoked = true;
        // Validate the response data
        ASSERT_TRUE(responseData.dataset.find(Keys::_ERROR) == responseData.dataset.end());
    };

    // Execute the task
    businessLogic->executeTask(requestData, callback);

    // Verify the callback was invoked
    ASSERT_TRUE(callbackInvoked);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
