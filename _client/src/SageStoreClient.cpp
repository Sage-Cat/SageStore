#include "SageStoreClient.hpp"

#include <QFont>

#include "SpdlogWrapper.hpp"

#include "UiManager.hpp"

#include "Models/AuthorizationModel.hpp"
#include "Models/RegistrationModel.hpp"

SageStoreClient::SageStoreClient(QApplication &app) : m_app(app), m_uiManager(new UiManager(this))
{
    SPDLOG_TRACE("SageStoreClient::SageStoreClient");

    // QApplication styles
    m_app.setFont(m_uiManager->defaultFont());

    // Lifecycle management
    connect(&m_app, &QCoreApplication::aboutToQuit, []()
            { SPDLOG_INFO("SageStoreClient finished with code=0"); });
}

SageStoreClient::~SageStoreClient()
{
    SPDLOG_TRACE("SageStoreClient::~SageStoreClient");
}

void SageStoreClient::init()
{
    SPDLOG_TRACE("SageStoreClient::init");
    m_uiManager->initUi();
}