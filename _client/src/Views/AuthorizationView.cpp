#include "AuthorizationView.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>

AuthorizationView::AuthorizationView(QWidget *parent) : QDialog(parent)
{
    initFields();
    initButtonsAndLinks();
    setupLayout();
    setupConnections();
}

void AuthorizationView::initFields()
{
    m_usernameField = new QLineEdit(this);
    m_passwordField = new QLineEdit(this);
    m_passwordField->setEchoMode(QLineEdit::Password);
}

void AuthorizationView::initButtonsAndLinks()
{
    m_loginButton = new QPushButton(tr("Login"), this);
    m_forgotPasswordLink = new QLabel(tr("<a href='#'>Forgot Password?</a>"), this);
    m_registerLink = new QLabel(tr("<a href='#'>Register</a>"), this);

    m_forgotPasswordLink->setTextFormat(Qt::RichText);
    m_forgotPasswordLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_forgotPasswordLink->setOpenExternalLinks(false);

    m_registerLink->setTextFormat(Qt::RichText);
    m_registerLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_registerLink->setOpenExternalLinks(false);
}

void AuthorizationView::setupLayout()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_usernameField);
    mainLayout->addWidget(m_passwordField);
    mainLayout->addWidget(m_loginButton);

    auto *linkLayout = new QHBoxLayout;
    linkLayout->addWidget(m_forgotPasswordLink);
    linkLayout->addWidget(m_registerLink);
    linkLayout->setAlignment(Qt::AlignCenter);

    mainLayout->addLayout(linkLayout);
    setLayout(mainLayout);
}

void AuthorizationView::setupConnections()
{
    connect(m_loginButton, &QPushButton::clicked, this, &AuthorizationView::onLoginClicked);
    connect(m_registerLink, &QLabel::linkActivated, this, &AuthorizationView::onRegisterLinkClicked);
}

void AuthorizationView::onLoginClicked()
{
    emit loginAttempted(m_usernameField->text(), m_passwordField->text());
}

void AuthorizationView::onRegisterLinkClicked()
{
    emit registerRequested();
}
