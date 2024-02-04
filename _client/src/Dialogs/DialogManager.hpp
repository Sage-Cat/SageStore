#pragma once

#include <QString>
#include <QObject>
#include <QMessageBox>

#include "LoginDialog.hpp"
#include "RegistrationDialog.hpp"
#include "Network/ApiManager.hpp"

#include "SpdlogConfig.hpp"

/**
 * @class DialogManager
 * Controls all dialogs in MessageBoxes
 */
class DialogManager : public QObject
{
    Q_OBJECT

protected:
public:
    /**
     * @brief Constructor and destructor of DialogManager, which control other objects
     */
    DialogManager(ApiManager *apiManager);
    virtual ~DialogManager();

    /**
     * @brief Show MessageBox with error message
     * @param message is text of error
     */
    void showErrorMessageBox(const QString &message);

    /**
     * @brief Show LoginDialog object
     */
    void showLoginDialog();

    /**
     * @brief Show RegistrationDialog object
     */
    void showRegistrationDialog();

private:
    /**
     * @brief initialization of dialogs
     */
    void initDialogs();

    /**
     * @brief setup connections for ApiManager
     */
    void setupApiConnections();

    /**
     * @brief setup connections for Dialogs
     */
    void setupDialogsConnections();

private:
    LoginDialog *m_loginDialog;               ///< responsible for login dialog
    RegistrationDialog *m_registrationDialog; ///< responsible for registration dialog
    ApiManager *m_apiManager;                 ///< pointer for connections
};