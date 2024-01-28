#include "UiManager.hpp"

#include <QMessageBox>

#include "Network/ApiClient.hpp"

#include "MainWindow.hpp"
#include "Dialogs/LoginDialog.hpp"
#include "Dialogs/RegistrationDialog.hpp"
#include "ViewModels/PurchaseOrdersViewModel.hpp"

#include "SpdlogConfig.hpp"

UiManager::UiManager(ApiClient *apiClient, QObject *parent) noexcept
    : m_apiClient(apiClient), QObject(parent)
{
    SPDLOG_TRACE("UiManager::UiManager");

    // Initialize all necessary elements
    init();
    setupApiConnections();
    setupDialogsConnections();
    setupMVVMConnections();
}

UiManager::~UiManager()
{
    SPDLOG_TRACE("UiManager::~UiManager");
}

void UiManager::init()
{
    SPDLOG_TRACE("UiManager::init");

    initMainWindow();
    initDialogs();
    initViewModels();
    initViews();
    initTheme();
}

void UiManager::initiateAuthorizationProcess()
{
    m_loginDialog->show();
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

void UiManager::showErrorMessageBox(const QString &message)
{
    QMessageBox::critical(nullptr, tr("Error"), message);
}

void UiManager::initTheme()
{
    SPDLOG_TRACE("UiManager::initTheme");
    setTheme(Theme::Dark);
}

void UiManager::initMainWindow()
{
    SPDLOG_TRACE("UiManager::initMainWindow");

    m_mainWindow = new MainWindow();
}

void UiManager::initDialogs()
{
    SPDLOG_TRACE("UiManager::initViews");

    m_loginDialog = new LoginDialog();
    m_registrationDialog = new RegistrationDialog();
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
    connect(m_loginDialog, &LoginDialog::loginAttempted,
            m_apiClient, &ApiClient::loginUser);
    connect(m_apiClient, &ApiClient::loginSuccess,
            this, [this]() { /* nothing for now */ });
    connect(m_apiClient, &ApiClient::loginFailed,
            this, showErrorMessageBox);

    // Registration
    connect(m_registrationDialog, &RegistrationDialog::registrationAttempted,
            m_apiClient, &ApiClient::registerUser);
    connect(m_apiClient, &ApiClient::registerSuccess,
            m_registrationDialog, [this]() { /* TODO: smart login (right after success registration) */ });
    connect(m_apiClient, &ApiClient::registerFailed,
            this, showErrorMessageBox);
}

void UiManager::setupDialogsConnections()
{
    SPDLOG_TRACE("UiManager::setupDialogsConnections");
    // Login
    connect(m_loginDialog, &LoginDialog::registrationRequested,
            [this]()
            {
                m_loginDialog->hide();
                m_registrationDialog->show();
            });

    // Registration
    connect(m_registrationDialog, &RegistrationDialog::requestErrorMessageBox, this, showErrorMessageBox);
    connect(m_registrationDialog, &RegistrationDialog::loginRequested,
            [this]()
            {
                m_registrationDialog->hide();
                m_loginDialog->show();
            });
}

void UiManager::setupMVVMConnections()
{
    SPDLOG_TRACE("UiManager::setupMVVMConnections");
}
