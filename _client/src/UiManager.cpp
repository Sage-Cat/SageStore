#include "UiManager.hpp"

#include <QApplication>
#include <QQmlContext>

#include "SpdlogWrapper.hpp"
#include "Ui/QmlTypeRegistrar.hpp"

#include "ViewModels/PurchaseOrdersViewModel.hpp"
#include "ViewModels/NAME_HEREViewModel.hpp"

UiManager::UiManager(QObject *parent) noexcept
    : QObject(parent)
{
    SPDLOG_TRACE("UiManager::UiManager");

    QmlTypeRegistrar::registerTypes();

    connect(&m_engine, &QQmlApplicationEngine::objectCreated,
            this, [this](QObject *obj, const QUrl &objUrl)
            { if (!obj) SPDLOG_CRITICAL("Failed to load QML from URL: {}", objUrl.toString().toStdString()); });
}

UiManager::~UiManager()
{
    SPDLOG_TRACE("UiManager::~UiManager");
}

void UiManager::initUi()
{
    SPDLOG_TRACE("UiManager::init");

    initMainWindow();
    initTheme();
    initModules();
    initDialogues();
}

void UiManager::setTheme(Theme theme)
{
    const auto theme_str = theme == Theme::Dark ? "Dark" : "Light";
    SPDLOG_TRACE(std::string("UiManager::setTheme") + theme_str);

    if (m_theme != theme)
    {
        m_theme = theme;
        SPDLOG_INFO(std::string("Changing theme to ") + theme_str);
        emit themeChanged(theme);
    }
}

UiManager::Theme UiManager::theme() const
{
    SPDLOG_TRACE("UiManager::theme");
    return m_theme;
}

QFont UiManager::defaultFont() const
{
    // TODO: make possibility to setup
    QFont defaultFont("Helvetica");
    defaultFont.setPixelSize(18);
    return defaultFont;
}

void UiManager::initTheme()
{
    SPDLOG_TRACE("UiManager::initTheme");
    setTheme(Theme::Dark);
}

void UiManager::initMainWindow()
{
    SPDLOG_TRACE("UiManager::initMainWindow");

    if (m_engine.rootContext())
    {
        m_engine.rootContext()->setContextProperty("uiManager", this);
        m_engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
        if (m_engine.rootObjects().isEmpty())
            SPDLOG_CRITICAL("UiManager::initMainWindow Failed to load main window");
    }
    else
    {
        SPDLOG_CRITICAL("UiManager::initMainWindow m_engine is invalid");
    }
}

void UiManager::initModules()
{
    SPDLOG_TRACE("UiManager::initModules");

    // PurchaseOrdersViewModel
    m_purchaseOrdersViewModel = new PurchaseOrdersViewModel(this);
    // NAME_HEREViewModel
    m_nAME_HEREViewModel = new NAME_HEREViewModel(this);
}

void UiManager::initDialogues()
{
    SPDLOG_TRACE("UiManager::initDialogues");
}
