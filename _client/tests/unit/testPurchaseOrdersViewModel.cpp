#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QApplication>

#include "Ui/ViewModels/PurchaseOrdersViewModel.hpp"
#include "SpdlogConfig.hpp"

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
    SpdlogConfig::init();
    QApplication app(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
