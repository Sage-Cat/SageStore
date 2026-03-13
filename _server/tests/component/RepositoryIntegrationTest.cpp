#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <memory>

#include "Database/RepositoryManager.hpp"
#include "Database/ProductTypeRepository.hpp"
#include "Database/RoleRepository.hpp"
#include "Database/SqliteDatabaseManager.hpp"
#include "Database/UserRepository.hpp"

#include "common/Entities/ProductType.hpp"
#include "common/Entities/Role.hpp"
#include "common/Entities/User.hpp"
#include "common/SpdlogConfig.hpp"

class RepositoryIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        const auto uniqueSuffix =
            std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        m_dbPath = std::filesystem::temp_directory_path() /
                   ("sagestore_repository_integration_" + uniqueSuffix + ".db");

        auto dbManager      = std::make_shared<SqliteDatabaseManager>(m_dbPath.string(),
                                                                      TEST_CREATE_DB_SQL_FILE_PATH);
        m_repositoryManager = std::make_unique<RepositoryManager>(dbManager);
        m_userRepository    = m_repositoryManager->getUserRepository();
        m_roleRepository    = m_repositoryManager->getRoleRepository();
        m_productTypeRepository = m_repositoryManager->getProductTypeRepository();
    }

    void TearDown() override
    {
        m_roleRepository.reset();
        m_userRepository.reset();
        m_productTypeRepository.reset();
        m_repositoryManager.reset();
        std::error_code errorCode;
        std::filesystem::remove(m_dbPath, errorCode);
    }

    std::filesystem::path m_dbPath;
    std::unique_ptr<RepositoryManager> m_repositoryManager;
    std::shared_ptr<IRepository<Common::Entities::User>> m_userRepository;
    std::shared_ptr<IRepository<Common::Entities::Role>> m_roleRepository;
    std::shared_ptr<IRepository<Common::Entities::ProductType>> m_productTypeRepository;
};

TEST_F(RepositoryIntegrationTest, RoleRepository_CRUD_RoundTrip)
{
    const auto initialRoles = m_roleRepository->getAll();
    ASSERT_GE(initialRoles.size(), 2U);

    m_roleRepository->add(Common::Entities::Role{.id = "", .name = "auditor"});
    auto createdRoles = m_roleRepository->getByField(Common::Entities::Role::NAME_KEY, "auditor");
    ASSERT_EQ(createdRoles.size(), 1U);
    ASSERT_FALSE(createdRoles.front().id.empty());

    const std::string roleId = createdRoles.front().id;
    m_roleRepository->update(Common::Entities::Role{.id = roleId, .name = "auditor_updated"});
    const auto updatedRoles =
        m_roleRepository->getByField(Common::Entities::Role::NAME_KEY, "auditor_updated");
    ASSERT_EQ(updatedRoles.size(), 1U);
    EXPECT_EQ(updatedRoles.front().id, roleId);

    m_roleRepository->deleteResource(roleId);
    const auto deletedRoles =
        m_roleRepository->getByField(Common::Entities::Role::NAME_KEY, "auditor_updated");
    EXPECT_TRUE(deletedRoles.empty());
}

TEST_F(RepositoryIntegrationTest, UserRepository_CRUD_RoundTrip)
{
    const Common::Entities::User newUser{
        .id = "", .username = "repo_user", .password = "hash_1", .roleId = "1"};
    m_userRepository->add(newUser);

    auto createdUsers =
        m_userRepository->getByField(Common::Entities::User::USERNAME_KEY, "repo_user");
    ASSERT_EQ(createdUsers.size(), 1U);
    ASSERT_FALSE(createdUsers.front().id.empty());
    EXPECT_EQ(createdUsers.front().roleId, "1");

    const std::string userId = createdUsers.front().id;
    m_userRepository->update(Common::Entities::User{
        .id = userId, .username = "repo_user_updated", .password = "hash_2", .roleId = "2"});
    const auto updatedUsers =
        m_userRepository->getByField(Common::Entities::User::USERNAME_KEY, "repo_user_updated");
    ASSERT_EQ(updatedUsers.size(), 1U);
    EXPECT_EQ(updatedUsers.front().password, "hash_2");
    EXPECT_EQ(updatedUsers.front().roleId, "2");

    m_userRepository->deleteResource(userId);
    const auto deletedUsers =
        m_userRepository->getByField(Common::Entities::User::USERNAME_KEY, "repo_user_updated");
    EXPECT_TRUE(deletedUsers.empty());
}

TEST_F(RepositoryIntegrationTest, ProductTypeRepository_CRUD_RoundTrip)
{
    const Common::Entities::ProductType newProductType{.id = "",
                                                       .code = "PT-100",
                                                       .barcode = "100200300",
                                                       .name = "Gear oil",
                                                       .description = "GL-5",
                                                       .lastPrice = "55.20",
                                                       .unit = "pcs",
                                                       .isImported = "1"};
    m_productTypeRepository->add(newProductType);

    auto createdProductTypes =
        m_productTypeRepository->getByField(Common::Entities::ProductType::CODE_KEY, "PT-100");
    ASSERT_EQ(createdProductTypes.size(), 1U);
    ASSERT_FALSE(createdProductTypes.front().id.empty());
    EXPECT_EQ(createdProductTypes.front().name, "Gear oil");

    const std::string productTypeId = createdProductTypes.front().id;
    m_productTypeRepository->update(Common::Entities::ProductType{.id = productTypeId,
                                                                  .code = "PT-100",
                                                                  .barcode = "100200300",
                                                                  .name = "Gear oil updated",
                                                                  .description = "GL-5 updated",
                                                                  .lastPrice = "60.00",
                                                                  .unit = "pcs",
                                                                  .isImported = "0"});

    const auto updatedProductTypes =
        m_productTypeRepository->getByField(Common::Entities::ProductType::CODE_KEY, "PT-100");
    ASSERT_EQ(updatedProductTypes.size(), 1U);
    EXPECT_EQ(updatedProductTypes.front().id, productTypeId);
    EXPECT_EQ(updatedProductTypes.front().name, "Gear oil updated");
    EXPECT_EQ(updatedProductTypes.front().isImported, "0");

    m_productTypeRepository->deleteResource(productTypeId);
    const auto deletedProductTypes =
        m_productTypeRepository->getByField(Common::Entities::ProductType::CODE_KEY, "PT-100");
    EXPECT_TRUE(deletedProductTypes.empty());
}

int main(int argc, char **argv)
{
    SpdlogConfig::init<SpdlogConfig::LogLevel::Off>();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
