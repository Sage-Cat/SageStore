#include "LoginDialog.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "common/SpdlogConfig.hpp"

LoginDialog::LoginDialog(BaseDialog *parent) : BaseDialog(parent)
{
    SPDLOG_TRACE("LoginDialog::LoginDialog");

    initFields();
    initButtonsAndLinks();
    setupLayout();
    setupConnections();
}

LoginDialog::~LoginDialog()
{
    delete labelLayout;
    delete lineLayout;
    delete inputLayout;
}

void LoginDialog::showWithPresetData(const QString &username, const QString &password)
{
    SPDLOG_TRACE("LoginDialog::showWithPresetData | user: {} | password: {}",
                 username.toStdString(), password.toStdString());
    m_usernameField->setText(username);
    m_passwordField->setText(password);
    show();
}

void LoginDialog::initFields()
{
    SPDLOG_TRACE("LoginDialog::initFields");

    m_usernameField = new QLineEdit(this);
    m_passwordField = new QLineEdit(this);
    m_passwordField->setEchoMode(QLineEdit::Password);
}

void LoginDialog::initButtonsAndLinks()
{
    SPDLOG_TRACE("LoginDialog::initButtonsAndLinks");

    m_loginButton  = new QPushButton(tr("Login"), this);
    m_registerLink = new QLabel(tr("<a href='#'>Register</a>"), this);

    m_registerLink->setTextFormat(Qt::RichText);
    m_registerLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_registerLink->setOpenExternalLinks(false);
}

void LoginDialog::setupLayout()
{
    SPDLOG_TRACE("LoginDialog::setupLayout");

    auto *mainLayout = new QVBoxLayout(this);

    ///> Layout for labels
    QLabel *m_usernameLable = new QLabel(tr("Username"));
    QLabel *m_passwordLable = new QLabel(tr("Password"));

    labelLayout = Utils::createLayout<QVBoxLayout>({m_usernameLable, m_passwordLable});

    ///> Layout for lines
    lineLayout = Utils::createLayout<QVBoxLayout>({m_usernameField, m_passwordField});

    ///> Layout for labels and lines
    inputLayout = Utils::createLayout<QHBoxLayout>({labelLayout, lineLayout});

    mainLayout->addLayout(inputLayout);

    ///> Buttons and links
    mainLayout->addWidget(m_loginButton);

    auto *linkLayout = new QHBoxLayout;
    linkLayout->addWidget(m_registerLink);
    linkLayout->setAlignment(Qt::AlignCenter);

    mainLayout->addLayout(linkLayout);
    setLayout(mainLayout);
}

void LoginDialog::setupConnections()
{
    SPDLOG_TRACE("LoginDialog::setupConnections");

    connect(m_loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(m_registerLink, &QLabel::linkActivated, this, &LoginDialog::onRegisterLinkClicked);
}

void LoginDialog::onLoginClicked()
{
    SPDLOG_TRACE("LoginDialog::onLoginClicked | Username: {}",
                 m_usernameField->text().toStdString());

    emit loginAttempted(
        m_usernameField->text(),
        QCryptographicHash::hash(m_passwordField->text().toUtf8(), QCryptographicHash::Sha256)
            .toHex());
}

void LoginDialog::onRegisterLinkClicked()
{
    SPDLOG_TRACE("LoginDialog::onRegisterLinkClicked");
    emit registrationRequested();
}
