#include "DialogManager.hpp"

DialogManager::DialogManager()
{
    m_loginDialog = new LoginDialog();
    m_registrationDialog = new RegistrationDialog();
}

DialogManager::~DialogManager()
{
    delete m_loginDialog;
    delete m_registrationDialog;
}

void DialogManager::showErrorMessageBox(QString &message)
{
    QMessageBox::critical(nullptr, tr("Error"), message, QMessageBox::Ok);
}

void DialogManager::showLoginDialog()
{
}

void DialogManager::showRegistrationDialog()
{
}