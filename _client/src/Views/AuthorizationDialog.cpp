#include "AuthorizationDialog.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "SpdlogWrapper.hpp"

AuthorizationDialog::AuthorizationDialog(BaseDialog *parent) : BaseDialog(parent)
{
    SPDLOG_TRACE("AuthorizationDialog::AuthorizationDialog");

    initFields();
    initButtonsAndLinks();
    setupLayout();
    setupConnections();
}

void AuthorizationDialog::onLoginSuccess()
{
    SPDLOG_TRACE("AuthorizationDialog::onLoginSuccess");
}

void AuthorizationDialog::onLoginFailure()
{
    SPDLOG_TRACE("AuthorizationDialog::onLoginFailure");
}

void AuthorizationDialog::initFields()
{
    SPDLOG_TRACE("AuthorizationDialog::initFields");

    m_usernameField = new QLineEdit(this);
    m_passwordField = new QLineEdit(this);
    m_passwordField->setEchoMode(QLineEdit::Password);
}

void AuthorizationDialog::initButtonsAndLinks()
{
    SPDLOG_TRACE("AuthorizationDialog::initButtonsAndLinks");

    m_loginButton = new QPushButton(tr("Login"), this);
    m_registerLink = new QLabel(tr("<a href='#'>Register</a>"), this);

    m_registerLink->setTextFormat(Qt::RichText);
    m_registerLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_registerLink->setOpenExternalLinks(false);
}

void AuthorizationDialog::setupLayout()
{
    SPDLOG_TRACE("AuthorizationDialog::setupLayout");

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_usernameField);
    mainLayout->addWidget(m_passwordField);
    mainLayout->addWidget(m_loginButton);

    auto *linkLayout = new QHBoxLayout;
    linkLayout->addWidget(m_registerLink);
    linkLayout->setAlignment(Qt::AlignCenter);

    mainLayout->addLayout(linkLayout);
    setLayout(mainLayout);
}

void AuthorizationDialog::setupConnections()
{
    SPDLOG_TRACE("AuthorizationDialog::setupConnections");

    connect(m_loginButton, &QPushButton::clicked, this, &AuthorizationDialog::onLoginClicked);
    connect(m_registerLink, &QLabel::linkActivated, this, &AuthorizationDialog::onRegisterLinkClicked);
}

void AuthorizationDialog::onLoginClicked()
{
    SPDLOG_TRACE("AuthorizationDialog::onLoginClicked | Username: {}", m_usernameField->text().toStdString());
    emit loginAttempted(m_usernameField->text(), m_passwordField->text());
}

void AuthorizationDialog::onRegisterLinkClicked()
{
    SPDLOG_TRACE("AuthorizationDialog::onRegisterLinkClicked");
    emit registrationRequested();
}
