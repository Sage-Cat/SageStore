#include <algorithm>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <string_view>
#include <vector>

#include "Database/IDatabaseManager.hpp"
#include "Database/ProductInfoRepository.hpp"
#include "ServerException.hpp"
#include "common/Entities/ProductInfo.hpp"

#include "common/Keys.hpp"
#include "common/SpdlogConfig.hpp"

#include "gmock/gmock.h"
#include "mocks/DatabaseManagerMock.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

class ProductInfoRepositoryTest : public ::testing::Test {
protected:
    std::shared_ptr<QueryResultMock> queryResultMock;
    std::shared_ptr<DatabaseManagerMock> databaseManagerMock;
    std::unique_ptr<ProductInfoRepository> productInfoRepository;

    ProductInfoRepositoryTest()
        : queryResultMock(std::make_shared<QueryResultMock>()),
          databaseManagerMock(std::make_shared<DatabaseManagerMock>()),
          productInfoRepository(std::make_unique<ProductInfoRepository>(databaseManagerMock))
    {
    }
};

TEST_F(ProductInfoRepositoryTest, add)
{
    const Common::Entities::ProductInfo requestData{
        .id            = "1",
        .productTypeId = "10",
        .name          = "Test_name",
        .value         = "test value",
    };

    EXPECT_CALL(*databaseManagerMock, executeQuery(_, _)).WillRepeatedly(Return(queryResultMock));

    productInfoRepository->add(requestData);
}

TEST_F(ProductInfoRepositoryTest, update)
{
    const Common::Entities::ProductInfo requestData{
        .id            = "1",
        .productTypeId = "10",
        .name          = "Test_name",
        .value         = "test value",
    };

    EXPECT_CALL(*databaseManagerMock, executeQuery(_, _)).WillRepeatedly(Return(queryResultMock));

    productInfoRepository->update(requestData);
}

TEST_F(ProductInfoRepositoryTest, deleteResource)
{
    const std::string requestData{"1"};

    EXPECT_CALL(*databaseManagerMock, executeQuery(_, _)).WillRepeatedly(Return(queryResultMock));

    productInfoRepository->deleteResource(requestData);
}

TEST_F(ProductInfoRepositoryTest, getByField)
{
    EXPECT_CALL(*databaseManagerMock, executeQuery(_, _)).WillRepeatedly(Return(queryResultMock));

    productInfoRepository->getByField("", "");
}

TEST_F(ProductInfoRepositoryTest, getAll)
{
    EXPECT_CALL(*databaseManagerMock, executeQuery(_, _)).WillRepeatedly(Return(queryResultMock));

    productInfoRepository->getAll();
}

int main(int argc, char **argv)
{
    SpdlogConfig::init<SpdlogConfig::LogLevel::Off>();
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}
