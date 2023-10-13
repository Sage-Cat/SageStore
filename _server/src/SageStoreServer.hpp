#pragma once

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
    // Private members can be added here
};