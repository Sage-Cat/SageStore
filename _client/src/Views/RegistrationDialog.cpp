#include "RegistrationDialog.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "SpdlogConfig.hpp"

RegistrationDialog::RegistrationDialog(BaseDialog *parent) : BaseDialog(parent)
{
    SPDLOG_TRACE("RegistrationDialog::RegistrationDialog");

    initFields();
    initButtonsAndLinks();
    setupLayout();
    setupConnections();
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

    m_usernameField = new QLineEdit(this);
    m_passwordField = new QLineEdit(this);
    m_confirmPasswordField = new QLineEdit(this);
    m_passwordField->setEchoMode(QLineEdit::Password);
    m_confirmPasswordField->setEchoMode(QLineEdit::Password);
}

void RegistrationDialog::initButtonsAndLinks()
{
    SPDLOG_TRACE("RegistrationDialog::initButtonsAndLinks");

    m_registerButton = new QPushButton(tr("Register"), this);
    m_loginLink = new QLabel(tr("<a href='#'>Login</a>"), this);
    m_loginLink->setTextFormat(Qt::RichText);
    m_loginLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_loginLink->setOpenExternalLinks(false);
}

void RegistrationDialog::setupLayout()
{
    SPDLOG_TRACE("RegistrationDialog::setupLayout");

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_usernameField);
    mainLayout->addWidget(m_passwordField);
    mainLayout->addWidget(m_confirmPasswordField);
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
    SPDLOG_TRACE("RegistrationDialog::onRegisterClicked | Username: {}", m_usernameField->text().toStdString());
    emit registrationAttempted(m_usernameField->text(), m_passwordField->text(), m_confirmPasswordField->text());
}

void RegistrationDialog::onLoginLinkClicked()
{
    SPDLOG_TRACE("RegistrationDialog::onLoginLinkClicked");
    emit loginRequested();
}
