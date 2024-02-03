#pragma once

#include <QString>
#include <QMessageBox>

#include "LoginDialog.hpp"
#include "RegistrationDialog.hpp"

/**
 * @class DialogManager
 * Controls all dialogs in MessageBoxes
 */

class DialogManager
{
protected:
public:
    /**
     * @brief Constructor and destructor of DialogManager, which control other objects
     */

    DialogManager();
    virtual ~DialogManager();

    /**
     * @brief Show MessageBox with error message
     * @param message is text of error
     */

    void showErrorMessageBox(QString &message);

    /**
     * @brief Show LoginDialog object
     */

    void showLoginDialog();

    /**
     * @brief Show RegistrationDialog object
     */

    void showRegistrationDialog();

private:
    LoginDialog *m_loginDialog;               /// responsible for login dialog
    RegistrationDialog *m_registrationDialog; /// responsible for registration dialog
};