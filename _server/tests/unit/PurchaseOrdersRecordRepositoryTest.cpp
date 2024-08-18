#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include <iostream>

#include "mocks/DatabaseManagerMock.hpp"
#include "Database/PurchaseOrdersRecordRepository.hpp"
#include "ServerException.hpp"

#include "common/Entities/PurchaseOrderRecord.hpp"
#include "common/Keys.hpp"
#include "common/SpdlogConfig.hpp"

using ::testing::_;
using ::testing::Return;

class PurchaseOrderRecordRepoTest : public ::testing::Test 
{
protected:
    std::unique_ptr<PurchaseOrderRecordRepository> purchOrdRecRepo;
    std::shared_ptr<DatabaseManagerMock> dataMock;
    std::shared_ptr<QueryResultMock> qureMock;

    PurchaseOrderRecordRepoTest() :
        dataMock(std::make_shared<DatabaseManagerMock>()),
        qureMock(std::make_shared<QueryResultMock>())
    {
        purchOrdRecRepo = std::make_unique<PurchaseOrderRecordRepository>(dataMock);
    }
};

TEST_F(PurchaseOrderRecordRepoTest, getAll)
{
    Purchaseorderrecord current;
    current.id = "2";
    current.orderId = "3";
    current.productTypeId = "44";
    current.quantity = "12";
    current.price = "900";

    EXPECT_CALL(*dataMock, executeQuery("SELECT id, orderId, productTypeId, quantity, price FROM " + std::string(Purchaseorderrecord::TABLE_NAME) + ";", ::testing::_))
        .WillOnce(::testing::Return(qureMock));
    EXPECT_CALL(*qureMock, next())
        .WillOnce(::testing::Return(true))
        .WillOnce(::testing::Return(false)); 

    EXPECT_CALL(*qureMock, getString(0)).WillOnce(::testing::Return(current.id));
    EXPECT_CALL(*qureMock, getString(1)).WillOnce(::testing::Return(current.orderId));
    EXPECT_CALL(*qureMock, getString(2)).WillOnce(::testing::Return(current.productTypeId));
    EXPECT_CALL(*qureMock, getString(3)).WillOnce(::testing::Return(current.quantity));
    EXPECT_CALL(*qureMock, getString(4)).WillOnce(::testing::Return(current.price));

    auto data = purchOrdRecRepo->getAll();
    for(auto purch : data)
    {
        ASSERT_EQ(purch.id, current.id);
        ASSERT_EQ(purch.orderId, current.orderId);
        ASSERT_EQ(purch.productTypeId, current.productTypeId);
        ASSERT_EQ(purch.quantity, current.quantity);
        ASSERT_EQ(purch.price, current.price);
    }
}

TEST_F(PurchaseOrderRecordRepoTest, getByField)
{
    Purchaseorderrecord current;
    current.id = "2";
    current.orderId = "3";
    current.productTypeId = "44";
    current.quantity = "12";
    current.price = "900";
EXPECT_CALL(*dataMock, executeQuery("SELECT id, orderId, productTypeId, quantity, price FROM PurchaseOrderRecord WHERE id = ?;", ::testing::_))
.WillOnce(::testing::Return(qureMock));

    EXPECT_CALL(*qureMock, next()).WillOnce(::testing::Return(true)).WillOnce(::testing::Return(false));

    EXPECT_CALL(*qureMock, getString(0)).WillOnce(::testing::Return(current.id));
    EXPECT_CALL(*qureMock, getString(1)).WillOnce(::testing::Return(current.orderId));
    EXPECT_CALL(*qureMock, getString(2)).WillOnce(::testing::Return(current.productTypeId));
    EXPECT_CALL(*qureMock, getString(3)).WillOnce(::testing::Return(current.quantity));
    EXPECT_CALL(*qureMock, getString(4)).WillOnce(::testing::Return(current.price));
        
    auto data = purchOrdRecRepo->getByField("id", "2");
    for(auto purch : data)
    {
        ASSERT_EQ(purch.id, current.id);
        ASSERT_EQ(purch.orderId, current.orderId);
        ASSERT_EQ(purch.productTypeId, current.productTypeId);
        ASSERT_EQ(purch.quantity, current.quantity);
        ASSERT_EQ(purch.price, current.price);
    }
}
