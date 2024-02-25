#include "SageStoreClient.hpp"

#include "Network/ApiManager.hpp"
#include "Ui/UiManager.hpp"

#include "SpdlogConfig.hpp"

SageStoreClient::SageStoreClient(ApiManager &apiManager, UiManager &uiManager)
    : m_apiManager(apiManager),
      m_uiManager(uiManager)
{
    SPDLOG_TRACE("SageStoreClient::SageStoreClient");
}

SageStoreClient::~SageStoreClient()
{
    SPDLOG_TRACE("SageStoreClient::~SageStoreClient");
}

void SageStoreClient::start()
{
    // start UI
    m_uiManager.startUiProcess();
}