#include "UiManager.hpp"

#include "ApiManager.hpp"

#include "Ui/MainWindow.hpp"
#include "Ui/Dialogs/DialogManager.hpp"

#include "SpdlogConfig.hpp"

UiManager::UiManager(QApplication &app, ApiManager &apiClient) noexcept
    : m_app(app),
      m_apiManager(apiClient)
{
    SPDLOG_TRACE("UiManager::UiManager");

    init();
    setupMVVMConnections();
    setupApiConnections();
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

    initModels();
    initViewModels();
    initViews();
}

void UiManager::startUiProcess()
{
    m_dialogManager->showLoginDialog();
}

void UiManager::initModels()
{
    SPDLOG_TRACE("UiManager::initModels");
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

    // Error handling
    connect(&m_apiManager, &ApiManager::errorOccurred,
            m_dialogManager, &DialogManager::showErrorMessageBox);
}

void UiManager::setupMVVMConnections()
{
    SPDLOG_TRACE("UiManager::setupMVVMConnections");
}
