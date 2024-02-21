#include "DialogManager.hpp"

DialogManager::DialogManager(ApiManager &apiManager)
    : m_apiManager(apiManager)
{
    initDialogs();
    setupApiConnections();
    setupDialogsConnections();
}

DialogManager::~DialogManager()
{
    delete m_loginDialog;
    delete m_registrationDialog;
}

void DialogManager::initDialogs()
{
    m_loginDialog = new LoginDialog();
    m_registrationDialog = new RegistrationDialog();
}

void DialogManager::setupApiConnections()
{
    // Login
    connect(m_loginDialog, &LoginDialog::loginAttempted,
            &m_apiManager, &ApiManager::loginUser);

    // Registrations
    connect(m_registrationDialog, &RegistrationDialog::registrationAttempted,
            &m_apiManager, &ApiManager::registerUser);
    connect(&m_apiManager, &ApiManager::registerSuccess,
            m_registrationDialog, [this]() { /* TODO: smart login (right after success registration) */ });
}

void DialogManager::setupDialogsConnections()
{
    // Login
    connect(m_loginDialog, &LoginDialog::registrationRequested,
            [this]()
            {
                m_loginDialog->hide();
                m_registrationDialog->show();
            });

    // Registration
    connect(m_registrationDialog, &RegistrationDialog::requestErrorMessageBox, this, showErrorMessageBox);
    connect(m_registrationDialog, &RegistrationDialog::loginRequested,
            [this]()
            {
                m_registrationDialog->hide();
                m_loginDialog->show();
            });
}

void DialogManager::showErrorMessageBox(const QString &message)
{
    QMessageBox::critical(nullptr, tr("Error"), message, QMessageBox::Ok);
}

void DialogManager::showLoginDialog()
{
    m_loginDialog->show();
}

void DialogManager::showRegistrationDialog()
{
    m_registrationDialog->show();
}
