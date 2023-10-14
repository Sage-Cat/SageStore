#pragma once

#include <QApplication>

#include <memory>
#include "UI/UIManager.hpp"

/**
 * @class SageStoreClient
 * @brief Main class for the Sage Store Client.
 *
 * This class is responsible for initializing and running the Sage Store Client.
 */
class SageStoreClient : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Default constructor for SageStoreClient.
     *
     * Initializes the SageStoreClient object with a reference to a QApplication object
     *
     * @param app A reference to the QApplication object responsible for the application lifecycle.
     */
    SageStoreClient(QApplication &app);

    /**
     * @brief Deleted copy constructor.
     */
    SageStoreClient(const SageStoreClient &) = delete;

    /**
     * @brief Deleted copy assignment operator.
     */
    SageStoreClient &operator=(const SageStoreClient &) = delete;

    /**
     * @brief Destructor for SageStoreClient.
     *
     * Cleans up any resources used by the SageStoreClient object.
     */
    ~SageStoreClient();

    /**
     * @brief Initialize the SageStore client.
     *
     * This function performs all necessary initialization tasks for the client.
     */
    void init();

private:
    /**
     * @brief Reference to the QApplication object.
     *
     * Holds a reference to the QApplication object to manage application-wide settings.
     */
    QApplication &m_app;

    /**
     * @brief UIManager object initialized on stack.
     *
     * This member is responsible for managing the user interface of the client.
     */
    UIManager m_uiManager;
};