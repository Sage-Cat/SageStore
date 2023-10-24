#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QApplication>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include "ViewModels/PurchaseOrdersViewModel.hpp"
#include "SpdlogWrapper.hpp"

class PurchaseOrdersViewModelTest : public ::testing::Test
{
protected:
    PurchaseOrdersViewModelTest()
    {
    }

    ~PurchaseOrdersViewModelTest() override
    {
    }
};

TEST_F(PurchaseOrdersViewModelTest, LifetimeNoThrow)
{
    SPDLOG_TRACE("Running test case: LifetimeNoThrow");
    EXPECT_NO_THROW({ PurchaseOrdersViewModel viewModel; });
}

int main(int argc, char **argv)
{
    SpdlogWrapper::init();
    QApplication app(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
