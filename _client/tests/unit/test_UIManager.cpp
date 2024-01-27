#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QApplication>

#include "UiManager.hpp"
#include "SpdlogConfig.hpp"

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
    EXPECT_NO_THROW(UiManager manager;);
}

TEST_F(UIManagerTest, SetsThemeDoesNotThrow)
{
    SPDLOG_TRACE("Running test case: SetsThemeDoesNotThrow");
    UiManager manager;
    EXPECT_NO_THROW(manager.setTheme(UiManager::Theme::Light););
}

TEST_F(UIManagerTest, SetsThemeCorrectly)
{
    SPDLOG_TRACE("Running test case: SetsThemeCorrectly");
    UiManager manager;
    manager.setTheme(UiManager::Theme::Light);
    EXPECT_EQ(manager.theme(), UiManager::Theme::Light);
}

TEST_F(UIManagerTest, NoFatalFailureInThemeSetting)
{
    SPDLOG_TRACE("Running test case: NoFatalFailureInThemeSetting");
    UiManager manager;
    EXPECT_NO_FATAL_FAILURE(manager.setTheme(UiManager::Theme::Light));
    EXPECT_EQ(manager.theme(), UiManager::Theme::Light);
}

int main(int argc, char **argv)
{
    SpdlogConfig::init();
    QApplication app(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}