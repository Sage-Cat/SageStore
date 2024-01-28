#include "SageStoreClient.hpp"

#include <QThread>
#include <QFont>

#include "Network/ConfigManager.hpp"
#include "Network/NetworkService.hpp"
#include "Network/JsonSerializer.hpp"
#include "Network/ApiClient.hpp"

#include "UiManager.hpp"

#include "SpdlogConfig.hpp"

SageStoreClient::SageStoreClient(QApplication &app) : m_app(app)
{
    SPDLOG_TRACE("SageStoreClient::SageStoreClient");

    // Initialize all necessary elements
    init();

    // setup Networking
    setupNetworkService();

    // QApplication settings
    applyAppFont();

    // Log lifecycle ending
    connect(&m_app, &QCoreApplication::aboutToQuit, []()
            { SPDLOG_INFO("SageStoreClient finished with code=0"); });

    // start UI
    m_uiManager->initiateAuthorizationProcess();
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
    m_apiManager = new ApiClient(m_networkService, this);

    // UI
    m_uiManager = new UiManager(m_apiManager, this);
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