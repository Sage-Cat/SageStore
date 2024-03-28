#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include <iostream>

#include "BusinessLogic/BusinessLogic.hpp"
#include "RepositoryManagerMock.hpp"
#include "RepositoryMock.hpp"

#include "BusinessLogic/UsersModule.hpp"
#include "ServerException.hpp"

#include "SpdlogConfig.hpp"

using ::testing::_;
using ::testing::Return;

class UserModuleTest : public ::testing::Test
{
protected:
    std::unique_ptr<UsersModule> usersModule;
    std::shared_ptr<RepositoryManagerMock> repositoryManagerMock;
    std::shared_ptr<RepositoryMock<User>> usersRepositoryMock;
    std::shared_ptr<RepositoryMock<Role>> rolesRepositoryMock;

    UserModuleTest()
        : repositoryManagerMock(std::make_shared<RepositoryManagerMock>()),
          usersRepositoryMock(std::make_shared<RepositoryMock<User>>()),
          rolesRepositoryMock(std::make_shared<RepositoryMock<Role>>())
    {
        EXPECT_CALL(*repositoryManagerMock, getUsersRepository())
            .WillRepeatedly(Return(usersRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getRolesRepository())
            .WillRepeatedly(Return(rolesRepositoryMock));

        usersModule = std::make_unique<UsersModule>(*repositoryManagerMock);
    }
};

TEST_F(UserModuleTest, loginUser)
{
    constexpr char CORRECT_USERNAME[] = "username1";
    constexpr char CORRECT_PASSWORD[] = "password1";

    RequestData requestData{
        .module = "users",
        .submodule = "login",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Keys::User::USERNAME, {CORRECT_USERNAME}},
                    {Keys::User::PASSWORD, {CORRECT_PASSWORD}}}};
    std::vector<User> expectedUsers{User("", CORRECT_USERNAME, CORRECT_PASSWORD, "")};
    EXPECT_CALL(*usersRepositoryMock, getByField(_, _)).WillOnce(Return(expectedUsers));

    ResponseData response = usersModule->executeTask(requestData);
    EXPECT_EQ(response.dataset.at(Keys::User::PASSWORD).front(), expectedUsers[0].password);
    EXPECT_EQ(response.dataset.at(Keys::User::USERNAME).front(), expectedUsers[0].username);
}

TEST_F(UserModuleTest, registerUser)
{
    constexpr char CORRECT_USERNAME[] = "username12";
    constexpr char CORRECT_PASSWORD[] = "password12";

    RequestData requestData{
        .module = "users",
        .submodule = "register",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Keys::User::USERNAME, {CORRECT_USERNAME}},
                    {Keys::User::PASSWORD, {CORRECT_PASSWORD}}}};

    std::vector<User> expectedUsers{};
    EXPECT_CALL(*usersRepositoryMock, getByField(_, _)).WillOnce(Return(expectedUsers));

    ResponseData response = usersModule->executeTask(requestData);
    for (size_t i = 0; i < expectedUsers.size(); i++)
    {
        EXPECT_EQ(response.dataset.at(Keys::User::PASSWORD).front(), expectedUsers[i].password);
        EXPECT_EQ(response.dataset.at(Keys::User::USERNAME).front(), expectedUsers[i].username);
    }
}

TEST_F(UserModuleTest, getRoles)
{
    RequestData requestData{
        .module = "users",
        .submodule = "roles",
        .method = "GET",
        .resourceId = "",
        .dataset = {}};

    std::vector<Role> expectedUsers{Role("1", "user"), Role("2", "admin")};
    EXPECT_CALL(*rolesRepositoryMock, getAll()).WillOnce(Return(expectedUsers));

    ResponseData response = usersModule->executeTask(requestData);
    for (size_t i = 0; i < expectedUsers.size(); i++)
    {
        EXPECT_EQ(response.dataset.at(Keys::Role::ID).front(), expectedUsers[i].id);
        EXPECT_EQ(response.dataset.at(Keys::Role::NAME).front(), expectedUsers[i].name);
        response.dataset.at(Keys::Role::ID).pop_front();
        response.dataset.at(Keys::Role::NAME).pop_front();
    }
}

TEST_F(UserModuleTest, addRoles)
{
    constexpr char CORRECT_ID[] = "3";
    constexpr char CORRECT_NAME[] = "adminchik";
    RequestData requestData{
        .module = "users",
        .submodule = "roles",
        .method = "POST",
        .resourceId = "",
        .dataset = {{Keys::Role::ID, {CORRECT_ID}},
                    {Keys::Role::NAME, {CORRECT_NAME}}}};

    std::vector<Role> expectedUsers{Role(CORRECT_ID, CORRECT_NAME)};
    EXPECT_CALL(*rolesRepositoryMock, add(_)).WillOnce(Return());

    ResponseData response = usersModule->executeTask(requestData);

    EXPECT_EQ(response.dataset.at(Keys::Role::NAME).front(), expectedUsers[0].name);
}

TEST_F(UserModuleTest, editRole)
{
    constexpr char CORRECT_ID[] = "3";
    constexpr char CORRECT_NAME[] = "admin3";
    RequestData requestData{
        .module = "users",
        .submodule = "roles",
        .method = "PUT",
        .resourceId = "3",
        .dataset = {{Keys::Role::ID, {CORRECT_ID}},
                    {Keys::Role::NAME, {CORRECT_NAME}}}};

    std::vector<Role> expectedUsers{};
    EXPECT_CALL(*rolesRepositoryMock, update(_)).WillOnce(Return());

    ResponseData response = usersModule->executeTask(requestData);
    for (size_t i = 0; i < expectedUsers.size(); i++)
    {
        EXPECT_EQ(response.dataset.at(Keys::Role::ID).front(), expectedUsers[i].id);
        EXPECT_EQ(response.dataset.at(Keys::Role::NAME).front(), expectedUsers[i].name);
    }
}

TEST_F(UserModuleTest, deleteRoles)
{
    constexpr char CORRECT_ID[] = "4";
    constexpr char CORRECT_NAME[] = "admin4";
    RequestData requestData{
        .module = "users",
        .submodule = "roles",
        .method = "DEL",
        .resourceId = "4",
        .dataset = {{Keys::Role::ID, {CORRECT_ID}},
                    {Keys::Role::NAME, {CORRECT_NAME}}}};

    std::vector<Role> expectedUsers{};
    EXPECT_CALL(*rolesRepositoryMock, deleteResource(_)).WillOnce(Return());

    ResponseData response = usersModule->executeTask(requestData);
}

int main(int argc, char **argv)
{
    SpdlogConfig::init<SpdlogConfig::LogLevel::Off>();
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
