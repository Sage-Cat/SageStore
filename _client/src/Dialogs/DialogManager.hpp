#pragma once

#include <QString>
#include <QObject>
#include <QMessageBox>

#include "LoginDialog.hpp"
#include "RegistrationDialog.hpp"
#include "Network/ApiManager.hpp"

/**
 * @class DialogManager
 * This class manages all dialogs in client: initializes them
 * and control their behavior
 */
class DialogManager final : public QObject
{
    Q_OBJECT

protected:
public:
    /**
     * @brief Constructor and destructor of DialogManager, which control other objects
     */
    DialogManager(ApiManager *apiManager);
    ~DialogManager();

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
     * @brief setup connections between ApiManager and DialogManager
     */
    void setupApiConnections();

    /**
     * @brief setup connections between Dialogs and DialogManager
     */
    void setupDialogsConnections();

private:
    LoginDialog *m_loginDialog;               ///< responsible for login dialog
    RegistrationDialog *m_registrationDialog; ///< responsible for registration dialog
    ApiManager *m_apiManager;                 ///< pointer for connections
};