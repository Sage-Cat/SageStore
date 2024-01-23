#include <gtest/gtest.h>

#include <QApplication>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include "Ui/MainWindow.hpp"
#include "SpdlogWrapper.hpp"

class MainWindowTest : public ::testing::Test
{
protected:
    MainWindowTest()
    {
    }

    ~MainWindowTest() override
    {
    }
};

TEST_F(MainWindowTest, ConstructorDoesNotThrow)
{
    SPDLOG_TRACE("Running test case: ConstructorDoesNotThrow");
    EXPECT_NO_THROW(MainWindow mainwind);
}

int main(int argc, char **argv)
{
    SpdlogWrapper::init();
    QApplication app(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();