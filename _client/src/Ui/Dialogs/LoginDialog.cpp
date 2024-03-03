#include "LoginDialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include "SpdlogConfig.hpp"

LoginDialog::LoginDialog(BaseDialog *parent) : BaseDialog(parent)
{
    SPDLOG_TRACE("LoginDialog::LoginDialog");

    initFields();
    initButtonsAndLinks();
    setupLayout();
    setupConnections();
}

void LoginDialog::showWithPresetData(const QString &username, const QString &password)
{
    SPDLOG_TRACE("LoginDialog::showWithPresetData | user: {} | password: {}", username.toStdString(), password.toStdString());
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

    m_loginButton = new QPushButton(tr("Login"), this);
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

    auto *labelLayout = Utils::createLayout<QVBoxLayout>(this,
                                                         {m_usernameLable, m_passwordLable});

    ///> Layout for lines
    auto *lineLayout = Utils::createLayout<QVBoxLayout>(this,
                                                        {m_usernameField, m_passwordField});

    ///> Layout for labels and lines
    auto *inputLayout = Utils::createLayout<QHBoxLayout>(this, {labelLayout, lineLayout});

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
    SPDLOG_TRACE("LoginDialog::onLoginClicked | Username: {}", m_usernameField->text().toStdString());

    QByteArray hashedPassword = QCryptographicHash::hash(m_passwordField->text().toUtf8(), QCryptographicHash::Sha256);
    emit loginAttempted(m_usernameField->text(), hashedPassword);
}

void LoginDialog::onRegisterLinkClicked()
{
    SPDLOG_TRACE("LoginDialog::onRegisterLinkClicked");
    emit registrationRequested();
}
