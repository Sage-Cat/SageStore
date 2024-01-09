#include "RegistrationView.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>

RegistrationView::RegistrationView(QWidget *parent) : QDialog(parent)
{
    initFields();
    initButtonsAndLinks();
    setupLayout();
    setupConnections();
}

void RegistrationView::initFields()
{
    m_usernameField = new QLineEdit(this);
    m_passwordField = new QLineEdit(this);
    m_confirmPasswordField = new QLineEdit(this);
    m_passwordField->setEchoMode(QLineEdit::Password);
    m_confirmPasswordField->setEchoMode(QLineEdit::Password);
}

void RegistrationView::initButtonsAndLinks()
{
    m_registerButton = new QPushButton(tr("Register"), this);
    m_loginLink = new QLabel(tr("<a href='#'>Login</a>"), this);
    m_loginLink->setTextFormat(Qt::RichText);
    m_loginLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_loginLink->setOpenExternalLinks(false);
}

void RegistrationView::setupLayout()
{
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

void RegistrationView::setupConnections()
{
    connect(m_registerButton, &QPushButton::clicked, this, &RegistrationView::onRegisterClicked);
    connect(m_loginLink, &QLabel::linkActivated, this, &RegistrationView::onLoginLinkClicked);
}

void RegistrationView::onRegisterClicked()
{
    emit registrationAttempted(m_usernameField->text(), m_passwordField->text(), m_confirmPasswordField->text());
}

void RegistrationView::onLoginLinkClicked()
{
    emit loginRequested();
}
