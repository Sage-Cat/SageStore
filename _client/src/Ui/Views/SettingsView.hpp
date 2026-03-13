#pragma once

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QWidget>

class SettingsView : public QWidget {
    Q_OBJECT

public:
    explicit SettingsView(QWidget *parent = nullptr);

signals:
    void settingsSaved();

private slots:
    void saveSettings();

private:
    void loadCurrentSettings();

    QComboBox *m_languageCombo{nullptr};
    QLineEdit *m_schemeField{nullptr};
    QLineEdit *m_addressField{nullptr};
    QSpinBox *m_portSpinBox{nullptr};
    QLabel *m_statusLabel{nullptr};
};
