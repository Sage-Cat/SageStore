#include "SageStoreClient.hpp"

#include <QThread>
#include <QFont>

#include "Network\ApiManager.hpp"

#include "UiManager.hpp"

#include "SpdlogConfig.hpp"

SageStoreClient::SageStoreClient(QApplication &app) : m_app(app)
{
    SPDLOG_TRACE("SageStoreClient::SageStoreClient");

    // Initialize all necessary elements
    init();
    m_apiManager->moveToThread(m_apiManagerThread);
    m_apiManagerThread->start();

    // QApplication settings
    applyAppFont();

    // Log lifecycle ending
    connect(&m_app, &QCoreApplication::aboutToQuit, []()
            { SPDLOG_INFO("SageStoreClient finished with code=0"); });

    // start UI
    m_uiManager->startUiProcess();
}

SageStoreClient::~SageStoreClient()
{
    m_apiManagerThread->quit();
    m_apiManagerThread->wait();
    delete m_apiManager;

    SPDLOG_TRACE("SageStoreClient::~SageStoreClient");
}

void SageStoreClient::init()
{
    SPDLOG_TRACE("SageStoreClient::init");

    // Network
    m_apiManager = new ApiManager();
    m_apiManagerThread = new QThread(this);

    // UI
    m_uiManager = new UiManager(m_apiManager, this);
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
    m_uiManager->showErrorMessageBox(tr(errorMessage.toLocal8Bit()));
}