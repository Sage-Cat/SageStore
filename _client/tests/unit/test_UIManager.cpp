#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QApplication>

#include "UiManager.hpp"
#include <spdlog/spdlog.h>

class UIManagerTest : public ::testing::Test
{
protected:
    UIManagerTest()
    {
    }

    ~UIManagerTest() override
    {
    }
};

int main(int argc, char **argv)
{
    SpdlogConfig::init();
    QApplication app(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}