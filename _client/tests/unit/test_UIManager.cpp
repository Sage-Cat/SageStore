#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QApplication>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include "UI/UIManager.hpp"
#include "Logging.hpp"

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

TEST_F(UIManagerTest, ConstructorDoesNotThrow)
{
    SPDLOG_TRACE("Running test case: ConstructorDoesNotThrow");
    EXPECT_NO_THROW(UIManager manager;);
}

TEST_F(UIManagerTest, SetsThemeDoesNotThrow)
{
    SPDLOG_TRACE("Running test case: SetsThemeDoesNotThrow");
    UIManager manager;
    EXPECT_NO_THROW(manager.setTheme(UIManager::Theme::Light););
}

TEST_F(UIManagerTest, SetsThemeCorrectly)
{
    SPDLOG_TRACE("Running test case: SetsThemeCorrectly");
    UIManager manager;
    manager.setTheme(UIManager::Theme::Light);
    EXPECT_EQ(manager.theme(), UIManager::Theme::Light);
}

TEST_F(UIManagerTest, NoFatalFailureInThemeSetting)
{
    SPDLOG_TRACE("Running test case: NoFatalFailureInThemeSetting");
    UIManager manager;
    EXPECT_NO_FATAL_FAILURE(manager.setTheme(UIManager::Theme::Light));
    EXPECT_EQ(manager.theme(), UIManager::Theme::Light);
}

int main(int argc, char **argv)
{
    Logging::init();
    QApplication app(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}