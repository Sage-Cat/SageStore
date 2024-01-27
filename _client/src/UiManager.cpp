#include "UiManager.hpp"

#include <QMessageBox>

#include "SpdlogWrapper.hpp"

#include "ViewModels/AuthorizationDialogModel.hpp"
#include "ViewModels/RegistrationDialogModel.hpp"

#include "ViewModels/PurchaseOrdersViewModel.hpp"
#include "Views/MainWindow.hpp"

#include "Views/AuthorizationDialog.hpp"
#include "Views/RegistrationDialog.hpp"

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

AuthorizationDialogModel *UiManager::authorizationViewModel() const
{
    SPDLOG_TRACE("UiManager::authorizationViewModel");
    return m_authorizationViewModel;
}

RegistrationDialogModel *UiManager::registrationViewModel() const
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

    m_mainWindow = new MainWindow();
}

void UiManager::initViewModels()
{
    SPDLOG_TRACE("UiManager::initViewModels");

    m_authorizationViewModel = new AuthorizationDialogModel;
    m_registrationViewModel = new RegistrationDialogModel;
}

void UiManager::initViews()
{
    SPDLOG_TRACE("UiManager::initViews");

    m_authorizationView = new AuthorizationDialog;
    m_registrationView = new RegistrationDialog;
}

void UiManager::setupVVMConnections()
{
    SPDLOG_TRACE("UiManager::setupVVMConnections");

    // Authorization
    connect(m_authorizationView, &AuthorizationDialog::loginAttempted,
            m_authorizationViewModel, &AuthorizationDialogModel::requestAuthentication);
    connect(m_authorizationViewModel, &AuthorizationDialogModel::loginSuccessful,
            m_authorizationView, &AuthorizationDialog::onLoginSuccess);
    connect(m_authorizationViewModel, &AuthorizationDialogModel::loginFailed,
            m_authorizationView, &AuthorizationDialog::onLoginFailure);
    connect(m_authorizationView, &AuthorizationDialog::registrationRequested,
            [this]()
            {
                m_authorizationView->hide();
                m_registrationView->show();
            });

    // Registration
    connect(m_registrationView, &RegistrationDialog::registrationAttempted,
            m_registrationViewModel, &RegistrationDialogModel::attemptRegistration);
    connect(m_registrationViewModel, &RegistrationDialogModel::registrationSuccessful,
            m_registrationView, &RegistrationDialog::onRegistrationSuccess);
    connect(m_registrationViewModel, &RegistrationDialogModel::registrationFailed,
            m_registrationView, &RegistrationDialog::onRegistrationFailure);
    connect(m_registrationView, &RegistrationDialog::loginRequested,
            [this]()
            {
                m_registrationView->hide();
                m_authorizationView->show();
            });
}
