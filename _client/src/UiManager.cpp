#include "UiManager.hpp"

#include "SpdlogWrapper.hpp"

#include "ViewModels/PurchaseOrdersViewModel.hpp"

UiManager::UiManager(QObject *parent) noexcept
    : QObject(parent)
{
    SPDLOG_TRACE("UiManager::UiManager");
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
}

void UiManager::initModules()
{
    SPDLOG_TRACE("UiManager::initModules");

    // PurchaseOrdersViewModel
    m_purchaseOrdersViewModel = new PurchaseOrdersViewModel(this);
}

void UiManager::initDialogues()
{
    SPDLOG_TRACE("UiManager::initDialogues");
}
