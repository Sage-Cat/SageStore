#include <gtest/gtest.h>

#include <QApplication>

#include "SageStoreClient.hpp"
#include "SpdlogWrapper.hpp"

class SageStoreClientTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        int argc = 0;
        app = new QApplication(argc, nullptr);
    }

    void TearDown() override
    {
        delete app;
    }

    QApplication *app;
};

TEST_F(SageStoreClientTest, LifetimeNoThrow)
{
    SPDLOG_TRACE("Running test case: LifetimeNoThrow");
    EXPECT_NO_THROW({ SageStoreClient client(*app); });
}

int main(int argc, char **argv)
{
    SpdlogWrapper::init();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
