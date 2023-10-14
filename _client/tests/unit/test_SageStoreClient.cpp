#include <gtest/gtest.h>

#include <QApplication>

#include "SageStoreClient.hpp"
#include "Logging.hpp"

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

TEST_F(SageStoreClientTest, ConstrNoException)
{
    SPDLOG_TRACE("Running test case: ConstrNoException");
    EXPECT_NO_THROW(SageStoreClient client(*app));
}

TEST_F(SageStoreClientTest, ConstrNoFailure)
{
    SPDLOG_TRACE("Running test case: ConstructorNoFatalFailure");
    EXPECT_NO_FATAL_FAILURE(SageStoreClient client(*app));
}

TEST_F(SageStoreClientTest, DestrNoThrow)
{
    SPDLOG_TRACE("Running test case: DestructorCleansUpResources");

    EXPECT_NO_THROW({
        SageStoreClient client(*app);
    });
}

TEST_F(SageStoreClientTest, DestrNoFalilure)
{
    SPDLOG_TRACE("Running test case: DestructorCleansUpResources");

    EXPECT_NO_FATAL_FAILURE({
        SageStoreClient client(*app);
    });
}

int main(int argc, char **argv)
{
    Logging::init();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
