#include <QComboBox>
#include <QFrame>
#include <QLabel>
#include <QListView>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QStyleOptionComboBox>
#include <QTemporaryDir>
#include <QtTest>

#include "Settings/AppSettings.hpp"
#include "Ui/Views/SettingsView.hpp"

#include "wrappers/TableUiTestHelpers.hpp"

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

    void testLanguageComboPopupUsesDedicatedListViewWithoutDarkOverlay()
    {
        auto *languageCombo = view->findChild<QComboBox *>("settingsLanguageCombo");
        QVERIFY(languageCombo != nullptr);

        QStyleOptionComboBox option;
        option.initFrom(languageCombo);
        option.editable = languageCombo->isEditable();
        QCOMPARE(languageCombo->style()->styleHint(QStyle::SH_ComboBox_Popup, &option,
                                                   languageCombo),
                 0);

        auto *popupView = qobject_cast<QListView *>(languageCombo->view());
        QVERIFY(popupView != nullptr);
        QVERIFY(popupView->property("comboPopup").toBool());
        QCOMPARE(popupView->horizontalScrollBarPolicy(), Qt::ScrollBarAlwaysOff);

        QWidget *popupContainer = TableUiTestHelpers::showComboPopup(languageCombo);
        QVERIFY(popupContainer != nullptr);
        QVERIFY(popupContainer->property("comboPopupContainer").toBool());

        auto *popupFrame = qobject_cast<QFrame *>(popupContainer);
        QVERIFY(popupFrame != nullptr);
        QCOMPARE(popupFrame->frameShape(), QFrame::NoFrame);

        int visibleScrollerCount = 0;
        for (auto *child : popupContainer->findChildren<QWidget *>()) {
            if (QString::fromLatin1(child->metaObject()->className()) ==
                    QStringLiteral("QComboBoxPrivateScroller") &&
                child->isVisible()) {
                ++visibleScrollerCount;
            }
        }
        QCOMPARE(visibleScrollerCount, 0);

        const QImage popupImage = TableUiTestHelpers::grabWidgetImage(popupContainer);
        QVERIFY(!popupImage.isNull());
        QVERIFY(popupImage.width() >= languageCombo->width());
        QVERIFY(popupImage.height() > 8);

        const double darkRatio = TableUiTestHelpers::darkPixelRatio(popupImage);
        QVERIFY2(darkRatio < 0.18,
                 qPrintable(QStringLiteral("Popup image dark ratio too high: %1")
                                .arg(QString::number(darkRatio, 'f', 3))));

        TableUiTestHelpers::hideComboPopup(languageCombo);
    }

private:
    QTemporaryDir tempDir;
    SettingsView *view{nullptr};
};

QTEST_MAIN(SettingsViewTest)
#include "SettingsViewTest.moc"
