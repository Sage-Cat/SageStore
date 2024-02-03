#pragma once

#include <memory>

class BusinessLogicFacade;
class HttpServer;

/**
 * @class SageStoreServer
 * @brief Class for managing the SageStore server.
 *
 * This class encapsulates the functionality needed to run the SageStore server.
 */
class SageStoreServer
{
public:
    /**
     * @brief Default constructor for the SageStoreServer class.
     */
    SageStoreServer();

    /**
     * @brief Destructor for the SageStoreServer class.
     */
    ~SageStoreServer();

    /**
     * @brief Run the SageStore server.
     *
     * This function is used to start and manage the SageStore server.
     */
    void run();

private:
    std::unique_ptr<BusinessLogicFacade> m_businessLogicFacade;
    std::unique_ptr<HttpServer> m_httpServer;
};