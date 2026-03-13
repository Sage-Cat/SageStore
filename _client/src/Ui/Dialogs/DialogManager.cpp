#include "DialogManager.hpp"

#include "common/SpdlogConfig.hpp"

DialogManager::DialogManager(ApiManager &apiManager, QObject *parent)
    : QObject(parent), m_apiManager(apiManager)
{
    SPDLOG_TRACE("DialogManager::DialogManager");
}

DialogManager::~DialogManager()
{
    SPDLOG_TRACE("DialogManager::~DialogManager");
    delete m_loginDialog;
    delete m_registrationDialog;
    delete m_messageDialog;
}

void DialogManager::init()
{
    setupDialogs();
    setupDialogConnections();
    setupApiConnections();
}

void DialogManager::showLoginDialog()
{
    SPDLOG_TRACE("DialogManager::showLoginDialog");
    m_registrationDialog->hide();
    m_loginDialog->setWindowModality(Qt::ApplicationModal);
    m_loginDialog->showWithPresetData("admin", "admin123");
    m_loginDialog->activateWindow();
}

void DialogManager::showRegistrationDialog()
{
    SPDLOG_TRACE("DialogManager::showRegistrationDialog");
    m_loginDialog->hide();
    m_registrationDialog->setWindowModality(Qt::ApplicationModal);
    m_registrationDialog->showWithPresetData("newuser", "123");
    m_registrationDialog->activateWindow();
}

void DialogManager::showErrorDialog(const QString &message)
{
    SPDLOG_TRACE("DialogManager::showErrorDialog");
    showMessage(tr("Error"), message, QMessageBox::Critical);
}

QMessageBox *DialogManager::messageDialog() const { return m_messageDialog; }

void DialogManager::setupDialogs()
{
    SPDLOG_TRACE("DialogManager::setupDialogs");
    m_loginDialog        = new LoginDialog();
    m_registrationDialog = new RegistrationDialog();
    m_messageDialog      = new QMessageBox();
}

void DialogManager::setupDialogConnections()
{
    SPDLOG_TRACE("DialogManager::setupDialogConnections");
    // --- Connnections ---
    // Login
    connect(m_loginDialog, &LoginDialog::registrationRequested, this,
            &DialogManager::showRegistrationDialog);
    connect(m_loginDialog, &QDialog::rejected, this, &DialogManager::onLoginDialogRejected);

    // Registration
    connect(m_registrationDialog, &RegistrationDialog::requestErrorMessageBox, this,
            &DialogManager::showErrorDialog);
    connect(m_registrationDialog, &RegistrationDialog::loginRequested, [this]() {
        m_registrationDialog->hide();
        m_loginDialog->show();
        m_loginDialog->activateWindow();
    });
    connect(m_registrationDialog, &QDialog::rejected, this,
            &DialogManager::onRegistrationDialogRejected);
}

void DialogManager::setupApiConnections()
{
    SPDLOG_TRACE("DialogManager::setupApiConnections");
    // Error handling
    connect(&m_apiManager, &ApiManager::errorOccurred, this, &DialogManager::showErrorDialog);

    // Login
    connect(m_loginDialog, &LoginDialog::loginAttempted, &m_apiManager, &ApiManager::loginUser);
    connect(&m_apiManager, &ApiManager::loginSuccess, this, &DialogManager::onLoginSuccess);

    // Registrations
    connect(m_registrationDialog, &RegistrationDialog::registrationAttempted, &m_apiManager,
            &ApiManager::addUser);
    connect(&m_apiManager, &ApiManager::userAdded, this, &DialogManager::onUserAdded);
}

void DialogManager::showMessage(const QString &title, const QString &message,
                                QMessageBox::Icon type)
{
    SPDLOG_TRACE("DialogManager::showMessage | title={} | message={}", title.toStdString(),
                 message.toStdString());
    m_messageDialog->setIcon(type);
    m_messageDialog->setWindowTitle(title);
    m_messageDialog->setText(message);
    m_messageDialog->exec();
}

void DialogManager::onLoginSuccess()
{
    SPDLOG_TRACE("DialogManager::onLoginSuccess");
    m_isAuthenticated = true;
    m_loginDialog->hide();
    m_registrationDialog->hide();
    emit loginSucceeded();
    showMessage(tr("Message"), tr("Login successfull"), QMessageBox::Information);
}

void DialogManager::onUserAdded()
{
    SPDLOG_TRACE("DialogManager::onUserAdded");
    m_registrationDialog->hide();
    m_loginDialog->showWithPresetData(m_registrationDialog->getUsername(),
                                      m_registrationDialog->getPassword());
    showMessage(tr("Message"), tr("Resistrarion successfull"), QMessageBox::Information);
}

void DialogManager::onLoginDialogRejected()
{
    SPDLOG_TRACE("DialogManager::onLoginDialogRejected");
    if (!m_isAuthenticated) {
        emit loginCancelled();
    }
}

void DialogManager::onRegistrationDialogRejected()
{
    SPDLOG_TRACE("DialogManager::onRegistrationDialogRejected");
    if (m_isAuthenticated) {
        return;
    }

    m_registrationDialog->hide();
    m_loginDialog->show();
    m_loginDialog->activateWindow();
}
