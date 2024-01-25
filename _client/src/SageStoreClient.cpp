#include "SageStoreClient.hpp"

#include <QThread>
#include <QFont>

#include "SpdlogWrapper.hpp"

#include "Network/ConfigManager.hpp"
#include "Network/NetworkService.hpp"
#include "Network/JsonSerializer.hpp"

#include "UiManager.hpp"
#include "Models/AuthorizationModel.hpp"
#include "Models/RegistrationModel.hpp"
#include "ViewModels/AuthorizationViewModel.hpp"
#include "ViewModels/RegistrationViewModel.hpp"

SageStoreClient::SageStoreClient(QApplication &app) : m_app(app)
{
    SPDLOG_TRACE("SageStoreClient::SageStoreClient");

    // Initialize all necessary elements
    init();

    // setup Networking
    setupNetworkService();

    // setup UI
    setupMVMConnections();
    m_uiManager->initiateAuthorizationProcess();

    // QApplication settings
    applyAppFont();

    // Log lifecycle ending
    connect(&m_app, &QCoreApplication::aboutToQuit, []()
            { SPDLOG_INFO("SageStoreClient finished with code=0"); });
}

SageStoreClient::~SageStoreClient()
{
    m_networkServiceThread->quit();
    m_networkServiceThread->wait();
    delete m_networkService;

    SPDLOG_TRACE("SageStoreClient::~SageStoreClient");
}

void SageStoreClient::init()
{
    SPDLOG_TRACE("SageStoreClient::init");

    // Network
    m_configManager = new ConfigManager(std::make_unique<JsonSerializer>(), this); // JSON is used by default
    m_networkService = new NetworkService(this);
    m_networkServiceThread = new QThread(this);

    // UiManager
    m_uiManager = new UiManager(this);

    // Init all Models
    initModels();
}

void SageStoreClient::initModels()
{
    SPDLOG_TRACE("SageStoreClient::initModels");

    m_authorizationModel = new AuthorizationModel;
    m_registrationModel = new RegistrationModel;
}

void SageStoreClient::setupNetworkService()
{
    SPDLOG_TRACE("SageStoreClient::setupNetworkService");

    connect(m_configManager, &ConfigManager::configurationFetched,
            [this](const Config &config)
            {
                std::unique_ptr<IDataSerializer> serializer;
                if (config.serializationType == "json")
                    serializer = std::make_unique<JsonSerializer>();

                this->m_networkService->updateApiUrl(config.apiUrl);
                this->m_networkService->updateSerializer(std::move(serializer));
                this->m_networkService->moveToThread(m_networkServiceThread);
                this->m_networkServiceThread->start();
            });

    connect(m_configManager, &ConfigManager::configurationFetchFailed,
            this, &SageStoreClient::onConfigFetchFailed);

    m_configManager->fetchConfiguration();
}

void SageStoreClient::setupMVMConnections()
{
    SPDLOG_TRACE("SageStoreClient::setupMVMConnections");

    if (m_uiManager)
    {
        // Authorization
        connect(m_uiManager->authorizationViewModel(), &AuthorizationViewModel::requestAuthentication,
                m_authorizationModel, &AuthorizationModel::onAuthenticationRequested);
        connect(m_authorizationModel, &AuthorizationModel::authenticationSuccessful,
                m_uiManager->authorizationViewModel(), &AuthorizationViewModel::loginSuccessful);
        connect(m_authorizationModel, &AuthorizationModel::authenticationFailed,
                m_uiManager->authorizationViewModel(), &AuthorizationViewModel::loginFailed);

        // Registration
        connect(m_uiManager->registrationViewModel(), &RegistrationViewModel::requestRegistration,
                m_registrationModel, &RegistrationModel::onRegistrationRequested);
        connect(m_registrationModel, &RegistrationModel::registrationSuccessful,
                m_uiManager->registrationViewModel(), &RegistrationViewModel::registrationSuccessful);
        connect(m_registrationModel, &RegistrationModel::registrationFailed,
                m_uiManager->registrationViewModel(), &RegistrationViewModel::registrationFailed);
    }
    else
    {
        SPDLOG_ERROR("SageStoreClient::setupMVMConnections m_uiManager is not initialized");
    }
}

void SageStoreClient::applyAppFont()
{
    SPDLOG_TRACE("SageStoreClient::applyAppFont");

    const QFont font = m_uiManager->defaultFont();
    m_app.setFont(font);
    SPDLOG_INFO("Application font set to {}", font.toString().toStdString());
}

void SageStoreClient::onConfigFetchFailed()
{
    const QString errorMessage = "Failed to fetch configuration.";
    SPDLOG_ERROR(errorMessage.toStdString());
    m_uiManager->showErrorMessageBox(errorMessage);
}