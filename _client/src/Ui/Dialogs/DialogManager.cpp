#include "DialogManager.hpp"

#include "SpdlogConfig.hpp"

DialogManager::DialogManager(ApiManager &apiManager)
    : m_apiManager(apiManager)
{
    SPDLOG_TRACE("DialogManager::DialogManager");
}

DialogManager::~DialogManager()
{
    SPDLOG_TRACE("DialogManager::~DialogManager");
    delete m_loginDialog;
    delete m_registrationDialog;
    delete m_messageDialog;
    m_messageDialog->addButton(QMessageBox::Ok);
}

void DialogManager::init()
{
    initDialogs();
    setupApiConnections();
    setupDialogsConnections();
}

void DialogManager::showLoginDialog()
{
    SPDLOG_TRACE("DialogManager::showLoginDialog");
    m_loginDialog->showWithPresetData("admin", "admin123");
}

void DialogManager::showRegistrationDialog()
{
    SPDLOG_TRACE("DialogManager::showRegistrationDialog");
    m_registrationDialog->showWithPresetData("newuser", "123");
}

void DialogManager::showErrorDialog(const QString &message)
{
    SPDLOG_TRACE("DialogManager::showErrorDialog");
    showMessage(tr("Error"), message, QMessageBox::Critical);
}

QMessageBox *DialogManager::messageDialog() const
{
    return m_messageDialog;
}

void DialogManager::initDialogs()
{
    SPDLOG_TRACE("DialogManager::initDialogs");
    m_loginDialog = new LoginDialog();
    m_registrationDialog = new RegistrationDialog();
    m_messageDialog = new QMessageBox();
}

void DialogManager::setupApiConnections()
{
    SPDLOG_TRACE("DialogManager::setupApiConnections");
    // Error handling
    connect(&m_apiManager, &ApiManager::errorOccurred,
            this, &DialogManager::showErrorDialog);

    // Login
    connect(m_loginDialog, &LoginDialog::loginAttempted,
            &m_apiManager, &ApiManager::loginUser);

    // Registrations
    connect(m_registrationDialog, &RegistrationDialog::registrationAttempted,
            &m_apiManager, &ApiManager::registerUser);
    connect(&m_apiManager, &ApiManager::registrationSuccess,
            m_registrationDialog, [this]() { /* TODO: smart login (right after success registration) */ });
}

void DialogManager::setupDialogsConnections()
{
    SPDLOG_TRACE("DialogManager::setupDialogsConnections");
    // Login
    connect(m_loginDialog, &LoginDialog::registrationRequested,
            [this]()
            {
                m_loginDialog->hide();
                m_registrationDialog->show();
            });

    // Registration
    connect(m_registrationDialog, &RegistrationDialog::requestErrorMessageBox, this, showErrorDialog);
    connect(m_registrationDialog, &RegistrationDialog::loginRequested,
            [this]()
            {
                m_registrationDialog->hide();
                m_loginDialog->show();
            });
}

void DialogManager::showMessage(const QString &title, const QString &message, QMessageBox::Icon type)
{
    SPDLOG_TRACE("DialogManager::showMessage | title={} | message={}", title.toStdString(), message.toStdString());
    m_messageDialog->setIcon(type);
    m_messageDialog->setWindowTitle(title);
    m_messageDialog->setText(message);
    m_messageDialog->exec();
}