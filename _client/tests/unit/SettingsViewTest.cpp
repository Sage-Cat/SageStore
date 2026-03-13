#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QTemporaryDir>
#include <QtTest>

#include "Settings/AppSettings.hpp"
#include "Ui/Views/SettingsView.hpp"

class SettingsViewTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase()
    {
        QVERIFY(tempDir.isValid());
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, tempDir.path());
        AppSettings::ensureApplicationMetadata();
    }

    void init()
    {
        QSettings settings;
        settings.clear();

        AppSettings::save(ClientSettings{.language = "en",
                                         .scheme = "http",
                                         .address = "127.0.0.1",
                                         .port = 8001});

        view = new SettingsView();
        view->show();
    }

    void cleanup()
    {
        delete view;
        view = nullptr;
        QSettings settings;
        settings.clear();
    }

    void testSaveSettingsPersistsLanguageAndServerDefaults()
    {
        auto *languageCombo = view->findChild<QComboBox *>("settingsLanguageCombo");
        auto *schemeField   = view->findChild<QLineEdit *>("settingsSchemeField");
        auto *addressField  = view->findChild<QLineEdit *>("settingsAddressField");
        auto *portSpinBox   = view->findChild<QSpinBox *>("settingsPortSpinBox");
        auto *saveButton    = view->findChild<QPushButton *>("settingsSaveButton");
        auto *statusLabel   = view->findChild<QLabel *>("settingsStatusLabel");

        QVERIFY(languageCombo != nullptr);
        QVERIFY(schemeField != nullptr);
        QVERIFY(addressField != nullptr);
        QVERIFY(portSpinBox != nullptr);
        QVERIFY(saveButton != nullptr);
        QVERIFY(statusLabel != nullptr);

        languageCombo->setCurrentIndex(languageCombo->findData("ua"));
        schemeField->setText("https");
        addressField->setText("erp.local");
        portSpinBox->setValue(9443);

        QTest::mouseClick(saveButton, Qt::LeftButton);

        const auto savedSettings = AppSettings::load();
        QCOMPARE(savedSettings.language, QString("ua"));
        QCOMPARE(savedSettings.scheme, QString("https"));
        QCOMPARE(savedSettings.address, QString("erp.local"));
        QCOMPARE(savedSettings.port, 9443);
        QVERIFY(statusLabel->text().contains("saved", Qt::CaseInsensitive));
    }

private:
    QTemporaryDir tempDir;
    SettingsView *view{nullptr};
};

QTEST_MAIN(SettingsViewTest)
#include "SettingsViewTest.moc"
