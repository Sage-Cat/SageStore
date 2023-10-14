#include "SageStoreClient.hpp"

#include <QFont>

#include "logging.hpp"

SageStoreClient::SageStoreClient(QApplication &app) : m_app(app)
{
    SPDLOG_TRACE("SageStoreClient::SageStoreClient");

    // Lifecycle management
    QObject::connect(&m_app, &QCoreApplication::aboutToQuit, []()
                     { SPDLOG_INFO("SageStoreClient finished with code=0"); });

    // QApplication styles
    m_app.setFont(m_uiManager.defaultFont());
}

SageStoreClient::~SageStoreClient()
{
    SPDLOG_TRACE("SageStoreClient::~SageStoreClient");
}

void SageStoreClient::init()
{
    SPDLOG_TRACE("SageStoreClient::init");
    m_uiManager.init();
}