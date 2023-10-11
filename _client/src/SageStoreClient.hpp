/**
 * @file SageStoreClient.hpp
 * @brief Header file for the SageStoreClient class.
 */

#ifndef SAGESTORECLIENT_HPP
#define SAGESTORECLIENT_HPP

#include <memory>
#include "UI/UIManager.hpp"

/**
 * @class SageStoreClient
 * @brief Main class for the Sage Store Client.
 *
 * This class is responsible for initializing and running the Sage Store Client.
 */
class SageStoreClient
{
public:
    /**
     * @brief Default constructor for SageStoreClient.
     *
     * Initializes the SageStoreClient object.
     */
    SageStoreClient();

    /**
     * @brief Destructor for SageStoreClient.
     *
     * Cleans up any resources used by the SageStoreClient object.
     */
    ~SageStoreClient();

    /**
     * @brief Runs the Sage Store Client.
     *
     * This function initializes and runs the UI manager, and handles the main logic for the client.
     */
    void run();

private:
    /**
     * @brief Unique pointer to the UIManager object.
     *
     * This member is responsible for managing the user interface of the client.
     */
    std::unique_ptr<UIManager> m_uiManager;
};

#endif // SAGESTORECLIENT_HPP
