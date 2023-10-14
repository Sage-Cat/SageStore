#include "UIManager.hpp"

#include <QApplication>
#include <QQmlContext>

#include <string>

#include "Logging.hpp"
#include "UI/QmlTypeRegistrar.hpp"

UIManager::UIManager(QObject *parent) noexcept
    : QObject(parent)
{
    SPDLOG_TRACE("UIManager::UIManager");

    QmlTypeRegistrar::registerTypes();

    connect(&m_engine, &QQmlApplicationEngine::objectCreated,
            this, [this](QObject *obj, const QUrl &objUrl)
            { if (!obj) SPDLOG_CRITICAL("Failed to load QML from URL: {}", objUrl.toString().toStdString()); });
}

UIManager::~UIManager()
{
    SPDLOG_TRACE("UIManager::~UIManager");
}

void UIManager::init()
{
    SPDLOG_TRACE("UIManager::init");

    initMainWindow();
    initTheme();
    initModules();
    initDialogues();
}

void UIManager::setTheme(Theme theme)
{
    const auto theme_str = theme == Theme::Dark ? "Dark" : "Light";
    SPDLOG_TRACE(std::string("UIManager::setTheme") + theme_str);

    if (m_theme != theme)
    {
        m_theme = theme;
        SPDLOG_INFO(std::string("Changing theme to ") + theme_str);
        emit themeChanged(theme);
    }
}

UIManager::Theme UIManager::theme() const
{
    SPDLOG_TRACE("UIManager::theme");
    return m_theme;
}

QFont UIManager::defaultFont() const
{
    // TODO: make possibility to setup
    QFont defaultFont("Helvetica");
    defaultFont.setPixelSize(18);
    return defaultFont;
}

void UIManager::initTheme()
{
    SPDLOG_TRACE("UIManager::initTheme");
    setTheme(Theme::Dark);
}

void UIManager::initMainWindow()
{
    SPDLOG_TRACE("UIManager::initMainWindow");

    if (m_engine.rootContext())
    {
        m_engine.rootContext()->setContextProperty("uiManager", this);
        m_engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
        if (m_engine.rootObjects().isEmpty())
            SPDLOG_CRITICAL("UIManager::initMainWindow Failed to load main window");
    }
    else
    {
        SPDLOG_CRITICAL("UIManager::initMainWindow m_engine is invalid");
    }
}

void UIManager::initModules()
{
    SPDLOG_TRACE("UIManager::initModules");
}

void UIManager::initDialogues()
{
    SPDLOG_TRACE("UIManager::initDialogues");
}
