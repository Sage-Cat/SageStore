#include <gtest/gtest.h>
#include <QApplication>
#include "SageStoreClient.hpp"
#include "logging.hpp"

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

TEST_F(SageStoreClientTest, ConstructorDoesNotThrow)
{
    SPDLOG_INFO("Running test case: ConstructorDoesNotThrow");
    EXPECT_NO_THROW(SageStoreClient client(*app));
}

TEST_F(SageStoreClientTest, ConstructorNoFatalFailure)
{
    SPDLOG_INFO("Running test case: ConstructorNoFatalFailure");
    EXPECT_NO_FATAL_FAILURE(SageStoreClient client(*app));
}

TEST_F(SageStoreClientTest, DestructorCleansUpResources)
{
    SPDLOG_INFO("Running test case: DestructorCleansUpResources");

    EXPECT_NO_FATAL_FAILURE({
        SageStoreClient client(*app);
    });
}

int main(int argc, char **argv)
{
    logging::init();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
