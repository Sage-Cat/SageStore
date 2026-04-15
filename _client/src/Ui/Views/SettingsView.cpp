#include "Ui/Views/SettingsView.hpp"

#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include "Settings/AppSettings.hpp"
#include "Ui/Views/TableInteractionHelpers.hpp"

SettingsView::SettingsView(QWidget *parent) : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    auto *card       = new QFrame(this);
    auto *cardLayout = new QVBoxLayout(card);
    auto *header     = new QLabel(tr("Application Settings"), card);
    auto *summary    = new QLabel(
        tr("These values are stored locally for the desktop client. Environment variables still "
           "override saved server connection defaults when they are present."),
        card);
    auto *formLayout = new QFormLayout;

    header->setObjectName("settingsTitleLabel");
    summary->setObjectName("settingsSummaryLabel");
    summary->setProperty("muted", true);
    summary->setWordWrap(true);

    m_languageCombo = new FormComboBox(card);
    configureFormComboBox(m_languageCombo, QStringLiteral("settingsLanguageCombo"));
    m_languageCombo->addItem(tr("English"), "en");
    m_languageCombo->addItem(tr("Ukrainian"), "ua");

    m_schemeField = new QLineEdit(card);
    m_schemeField->setObjectName("settingsSchemeField");

    m_addressField = new QLineEdit(card);
    m_addressField->setObjectName("settingsAddressField");

    m_portSpinBox = new QSpinBox(card);
    m_portSpinBox->setObjectName("settingsPortSpinBox");
    m_portSpinBox->setRange(1, 65535);

    formLayout->addRow(tr("Language"), m_languageCombo);
    formLayout->addRow(tr("Server scheme"), m_schemeField);
    formLayout->addRow(tr("Server address"), m_addressField);
    formLayout->addRow(tr("Server port"), m_portSpinBox);

    auto *actionsLayout = new QHBoxLayout;
    auto *saveButton    = new QPushButton(tr("Save Settings"), card);
    saveButton->setObjectName("settingsSaveButton");

    m_statusLabel = new QLabel(tr("Status: ready"), card);
    m_statusLabel->setObjectName("settingsStatusLabel");
    m_statusLabel->setProperty("muted", true);
    m_statusLabel->setWordWrap(true);

    actionsLayout->addWidget(saveButton);
    actionsLayout->addStretch();

    cardLayout->addWidget(header);
    cardLayout->addWidget(summary);
    cardLayout->addLayout(formLayout);
    cardLayout->addLayout(actionsLayout);
    cardLayout->addWidget(m_statusLabel);
    cardLayout->setSpacing(14);
    cardLayout->setContentsMargins(20, 20, 20, 20);

    mainLayout->addWidget(card);
    mainLayout->addStretch();
    mainLayout->setContentsMargins(18, 18, 18, 18);

    card->setObjectName("settingsCard");
    card->setProperty("card", true);
    setObjectName("settingsView");
    setStyleSheet(
        "#settingsView { background-color: transparent; }"
        "#settingsTitleLabel { color: #0f172a; font-size: 24px; font-weight: 700; }");

    connect(saveButton, &QPushButton::clicked, this, &SettingsView::saveSettings);

    loadCurrentSettings();
}

void SettingsView::saveSettings()
{
    ClientSettings settings;
    settings.language = m_languageCombo->currentData().toString();
    settings.scheme   = m_schemeField->text().trimmed();
    settings.address  = m_addressField->text().trimmed();
    settings.port     = m_portSpinBox->value();

    if (settings.scheme.isEmpty() || settings.address.isEmpty()) {
        m_statusLabel->setText(tr("Status: scheme and address are required."));
        return;
    }

    AppSettings::save(settings);
    m_statusLabel->setText(
        tr("Status: settings saved. Restart the application to apply language or server changes."));
    emit settingsSaved();
}

void SettingsView::loadCurrentSettings()
{
    const auto settings = AppSettings::load();

    const int languageIndex = m_languageCombo->findData(settings.language);
    if (languageIndex >= 0) {
        m_languageCombo->setCurrentIndex(languageIndex);
    }

    m_schemeField->setText(settings.scheme);
    m_addressField->setText(settings.address);
    m_portSpinBox->setValue(settings.port);
}
