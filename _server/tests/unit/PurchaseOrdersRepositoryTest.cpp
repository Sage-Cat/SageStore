#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include <iostream>

#include "mocks/DatabaseManagerMock.hpp"
#include "Database/PurchaseOrdersRepository.hpp"
#include "ServerException.hpp"

#include "common/Entities/PurchaseOrder.hpp"
#include "common/Keys.hpp"
#include "common/SpdlogConfig.hpp"

using ::testing::_;
using ::testing::Return;

class PurchaseOrderRepoTest : public ::testing::Test {
protected:
    std::unique_ptr<PurchaseOrderRepository> purchOrdRepo;
    std::shared_ptr<DatabaseManagerMock> dataMock;
    std::shared_ptr<QueryResultMock> qureMock;

    PurchaseOrderRepoTest()
        : dataMock(std::make_shared<DatabaseManagerMock>()),
          qureMock(std::make_shared<QueryResultMock>())
    {
        purchOrdRepo = std::make_unique<PurchaseOrderRepository>(dataMock);
    }
};

TEST_F(PurchaseOrderRepoTest, getAll)
{
    Purchaseorder current;
    current.id = "2";
    current.date = "12-01-22";
    current.userId = "47";
    current.supplierId = "11";
    current.status = "sus";

    EXPECT_CALL(*dataMock, executeQuery("SELECT id, date, userId, supplierId, status FROM PurchaseOrder;", ::testing::_))
        .WillOnce(::testing::Return(qureMock));
    EXPECT_CALL(*qureMock, next())
        .WillOnce(::testing::Return(true))
        .WillOnce(::testing::Return(false)); 
    EXPECT_CALL(*qureMock, getString(0)).WillOnce(::testing::Return(current.id));
    EXPECT_CALL(*qureMock, getString(1)).WillOnce(::testing::Return(current.date));
    EXPECT_CALL(*qureMock, getString(2)).WillOnce(::testing::Return(current.userId));
    EXPECT_CALL(*qureMock, getString(3)).WillOnce(::testing::Return(current.supplierId));
    EXPECT_CALL(*qureMock, getString(4)).WillOnce(::testing::Return(current.status));

    auto data = purchOrdRepo->getAll();
    for(auto purch : data)
    {
        ASSERT_EQ(purch.id, current.id);
        ASSERT_EQ(purch.date, current.date);
        ASSERT_EQ(purch.userId, current.userId);
        ASSERT_EQ(purch.supplierId, current.supplierId);
        ASSERT_EQ(purch.status, current.status);
    }
}

TEST_F(PurchaseOrderRepoTest, getByField)
{
    Purchaseorder current;
    current.id = "2";
    current.date = "12-01-22";
    current.userId = "47";
    current.supplierId = "11";
    current.status = "sus";

    EXPECT_CALL(*dataMock, executeQuery("SELECT id, date, userId, supplierId, status FROM PurchaseOrder WHERE status = ?;", ::testing::_))
        .WillOnce(::testing::Return(qureMock));
    EXPECT_CALL(*qureMock, next())
        .WillOnce(::testing::Return(true))
        .WillOnce(::testing::Return(false));
    
    EXPECT_CALL(*qureMock, getString(0)).WillOnce(::testing::Return(current.id));
    EXPECT_CALL(*qureMock, getString(1)).WillOnce(::testing::Return(current.date));
    EXPECT_CALL(*qureMock, getString(2)).WillOnce(::testing::Return(current.userId));
    EXPECT_CALL(*qureMock, getString(3)).WillOnce(::testing::Return(current.supplierId));
    EXPECT_CALL(*qureMock, getString(4)).WillOnce(::testing::Return(current.status));


    auto data = purchOrdRepo->getByField("status", "sus");
    for(auto purch : data)
    {
        ASSERT_EQ(purch.id, current.id);
        ASSERT_EQ(purch.date, current.date);
        ASSERT_EQ(purch.userId, current.userId);
        ASSERT_EQ(purch.supplierId, current.supplierId);
        ASSERT_EQ(purch.status, current.status);
    }
}

