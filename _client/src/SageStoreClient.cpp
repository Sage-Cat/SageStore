#include "SageStoreClient.hpp"

#include "statements.hpp"

SageStoreClient::SageStoreClient(QApplication &app) : m_app(app), m_uiManager(std::make_unique<UIManager>())
{
    SPDLOG_TRACE("SageStoreClient::SageStoreClient");
}

SageStoreClient::~SageStoreClient()
{
    SPDLOG_TRACE("SageStoreClient::~SageStoreClient");
}

void SageStoreClient::init()
{
    SPDLOG_TRACE("SageStoreClient::init");
    m_uiManager->init();
}