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
     * Initializes the SageStoreClient object.
     */
    SageStoreClient(QApplication &app);

    /**
     * @brief Destructor for SageStoreClient.
     *
     * Cleans up any resources used by the SageStoreClient object.
     */
    ~SageStoreClient();

    void init();

private:
    QApplication &m_app;

    /**
     * @brief Unique pointer to the UIManager object.
     *
     * This member is responsible for managing the user interface of the client.
     */
    UIManager m_uiManager;
};