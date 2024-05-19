#include "RegistrationDialog.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "common/SpdlogConfig.hpp"

RegistrationDialog::RegistrationDialog(BaseDialog *parent) : BaseDialog(parent)
{
    SPDLOG_TRACE("RegistrationDialog::RegistrationDialog");

    initFields();
    initButtonsAndLinks();
    setupLayout();
    setupConnections();
}

RegistrationDialog::~RegistrationDialog()
{
    delete labelLayout;
    delete lineLayout;
    delete inputLayout;
}

void RegistrationDialog::showWithPresetData(const QString &username, const QString &password)
{
    SPDLOG_TRACE("RegistrationDialog::showWithPresetData | user: {} | password: {}",
                 username.toStdString(), password.toStdString());
    m_usernameField->setText(username);
    m_passwordField->setText(password);
    show();
}

void RegistrationDialog::onRegistrationSuccess()
{
    SPDLOG_TRACE("RegistrationDialog::onRegistrationSuccess");
}

void RegistrationDialog::onRegistrationFailure()
{
    SPDLOG_TRACE("RegistrationDialog::onRegistrationFailure");
}

void RegistrationDialog::initFields()
{
    SPDLOG_TRACE("RegistrationDialog::initFields");

    m_usernameField        = new QLineEdit(this);
    m_passwordField        = new QLineEdit(this);
    m_confirmPasswordField = new QLineEdit(this);
    m_passwordField->setEchoMode(QLineEdit::Password);
    m_confirmPasswordField->setEchoMode(QLineEdit::Password);
}

void RegistrationDialog::initButtonsAndLinks()
{
    SPDLOG_TRACE("RegistrationDialog::initButtonsAndLinks");

    m_registerButton = new QPushButton(tr("Register"), this);
    m_loginLink      = new QLabel(tr("<a href='#'>Login</a>"), this);
    m_loginLink->setTextFormat(Qt::RichText);
    m_loginLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_loginLink->setOpenExternalLinks(false);
}

void RegistrationDialog::setupLayout()
{
    SPDLOG_TRACE("RegistrationDialog::setupLayout");

    auto *mainLayout = new QVBoxLayout(this);

    ///> Layout for labels
    QLabel *m_usernameLable        = new QLabel(tr("Username"));
    QLabel *m_passwordLable        = new QLabel(tr("Password"));
    QLabel *m_confirmPasswordLable = new QLabel(tr("Confirm password"));

    labelLayout = Utils::createLayout<QVBoxLayout>(
        {m_usernameLable, m_passwordLable, m_confirmPasswordLable});

    ///> Layout for lines
    lineLayout = Utils::createLayout<QVBoxLayout>(
        {m_usernameField, m_passwordField, m_confirmPasswordField});

    ///> Layout for labels and lines
    inputLayout = Utils::createLayout<QHBoxLayout>({labelLayout, lineLayout});

    mainLayout->addLayout(inputLayout);

    ///> Buttons and links
    mainLayout->addWidget(m_registerButton);

    auto *linkLayout = new QHBoxLayout;
    linkLayout->addWidget(m_loginLink);
    linkLayout->setAlignment(Qt::AlignCenter);

    mainLayout->addLayout(linkLayout);
    setLayout(mainLayout);
}

void RegistrationDialog::setupConnections()
{
    SPDLOG_TRACE("RegistrationDialog::setupConnections");

    connect(m_registerButton, &QPushButton::clicked, this, &RegistrationDialog::onRegisterClicked);
    connect(m_loginLink, &QLabel::linkActivated, this, &RegistrationDialog::onLoginLinkClicked);
}

void RegistrationDialog::onRegisterClicked()
{
    SPDLOG_TRACE("RegistrationDialog::onRegisterClicked | Username: {}",
                 m_usernameField->text().toStdString());
    if (m_passwordField->text() == m_confirmPasswordField->text())
        emit registrationAttempted(
            m_usernameField->text(),
            QCryptographicHash::hash(m_passwordField->text().toUtf8(), QCryptographicHash::Sha256));
    else
        emit requestErrorMessageBox(tr("Passwords do not match"));
}

void RegistrationDialog::onLoginLinkClicked()
{
    SPDLOG_TRACE("RegistrationDialog::onLoginLinkClicked");
    emit loginRequested();
}

const QString RegistrationDialog::getUsername() { return m_usernameField->text(); }

const QString RegistrationDialog::getPassword() { return m_passwordField->text(); }