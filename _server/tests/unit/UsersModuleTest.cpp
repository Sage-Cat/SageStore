#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <vector>

#include "BusinessLogic/BusinessLogic.hpp"
#include "mocks/RepositoryManagerMock.hpp"
#include "mocks/RepositoryMock.hpp"

#include "BusinessLogic/UsersModule.hpp"
#include "ServerException.hpp"

#include "common/Keys.hpp"
#include "common/SpdlogConfig.hpp"

using ::testing::_;
using ::testing::Return;

class UsersModuleTest : public ::testing::Test {
protected:
    std::unique_ptr<UsersModule> usersModule;
    std::shared_ptr<RepositoryManagerMock> repositoryManagerMock;
    std::shared_ptr<RepositoryMock<Common::Entities::User>> usersRepositoryMock;
    std::shared_ptr<RepositoryMock<Common::Entities::Role>> rolesRepositoryMock;

    UsersModuleTest()
        : repositoryManagerMock(std::make_shared<RepositoryManagerMock>()),
          usersRepositoryMock(std::make_shared<RepositoryMock<Common::Entities::User>>()),
          rolesRepositoryMock(std::make_shared<RepositoryMock<Common::Entities::Role>>())
    {
        EXPECT_CALL(*repositoryManagerMock, getUserRepository())
            .WillRepeatedly(Return(usersRepositoryMock));
        EXPECT_CALL(*repositoryManagerMock, getRoleRepository())
            .WillRepeatedly(Return(rolesRepositoryMock));

        usersModule = std::make_unique<UsersModule>(*repositoryManagerMock);
    }
};

TEST_F(UsersModuleTest, loginUser)
{
    const std::string CORRECT_USERNAME = "username1";
    const std::string CORRECT_PASSWORD = "password1";

    RequestData requestData{
        .module     = "users",
        .submodule  = "login",
        .method     = "POST",
        .resourceId = "",
        .dataset    = {{Common::Entities::User::USERNAME_KEY, {CORRECT_USERNAME}},
                       {Common::Entities::User::PASSWORD_KEY, {CORRECT_PASSWORD}}}};

    const Common::Entities::User user{
        .id = "1", .username = CORRECT_USERNAME, .password = CORRECT_PASSWORD, .roleId = "role1"};
    std::vector<Common::Entities::User> expectedUsers{user};
    EXPECT_CALL(*usersRepositoryMock, getByField(_, _)).WillOnce(Return(expectedUsers));

    ResponseData response = usersModule->executeTask(requestData);
    EXPECT_EQ(response.dataset.at(Common::Entities::User::ID_KEY).front(), expectedUsers[0].id);
    EXPECT_EQ(response.dataset.at(Common::Entities::User::ROLE_ID_KEY).front(),
              expectedUsers[0].roleId);
}

TEST_F(UsersModuleTest, getUsers)
{
    RequestData requestData{
        .module = "users", .submodule = "users", .method = "GET", .resourceId = "", .dataset = {}};

    std::vector<Common::Entities::User> expectedUsers{
        Common::Entities::User{"1", "user1", "pass1", "role1"},
        Common::Entities::User{"2", "user2", "pass2", "role2"}};

    EXPECT_CALL(*usersRepositoryMock, getAll()).WillOnce(Return(expectedUsers));

    ResponseData response = usersModule->executeTask(requestData);
    for (size_t i = 0; i < expectedUsers.size(); i++) {
        auto it_id = response.dataset.at(Common::Entities::User::ID_KEY).begin();
        std::advance(it_id, i);
        EXPECT_EQ(*it_id, expectedUsers[i].id);

        auto it_username = response.dataset.at(Common::Entities::User::USERNAME_KEY).begin();
        std::advance(it_username, i);
        EXPECT_EQ(*it_username, expectedUsers[i].username);

        auto it_roleId = response.dataset.at(Common::Entities::User::ROLE_ID_KEY).begin();
        std::advance(it_roleId, i);
        EXPECT_EQ(*it_roleId, expectedUsers[i].roleId);
    }
}

TEST_F(UsersModuleTest, addUser)
{
    const std::string CORRECT_USERNAME = "newuser";
    const std::string CORRECT_PASSWORD = "newpassword";
    const std::string CORRECT_ROLE_ID  = "newrole";

    RequestData requestData{.module     = "users",
                            .submodule  = "users",
                            .method     = "POST",
                            .resourceId = "",
                            .dataset = {{Common::Entities::User::USERNAME_KEY, {CORRECT_USERNAME}},
                                        {Common::Entities::User::PASSWORD_KEY, {CORRECT_PASSWORD}},
                                        {Common::Entities::User::ROLE_ID_KEY, {CORRECT_ROLE_ID}}}};

    std::vector<Common::Entities::User> expectedUsers{};
    EXPECT_CALL(*usersRepositoryMock, getByField(_, _)).WillOnce(Return(expectedUsers));
    EXPECT_CALL(*usersRepositoryMock, add(_)).WillOnce(Return());

    EXPECT_NO_THROW(usersModule->executeTask(requestData));
}

TEST_F(UsersModuleTest, editUser)
{
    const std::string USER_ID      = "1";
    const std::string NEW_USERNAME = "updateduser";
    const std::string NEW_PASSWORD = "updatedpassword";
    const std::string NEW_ROLE_ID  = "updatedrole";

    RequestData requestData{.module     = "users",
                            .submodule  = "users",
                            .method     = "PUT",
                            .resourceId = USER_ID,
                            .dataset    = {{Common::Entities::User::USERNAME_KEY, {NEW_USERNAME}},
                                           {Common::Entities::User::PASSWORD_KEY, {NEW_PASSWORD}},
                                           {Common::Entities::User::ROLE_ID_KEY, {NEW_ROLE_ID}}}};

    EXPECT_CALL(*usersRepositoryMock, update(_)).WillOnce(Return());

    EXPECT_NO_THROW(usersModule->executeTask(requestData));
}

TEST_F(UsersModuleTest, deleteUser)
{
    const std::string USER_ID = "1";

    RequestData requestData{.module     = "users",
                            .submodule  = "users",
                            .method     = "DELETE",
                            .resourceId = USER_ID,
                            .dataset    = {}};

    EXPECT_CALL(*usersRepositoryMock, deleteResource(USER_ID)).WillOnce(Return());

    EXPECT_NO_THROW(usersModule->executeTask(requestData));
}

TEST_F(UsersModuleTest, getRoles)
{
    RequestData requestData{
        .module = "users", .submodule = "roles", .method = "GET", .resourceId = "", .dataset = {}};

    std::vector<Common::Entities::Role> expectedRoles{
        Common::Entities::Role{.id = "1", .name = "user"},
        Common::Entities::Role{.id = "2", .name = "admin"}};

    EXPECT_CALL(*rolesRepositoryMock, getAll()).WillOnce(Return(expectedRoles));

    ResponseData response = usersModule->executeTask(requestData);
    for (size_t i = 0; i < expectedRoles.size(); i++) {
        auto it_roleId = response.dataset.at(Common::Entities::Role::ID_KEY).begin();
        std::advance(it_roleId, i);
        EXPECT_EQ(*it_roleId, expectedRoles[i].id);

        auto it_roleName = response.dataset.at(Common::Entities::Role::NAME_KEY).begin();
        std::advance(it_roleName, i);
        EXPECT_EQ(*it_roleName, expectedRoles[i].name);
    }
}

TEST_F(UsersModuleTest, addRoles)
{
    const std::string CORRECT_NAME = "adminchik";
    RequestData requestData{.module     = "users",
                            .submodule  = "roles",
                            .method     = "POST",
                            .resourceId = "",
                            .dataset    = {{Common::Entities::Role::NAME_KEY, {CORRECT_NAME}}}};

    std::vector<Common::Entities::Role> expectedRoles{};
    EXPECT_CALL(*rolesRepositoryMock, getByField(_, _)).WillOnce(Return(expectedRoles));
    EXPECT_CALL(*rolesRepositoryMock, add(_)).WillOnce(Return());

    EXPECT_NO_THROW(usersModule->executeTask(requestData));
}

TEST_F(UsersModuleTest, editRole)
{
    const std::string CORRECT_ID = "3";
    const std::string NEW_NAME   = "admin3";
    RequestData requestData{.module     = "users",
                            .submodule  = "roles",
                            .method     = "PUT",
                            .resourceId = CORRECT_ID,
                            .dataset    = {{Common::Entities::Role::NAME_KEY, {NEW_NAME}}}};

    EXPECT_CALL(*rolesRepositoryMock, update(_)).WillOnce(Return());

    EXPECT_NO_THROW(usersModule->executeTask(requestData));
}

TEST_F(UsersModuleTest, deleteRoles)
{
    const std::string CORRECT_ID = "4";

    RequestData requestData{.module     = "users",
                            .submodule  = "roles",
                            .method     = "DELETE",
                            .resourceId = CORRECT_ID,
                            .dataset    = {}};

    EXPECT_CALL(*rolesRepositoryMock, deleteResource(CORRECT_ID)).WillOnce(Return());

    EXPECT_NO_THROW(usersModule->executeTask(requestData));
}

int main(int argc, char **argv)
{
    SpdlogConfig::init<SpdlogConfig::LogLevel::Off>();
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
