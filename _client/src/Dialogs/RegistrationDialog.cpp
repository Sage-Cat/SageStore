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

    ///> Layout for labels
    std::vector<QWidget *> labels;
    labels.push_back(new QLabel(tr("Username")));
    labels.push_back(new QLabel(tr("Password")));
    labels.push_back(new QLabel(tr("Confirm password")));

    auto *labelLayout = Utils::createLayout(labels);

    ///> Layout for lines
    std::vector<QWidget *> lines;
    lines.push_back(m_usernameField);
    lines.push_back(m_passwordField);
    lines.push_back(m_confirmPasswordField);

    auto *lineLayout = Utils::createLayout(lines);

    ///> Layout for labels and lines
    auto *inputLayout = new QHBoxLayout(this);
    inputLayout->addLayout(labelLayout);
    inputLayout->addLayout(lineLayout);

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
    SPDLOG_TRACE("RegistrationDialog::onRegisterClicked | Username: {}", m_usernameField->text().toStdString());

    if (m_passwordField->text() == m_confirmPasswordField->text())
        emit registrationAttempted(m_usernameField->text(), m_passwordField->text());
    else
        emit requestErrorMessageBox(tr("Passwords do not match"));
}

void RegistrationDialog::onLoginLinkClicked()
{
    SPDLOG_TRACE("RegistrationDialog::onLoginLinkClicked");
    emit loginRequested();
}
