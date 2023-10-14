#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QApplication>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "UI/UIManager.hpp"
#include "logging.hpp"

class UIManagerTest : public ::testing::Test
{
protected:
    UIManagerTest()
    {
        SPDLOG_INFO("UIManagerTest setup");
    }

    ~UIManagerTest() override
    {
        SPDLOG_INFO("UIManagerTest teardown");
    }
};

TEST_F(UIManagerTest, ConstructorDoesNotThrow)
{
    SPDLOG_INFO("Running test case: ConstructorDoesNotThrow");
    EXPECT_NO_THROW(UIManager manager;);
}

TEST_F(UIManagerTest, SetsThemeDoesNotThrow)
{
    SPDLOG_INFO("Running test case: SetsThemeDoesNotThrow");
    UIManager manager;
    EXPECT_NO_THROW(manager.setTheme(UIManager::Theme::Light););
}

TEST_F(UIManagerTest, SetsThemeCorrectly)
{
    SPDLOG_INFO("Running test case: SetsThemeCorrectly");
    UIManager manager;
    manager.setTheme(UIManager::Theme::Light);
    EXPECT_EQ(manager.theme(), UIManager::Theme::Light);
}

TEST_F(UIManagerTest, NoFatalFailureInThemeSetting)
{
    SPDLOG_INFO("Running test case: NoFatalFailureInThemeSetting");
    UIManager manager;
    EXPECT_NO_FATAL_FAILURE(manager.setTheme(UIManager::Theme::Light));
    EXPECT_EQ(manager.theme(), UIManager::Theme::Light);
}

int main(int argc, char **argv)
{
    logging::init();
    QApplication app(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}