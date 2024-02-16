#include "UiManager.hpp"

#include <QMessageBox>

#include "Network\ApiManager.hpp"

#include "MainWindow.hpp"

#include "Dialogs/DialogManager.hpp"
#include "Dialogs/LoginDialog.hpp"
#include "Dialogs/RegistrationDialog.hpp"

#include "ViewModels/PurchaseOrdersViewModel.hpp"

#include "SpdlogConfig.hpp"

UiManager::UiManager(ApiManager *apiClient, QObject *parent) noexcept
    : m_apiManager(apiClient), QObject(parent)
{
    SPDLOG_TRACE("UiManager::UiManager");

    // Initialize all necessary elements
    init();
    setupApiConnections();
    setupMVVMConnections();

    // Before initialization
    m_mainWindow->hide();

    // Error handling
    // connect(m_apiManager, &ApiManager::errorOccurred, this, showErrorMessageBox);
}

UiManager::~UiManager()
{
    SPDLOG_TRACE("UiManager::~UiManager");
}

void UiManager::init()
{
    SPDLOG_TRACE("UiManager::init");

    m_mainWindow = new MainWindow();
    m_dialogManager = new DialogManager(m_apiManager);
    initViewModels();
    initViews();
    initTheme();
}

void UiManager::startUiProcess()
{
    m_dialogManager->showLoginDialog();
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
    SPDLOG_TRACE("UiManager::defaultFont");
    // TODO: make possibility to setup
    QFont defaultFont("Helvetica");
    defaultFont.setPixelSize(18);
    return defaultFont;
}

void UiManager::handleError(const QString &message)
{
    m_dialogManager->showErrorMessageBox(message);
}

void UiManager::initTheme()
{
    SPDLOG_TRACE("UiManager::initTheme");
    setTheme(Theme::Dark);
}

void UiManager::initViewModels()
{
    SPDLOG_TRACE("UiManager::initViewModels");
}

void UiManager::initViews()
{
    SPDLOG_TRACE("UiManager::initViews");
}

void UiManager::setupApiConnections()
{
    SPDLOG_TRACE("UiManager::setupApiConnections");
    // Login
    connect(m_apiManager, &ApiManager::loginSuccess,
            m_mainWindow, &MainWindow::showMaximized);
}

void UiManager::setupMVVMConnections()
{
    SPDLOG_TRACE("UiManager::setupMVVMConnections");
}
