#include "SageStoreClient.hpp"

SageStoreClient::SageStoreClient()
    : m_uiManager(std::make_unique<UIManager>())
{
}

SageStoreClient::~SageStoreClient()
{
    // Destructor will automatically clean up m_uiManager due to unique_ptr
}

void SageStoreClient::run()
{
    // Initialize and run UIManager here
    m_uiManager->init();

    // Your other logic here
}
