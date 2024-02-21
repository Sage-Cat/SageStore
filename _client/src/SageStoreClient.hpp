#pragma once

class ApiManager;
class UiManager;

/**
 * @class SageStoreClient
 * @brief Main class for the Sage Store Client.
 */
class SageStoreClient
{
public:
    SageStoreClient(ApiManager &apiManager, UiManager &uiManager);
    ~SageStoreClient();

    // copy removed
    SageStoreClient(const SageStoreClient &) = delete;
    SageStoreClient &operator=(const SageStoreClient &) = delete;

    void start();

private:
    // --- Networking ---
    ApiManager &m_apiManager;

    // --- UI ---
    UiManager &m_uiManager;
};