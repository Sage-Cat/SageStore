#include "UiManager.hpp"

#include <QMessageBox>

#include "SpdlogWrapper.hpp"

#include "ViewModels/AuthorizationViewModel.hpp"
#include "ViewModels/RegistrationViewModel.hpp"
#include "ViewModels/PurchaseOrdersViewModel.hpp"

#include "Views/AuthorizationView.hpp"
#include "Views/RegistrationView.hpp"

UiManager::UiManager(QObject *parent) noexcept
    : QObject(parent)
{
    SPDLOG_TRACE("UiManager::UiManager");

    // Initialize all necessary elements
    init();
    setupVVMConnections();
}

UiManager::~UiManager()
{
    SPDLOG_TRACE("UiManager::~UiManager");
}

void UiManager::init()
{
    SPDLOG_TRACE("UiManager::init");

    initMainWindow();
    initTheme();
    initViewModels();
    initViews();
}

void UiManager::initiateAuthorizationProcess()
{
    m_authorizationView->show();
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

AuthorizationViewModel *UiManager::authorizationViewModel() const
{
    SPDLOG_TRACE("UiManager::authorizationViewModel");
    return m_authorizationViewModel;
}

RegistrationViewModel *UiManager::registrationViewModel() const
{
    SPDLOG_TRACE("UiManager::registrationViewModel");
    return m_registrationViewModel;
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
}

void UiManager::initViewModels()
{
    SPDLOG_TRACE("UiManager::initViewModels");

    m_authorizationViewModel = new AuthorizationViewModel;
    m_registrationViewModel = new RegistrationViewModel;
}

void UiManager::initViews()
{
    SPDLOG_TRACE("UiManager::initViews");

    m_authorizationView = new AuthorizationView;
    m_registrationView = new RegistrationView;
}

void UiManager::setupVVMConnections()
{
    SPDLOG_TRACE("UiManager::setupVVMConnections");

    // Authorization
    connect(m_authorizationView, &AuthorizationView::loginAttempted,
            m_authorizationViewModel, &AuthorizationViewModel::requestAuthentication);
    connect(m_authorizationViewModel, &AuthorizationViewModel::loginSuccessful,
            m_authorizationView, &AuthorizationView::onLoginSuccess);
    connect(m_authorizationViewModel, &AuthorizationViewModel::loginFailed,
            m_authorizationView, &AuthorizationView::onLoginFailure);
    connect(m_authorizationView, &AuthorizationView::registrationRequested,
            [this]()
            {
                m_authorizationView->hide();
                m_registrationView->show();
            });

    // Registration
    connect(m_registrationView, &RegistrationView::registrationAttempted,
            m_registrationViewModel, &RegistrationViewModel::attemptRegistration);
    connect(m_registrationViewModel, &RegistrationViewModel::registrationSuccessful,
            m_registrationView, &RegistrationView::onRegistrationSuccess);
    connect(m_registrationViewModel, &RegistrationViewModel::registrationFailed,
            m_registrationView, &RegistrationView::onRegistrationFailure);
    connect(m_registrationView, &RegistrationView::loginRequested,
            [this]()
            {
                m_registrationView->hide();
                m_authorizationView->show();
            });
}
