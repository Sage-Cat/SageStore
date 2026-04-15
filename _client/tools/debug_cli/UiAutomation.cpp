#include "UiAutomation.hpp"

#include <QAbstractButton>
#include <QAction>
#include <QComboBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFileInfo>
#include <QJsonDocument>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QMetaObject>
#include <QPlainTextEdit>
#include <QPointer>
#include <QPushButton>
#include <QSet>
#include <QSignalSpy>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QTimer>
#include <QWidget>
#include <QtTest>

#include <optional>

#include "JsonUtils.hpp"
#include "Network/ApiManager.hpp"
#include "Network/NetworkService.hpp"
#include "Settings/AppSettings.hpp"
#include "TableAutomation.hpp"
#include "Ui/Dialogs/DialogManager.hpp"
#include "Ui/Dialogs/LoginDialog.hpp"
#include "Ui/MainMenuActions.hpp"
#include "Ui/MainWindow.hpp"
#include "Ui/StartupController.hpp"
#include "common/Network/JsonSerializer.hpp"

namespace DebugCli {
namespace {
void pumpEvents(int durationMs = 25)
{
    QCoreApplication::processEvents(QEventLoop::AllEvents, durationMs);
    QTest::qWait(durationMs);
}

QString normalizeMenuName(const QString &menuName)
{
    QString normalized = menuName.trimmed().toUpper();
    normalized.replace(' ', '_');
    normalized.replace('-', '_');
    return normalized;
}

const QList<QPair<QString, MainMenuActions::Type>> &menuActions()
{
    static const QList<QPair<QString, MainMenuActions::Type>> actions = {
        {QStringLiteral("SETTINGS"), MainMenuActions::Type::SETTINGS},
        {QStringLiteral("EXIT"), MainMenuActions::Type::EXIT},
        {QStringLiteral("PURCHASE_ORDERS"), MainMenuActions::Type::PURCHASE_ORDERS},
        {QStringLiteral("SUPPLIER_MANAGEMENT"), MainMenuActions::Type::SUPPLIER_MANAGEMENT},
        {QStringLiteral("GOODS_RECEIPTS"), MainMenuActions::Type::GOODS_RECEIPTS},
        {QStringLiteral("SALES_ORDERS"), MainMenuActions::Type::SALES_ORDERS},
        {QStringLiteral("CUSTOMER_MANAGEMENT"), MainMenuActions::Type::CUSTOMER_MANAGEMENT},
        {QStringLiteral("INVOICING"), MainMenuActions::Type::INVOICING},
        {QStringLiteral("PRODUCT_MANAGEMENT"), MainMenuActions::Type::PRODUCT_MANAGEMENT},
        {QStringLiteral("SUPPLIER_PRICELIST_UPLOAD"),
         MainMenuActions::Type::SUPPLIER_PRICELIST_UPLOAD},
        {QStringLiteral("STOCK_TRACKING"), MainMenuActions::Type::STOCK_TRACKING},
        {QStringLiteral("SALES_ANALYTICS"), MainMenuActions::Type::SALES_ANALYTICS},
        {QStringLiteral("INVENTORY_ANALYTICS"), MainMenuActions::Type::INVENTORY_ANALYTICS},
        {QStringLiteral("USERS"), MainMenuActions::Type::USERS},
        {QStringLiteral("USER_ROLES"), MainMenuActions::Type::USER_ROLES},
        {QStringLiteral("USER_LOGS"), MainMenuActions::Type::USER_LOGS},
        {QStringLiteral("EMPLOYEES"), MainMenuActions::Type::EMPLOYEES},
        {QStringLiteral("CUSTOMERS"), MainMenuActions::Type::CUSTOMERS},
        {QStringLiteral("SUPPLIERS"), MainMenuActions::Type::SUPPLIERS},
    };
    return actions;
}

std::optional<MainMenuActions::Type> menuActionFromName(const QString &menuName)
{
    const QString normalized = normalizeMenuName(menuName);
    for (const auto &[name, action] : menuActions()) {
        if (name == normalized) {
            return action;
        }
    }

    return std::nullopt;
}

template <typename PointerType> QList<PointerType> uniqueMatches(const QList<PointerType> &items)
{
    QList<PointerType> matches;
    QSet<void *> seen;
    for (PointerType item : items) {
        if (item == nullptr) {
            continue;
        }

        void *raw = static_cast<void *>(item);
        if (seen.contains(raw)) {
            continue;
        }

        seen.insert(raw);
        matches.append(item);
    }
    return matches;
}

QList<QWidget *> findWidgetsByObjectName(const QString &objectName)
{
    QList<QWidget *> matches;
    for (QWidget *topLevel : QApplication::topLevelWidgets()) {
        if (topLevel == nullptr) {
            continue;
        }

        if (topLevel->objectName() == objectName) {
            matches.append(topLevel);
        }
        matches.append(topLevel->findChildren<QWidget *>(objectName, Qt::FindChildrenRecursively));
    }

    return uniqueMatches(matches);
}

QList<QAction *> findActionsByObjectName(QWidget *mainWindow, const QString &objectName)
{
    QList<QAction *> matches;
    if (mainWindow != nullptr) {
        matches.append(mainWindow->findChildren<QAction *>(objectName, Qt::FindChildrenRecursively));
    }

    for (QWidget *topLevel : QApplication::topLevelWidgets()) {
        if (topLevel == nullptr || topLevel == mainWindow) {
            continue;
        }
        matches.append(topLevel->findChildren<QAction *>(objectName, Qt::FindChildrenRecursively));
    }

    return uniqueMatches(matches);
}

QString widgetTextValue(QWidget *widget)
{
    if (auto *label = qobject_cast<QLabel *>(widget); label != nullptr) {
        return label->text();
    }
    if (auto *lineEdit = qobject_cast<QLineEdit *>(widget); lineEdit != nullptr) {
        return lineEdit->text();
    }
    if (auto *plainText = qobject_cast<QPlainTextEdit *>(widget); plainText != nullptr) {
        return plainText->toPlainText();
    }
    if (auto *textEdit = qobject_cast<QTextEdit *>(widget); textEdit != nullptr) {
        return textEdit->toPlainText();
    }
    if (auto *button = qobject_cast<QAbstractButton *>(widget); button != nullptr) {
        return button->text();
    }
    if (auto *combo = qobject_cast<QComboBox *>(widget); combo != nullptr) {
        return combo->currentText();
    }
    if (auto *statusBar = qobject_cast<QStatusBar *>(widget); statusBar != nullptr) {
        return statusBar->currentMessage();
    }
    return {};
}

QJsonObject geometryToJson(const QRect &geometry)
{
    return QJsonObject{{QStringLiteral("x"), geometry.x()},
                       {QStringLiteral("y"), geometry.y()},
                       {QStringLiteral("width"), geometry.width()},
                       {QStringLiteral("height"), geometry.height()}};
}

QJsonArray serializeActions(QObject *owner)
{
    QJsonArray array;
    const auto actions = owner->findChildren<QAction *>(QString(), Qt::FindChildrenRecursively);
    for (QAction *action : actions) {
        if (action->objectName().isEmpty()) {
            continue;
        }

        array.append(QJsonObject{{QStringLiteral("objectName"), action->objectName()},
                                 {QStringLiteral("text"), action->text()},
                                 {QStringLiteral("enabled"), action->isEnabled()},
                                 {QStringLiteral("visible"), action->isVisible()}});
    }
    return array;
}

QJsonObject serializeWidget(QWidget *widget)
{
    QJsonObject object{
        {QStringLiteral("objectName"), widget->objectName()},
        {QStringLiteral("className"), widget->metaObject()->className()},
        {QStringLiteral("visible"), widget->isVisible()},
        {QStringLiteral("enabled"), widget->isEnabled()},
        {QStringLiteral("windowTitle"), widget->windowTitle()},
        {QStringLiteral("geometry"), geometryToJson(widget->geometry())},
    };

    const QString textValue = widgetTextValue(widget);
    if (!textValue.isEmpty()) {
        object.insert(QStringLiteral("text"), textValue);
    }

    if (auto *combo = qobject_cast<QComboBox *>(widget); combo != nullptr) {
        QJsonArray items;
        for (int index = 0; index < combo->count(); ++index) {
            items.append(combo->itemText(index));
        }
        object.insert(QStringLiteral("currentIndex"), combo->currentIndex());
        object.insert(QStringLiteral("currentText"), combo->currentText());
        object.insert(QStringLiteral("items"), items);
    }

    if (auto *tabWidget = qobject_cast<QTabWidget *>(widget); tabWidget != nullptr) {
        QJsonArray tabs;
        for (int index = 0; index < tabWidget->count(); ++index) {
            tabs.append(QJsonObject{{QStringLiteral("index"), index},
                                    {QStringLiteral("title"), tabWidget->tabText(index)}});
        }
        object.insert(QStringLiteral("currentIndex"), tabWidget->currentIndex());
        object.insert(QStringLiteral("tabs"), tabs);
    }

    if (auto *table = qobject_cast<QTableWidget *>(widget); table != nullptr) {
        QJsonArray headers;
        for (int column = 0; column < table->columnCount(); ++column) {
            auto *headerItem = table->horizontalHeaderItem(column);
            headers.append(headerItem != nullptr ? headerItem->text() : QString());
        }

        QJsonArray rows;
        for (int row = 0; row < table->rowCount(); ++row) {
            QJsonArray rowValues;
            for (int column = 0; column < table->columnCount(); ++column) {
                auto *item = table->item(row, column);
                rowValues.append(QJsonObject{
                    {QStringLiteral("text"), item != nullptr ? item->text() : QString()},
                    {QStringLiteral("userRole"),
                     item != nullptr ? item->data(Qt::UserRole).toString() : QString()},
                });
            }
            rows.append(rowValues);
        }

        object.insert(QStringLiteral("rowCount"), table->rowCount());
        object.insert(QStringLiteral("columnCount"), table->columnCount());
        object.insert(QStringLiteral("currentRow"), table->currentRow());
        object.insert(QStringLiteral("currentColumn"), table->currentColumn());
        object.insert(QStringLiteral("headers"), headers);
        object.insert(QStringLiteral("rows"), rows);
    }

    if (auto *messageBox = qobject_cast<QMessageBox *>(widget); messageBox != nullptr) {
        object.insert(QStringLiteral("informativeText"), messageBox->informativeText());
        object.insert(QStringLiteral("detailedText"), messageBox->detailedText());
    }

    if (qobject_cast<QMainWindow *>(widget) != nullptr) {
        object.insert(QStringLiteral("actions"), serializeActions(widget));
    }

    QJsonArray children;
    const auto childWidgets = widget->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
    for (QWidget *child : childWidgets) {
        children.append(serializeWidget(child));
    }
    object.insert(QStringLiteral("children"), children);
    return object;
}

QWidget *currentRootWidget(QWidget *mainWindow)
{
    if (mainWindow != nullptr && mainWindow->isVisible()) {
        return mainWindow;
    }
    if (QWidget *modal = QApplication::activeModalWidget(); modal != nullptr && modal->isVisible()) {
        return modal;
    }
    if (QWidget *active = QApplication::activeWindow(); active != nullptr && active->isVisible()) {
        return active;
    }
    for (QWidget *topLevel : QApplication::topLevelWidgets()) {
        if (topLevel != nullptr && topLevel->isVisible()) {
            return topLevel;
        }
    }
    return nullptr;
}
} // namespace

QJsonArray describeMenuActions()
{
    QJsonArray array;
    for (const auto &[name, action] : menuActions()) {
        array.append(QJsonObject{{QStringLiteral("name"), name},
                                 {QStringLiteral("objectName"),
                                  MainMenuActions::objectName(action)}});
    }
    return array;
}

UiAutomation::UiAutomation(QApplication &application) : m_application(application) {}

UiAutomation::~UiAutomation()
{
    m_startupController.reset();
    m_mainWindow.reset();
    m_dialogManager.reset();
    m_apiManager.reset();
    m_networkService.reset();
}

OperationResult UiAutomation::start(const NetworkService::ServerConfig &serverConfig,
                                    const Options &options)
{
    if (isStarted()) {
        return OperationResult::success(info());
    }

    m_options = options;
    AppSettings::ensureApplicationMetadata();

    m_networkService =
        std::make_unique<NetworkService>(serverConfig, std::make_unique<JsonSerializer>());
    m_apiManager = std::make_unique<ApiManager>(*m_networkService);
    m_dialogManager = std::make_unique<DialogManager>(*m_apiManager);
    m_networkService->init();
    m_dialogManager->init();

    if (m_options.autoAcceptDialogs) {
        QObject::connect(m_dialogManager.get(), &DialogManager::loginSucceeded, m_dialogManager.get(),
                         [dialogManager = m_dialogManager.get()]() {
                             QTimer::singleShot(0, dialogManager->messageDialog(),
                                                &QMessageBox::accept);
                         });
    }

    m_startupController = std::make_unique<StartupController>(
        *m_dialogManager, [this]() {
            m_mainWindow =
                std::make_unique<MainWindow>(m_application, *m_apiManager, *m_dialogManager);
            return m_mainWindow.get();
        });
    m_startupController->start();

    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < 5000) {
        for (QWidget *widget : QApplication::topLevelWidgets()) {
            auto *dialog = qobject_cast<LoginDialog *>(widget);
            if (dialog != nullptr && dialog->objectName() == QStringLiteral("loginDialog") &&
                dialog->isVisible()) {
                return OperationResult::success(info());
            }
        }
        pumpEvents();
    }

    return OperationResult::failure(QStringLiteral("timeout"),
                                    QStringLiteral("Login dialog did not become visible"));
}

OperationResult UiAutomation::login(const QString &username, const QString &password, int timeoutMs)
{
    if (!isStarted() || m_dialogManager == nullptr) {
        return OperationResult::failure(QStringLiteral("infra"),
                                        QStringLiteral("Client runtime is not started"));
    }

    LoginDialog *loginDialog = nullptr;
    for (QWidget *widget : QApplication::topLevelWidgets()) {
        auto *dialog = qobject_cast<LoginDialog *>(widget);
        if (dialog != nullptr && dialog->objectName() == QStringLiteral("loginDialog") &&
            dialog->isVisible()) {
            loginDialog = dialog;
            break;
        }
    }

    if (loginDialog == nullptr) {
        if (m_mainWindow != nullptr && m_mainWindow->isVisible()) {
            return OperationResult::success(info());
        }

        return OperationResult::failure(QStringLiteral("selector"),
                                        QStringLiteral("Visible login dialog was not found"));
    }

    auto *usernameField = loginDialog->findChild<QLineEdit *>(QStringLiteral("loginUsernameField"));
    auto *passwordField = loginDialog->findChild<QLineEdit *>(QStringLiteral("loginPasswordField"));
    auto *loginButton =
        loginDialog->findChild<QPushButton *>(QStringLiteral("loginSubmitButton"));
    if (usernameField == nullptr || passwordField == nullptr || loginButton == nullptr) {
        return OperationResult::failure(
            QStringLiteral("selector"),
            QStringLiteral("Login dialog fields/buttons are missing expected object names"));
    }

    usernameField->setFocus(Qt::OtherFocusReason);
    usernameField->clear();
    QTest::keyClicks(usernameField, username);
    passwordField->setFocus(Qt::OtherFocusReason);
    passwordField->clear();
    QTest::keyClicks(passwordField, password);

    QSignalSpy loginSucceededSpy(m_dialogManager.get(), &DialogManager::loginSucceeded);
    QTest::mouseClick(loginButton, Qt::LeftButton);

    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < timeoutMs) {
        if (loginSucceededSpy.count() > 0 && m_mainWindow != nullptr && m_mainWindow->isVisible()) {
            return OperationResult::success(info());
        }

        if (auto *messageDialog = m_dialogManager->messageDialog();
            messageDialog != nullptr && messageDialog->isVisible() &&
            messageDialog->windowTitle() == QStringLiteral("Error")) {
            return OperationResult::failure(QStringLiteral("server_error"), messageDialog->text(),
                                            QJsonObject{{QStringLiteral("dialogText"),
                                                         messageDialog->text()}});
        }

        pumpEvents();
    }

    return OperationResult::failure(QStringLiteral("timeout"),
                                    QStringLiteral("Timed out waiting for login success"));
}

OperationResult UiAutomation::triggerMenu(const QString &menuName)
{
    const auto actionType = menuActionFromName(menuName);
    if (!actionType.has_value()) {
        return OperationResult::failure(QStringLiteral("scenario"),
                                        QStringLiteral("Unknown menu action: %1").arg(menuName));
    }

    return triggerAction(MainMenuActions::objectName(actionType.value()));
}

OperationResult UiAutomation::triggerAction(const QString &objectName)
{
    const QList<QAction *> actions = findActionsByObjectName(m_mainWindow.get(), objectName);
    if (actions.isEmpty()) {
        return OperationResult::failure(
            QStringLiteral("selector"),
            QStringLiteral("QAction not found for objectName '%1'").arg(objectName));
    }
    if (actions.size() > 1) {
        return OperationResult::failure(
            QStringLiteral("selector"),
            QStringLiteral("Multiple QActions matched objectName '%1'").arg(objectName));
    }

    actions.front()->trigger();
    pumpEvents(50);
    return OperationResult::success(QJsonObject{{QStringLiteral("objectName"), objectName}});
}

OperationResult UiAutomation::click(const QString &objectName)
{
    const QList<QWidget *> matches = findWidgetsByObjectName(objectName);
    QList<QWidget *> visibleMatches;
    for (QWidget *widget : matches) {
        if (widget->isVisible()) {
            visibleMatches.append(widget);
        }
    }

    if (visibleMatches.isEmpty()) {
        return OperationResult::failure(
            QStringLiteral("selector"),
            QStringLiteral("Visible widget not found for objectName '%1'").arg(objectName));
    }
    if (visibleMatches.size() > 1) {
        return OperationResult::failure(
            QStringLiteral("selector"),
            QStringLiteral("Multiple visible widgets matched objectName '%1'").arg(objectName));
    }

    QWidget *widget = visibleMatches.front();
    if (!widget->isEnabled()) {
        return OperationResult::failure(QStringLiteral("assertion"),
                                        QStringLiteral("Widget '%1' is disabled").arg(objectName));
    }

    QTest::mouseClick(widget, Qt::LeftButton, Qt::NoModifier, widget->rect().center());
    pumpEvents(50);
    return OperationResult::success(QJsonObject{{QStringLiteral("objectName"), objectName}});
}

OperationResult UiAutomation::setText(const QString &objectName, const QString &text)
{
    const QList<QWidget *> matches = findWidgetsByObjectName(objectName);
    if (matches.size() != 1 || !matches.front()->isVisible()) {
        return OperationResult::failure(
            QStringLiteral("selector"),
            QStringLiteral("Unable to resolve a single visible widget for '%1'").arg(objectName));
    }

    QWidget *widget = matches.front();
    if (auto *lineEdit = qobject_cast<QLineEdit *>(widget); lineEdit != nullptr) {
        lineEdit->setFocus(Qt::OtherFocusReason);
        lineEdit->selectAll();
        QTest::keyClick(lineEdit, Qt::Key_A, Qt::ControlModifier);
        QTest::keyClicks(lineEdit, text);
        QTest::keyClick(lineEdit, Qt::Key_Enter);
        pumpEvents(50);
        return OperationResult::success(
            QJsonObject{{QStringLiteral("objectName"), objectName},
                        {QStringLiteral("text"), lineEdit->text()}});
    }

    if (auto *plainText = qobject_cast<QPlainTextEdit *>(widget); plainText != nullptr) {
        plainText->setPlainText(text);
        pumpEvents(50);
        return OperationResult::success(
            QJsonObject{{QStringLiteral("objectName"), objectName},
                        {QStringLiteral("text"), plainText->toPlainText()}});
    }

    if (auto *textEdit = qobject_cast<QTextEdit *>(widget); textEdit != nullptr) {
        textEdit->setPlainText(text);
        pumpEvents(50);
        return OperationResult::success(
            QJsonObject{{QStringLiteral("objectName"), objectName},
                        {QStringLiteral("text"), textEdit->toPlainText()}});
    }

    return OperationResult::failure(
        QStringLiteral("assertion"),
        QStringLiteral("Widget '%1' does not support text entry").arg(objectName));
}

OperationResult UiAutomation::setComboText(const QString &objectName, const QString &text)
{
    const QList<QWidget *> matches = findWidgetsByObjectName(objectName);
    if (matches.size() != 1 || !matches.front()->isVisible()) {
        return OperationResult::failure(
            QStringLiteral("selector"),
            QStringLiteral("Unable to resolve a single visible combo box for '%1'").arg(objectName));
    }

    auto *combo = qobject_cast<QComboBox *>(matches.front());
    if (combo == nullptr) {
        return OperationResult::failure(QStringLiteral("assertion"),
                                        QStringLiteral("Widget '%1' is not a combo box").arg(objectName));
    }

    const int index = combo->findText(text);
    if (index < 0) {
        return OperationResult::failure(
            QStringLiteral("assertion"),
            QStringLiteral("Value '%1' not found in combo '%2'").arg(text, objectName));
    }

    combo->setCurrentIndex(index);
    QMetaObject::invokeMethod(combo, "activated", Qt::DirectConnection, Q_ARG(int, index));
    pumpEvents(50);
    return OperationResult::success(
        QJsonObject{{QStringLiteral("objectName"), objectName},
                    {QStringLiteral("currentIndex"), combo->currentIndex()},
                    {QStringLiteral("currentText"), combo->currentText()}});
}

OperationResult UiAutomation::selectTableRow(const QString &objectName, int row)
{
    const QList<QWidget *> matches = findWidgetsByObjectName(objectName);
    if (matches.size() != 1 || !matches.front()->isVisible()) {
        return OperationResult::failure(
            QStringLiteral("selector"),
            QStringLiteral("Unable to resolve a single visible table for '%1'").arg(objectName));
    }

    auto *table = qobject_cast<QTableWidget *>(matches.front());
    if (table == nullptr) {
        return OperationResult::failure(QStringLiteral("assertion"),
                                        QStringLiteral("Widget '%1' is not a table").arg(objectName));
    }
    if (row < 0 || row >= table->rowCount()) {
        return OperationResult::failure(
            QStringLiteral("assertion"),
            QStringLiteral("Requested row %1 is out of range for '%2'").arg(row).arg(objectName));
    }

    table->selectRow(row);
    pumpEvents(50);
    return OperationResult::success(QJsonObject{{QStringLiteral("objectName"), objectName},
                                                {QStringLiteral("selectedRow"), table->currentRow()}});
}

OperationResult UiAutomation::editTableCell(const QString &objectName, int row, int column,
                                            const QString &text, bool comboMode)
{
    const QList<QWidget *> matches = findWidgetsByObjectName(objectName);
    if (matches.size() != 1 || !matches.front()->isVisible()) {
        return OperationResult::failure(
            QStringLiteral("selector"),
            QStringLiteral("Unable to resolve a single visible table for '%1'").arg(objectName));
    }

    auto *table = qobject_cast<QTableWidget *>(matches.front());
    if (table == nullptr) {
        return OperationResult::failure(QStringLiteral("assertion"),
                                        QStringLiteral("Widget '%1' is not a table").arg(objectName));
    }
    if (row < 0 || row >= table->rowCount() || column < 0 || column >= table->columnCount()) {
        return OperationResult::failure(
            QStringLiteral("assertion"),
            QStringLiteral("Requested cell (%1, %2) is out of range for '%3'")
                .arg(row)
                .arg(column)
                .arg(objectName));
    }

    OperationResult result = comboMode ? TableAutomation::setComboCellByText(table, row, column, text)
                                       : TableAutomation::editTextCell(table, row, column, text);
    if (!result.ok) {
        return result;
    }

    return OperationResult::success(QJsonObject{{QStringLiteral("objectName"), objectName},
                                                {QStringLiteral("row"), row},
                                                {QStringLiteral("column"), column},
                                                {QStringLiteral("text"), text}});
}

OperationResult UiAutomation::waitVisible(const QString &objectName, int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < timeoutMs) {
        const QList<QWidget *> matches = findWidgetsByObjectName(objectName);
        QList<QWidget *> visibleMatches;
        for (QWidget *widget : matches) {
            if (widget->isVisible()) {
                visibleMatches.append(widget);
            }
        }
        if (visibleMatches.size() == 1) {
            return OperationResult::success(
                QJsonObject{{QStringLiteral("objectName"), objectName},
                            {QStringLiteral("className"),
                             visibleMatches.front()->metaObject()->className()}});
        }
        pumpEvents();
    }

    return OperationResult::failure(
        QStringLiteral("timeout"),
        QStringLiteral("Timed out waiting for visible widget '%1'").arg(objectName));
}

OperationResult UiAutomation::waitText(const QString &objectName, const QString &expectedText,
                                       int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < timeoutMs) {
        const QList<QWidget *> matches = findWidgetsByObjectName(objectName);
        if (matches.size() == 1) {
            const QString actual = widgetTextValue(matches.front());
            if (actual == expectedText) {
                return OperationResult::success(QJsonObject{{QStringLiteral("objectName"), objectName},
                                                            {QStringLiteral("text"), actual}});
            }
        }
        pumpEvents();
    }

    return OperationResult::failure(
        QStringLiteral("timeout"),
        QStringLiteral("Timed out waiting for text '%1' on '%2'").arg(expectedText, objectName));
}

OperationResult UiAutomation::waitTableRows(const QString &objectName, int minRows, int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < timeoutMs) {
        const QList<QWidget *> matches = findWidgetsByObjectName(objectName);
        if (matches.size() == 1) {
            auto *table = qobject_cast<QTableWidget *>(matches.front());
            if (table != nullptr && table->rowCount() >= minRows) {
                return OperationResult::success(
                    QJsonObject{{QStringLiteral("objectName"), objectName},
                                {QStringLiteral("rowCount"), table->rowCount()}});
            }
        }
        pumpEvents();
    }

    return OperationResult::failure(
        QStringLiteral("timeout"),
        QStringLiteral("Timed out waiting for %1 rows on '%2'").arg(minRows).arg(objectName));
}

OperationResult UiAutomation::dumpState(const QString &objectName, const QString &outputPath) const
{
    QWidget *root = nullptr;
    if (objectName.isEmpty()) {
        root = currentRootWidget(m_mainWindow.get());
    } else {
        const QList<QWidget *> matches = findWidgetsByObjectName(objectName);
        if (matches.size() != 1) {
            return OperationResult::failure(
                QStringLiteral("selector"),
                QStringLiteral("Unable to resolve a single widget for state dump '%1'")
                    .arg(objectName));
        }
        root = matches.front();
    }

    if (root == nullptr) {
        return OperationResult::failure(QStringLiteral("infra"),
                                        QStringLiteral("No UI root is available for state dump"));
    }

    QJsonObject state{{QStringLiteral("platform"), m_application.platformName()},
                      {QStringLiteral("selectedRoot"), serializeWidget(root)}};

    QJsonArray topLevels;
    for (QWidget *topLevel : QApplication::topLevelWidgets()) {
        if (topLevel != nullptr && topLevel->isVisible()) {
            topLevels.append(serializeWidget(topLevel));
        }
    }
    state.insert(QStringLiteral("topLevels"), topLevels);

    QJsonObject result{{QStringLiteral("objectName"), root->objectName()}};
    if (!outputPath.isEmpty()) {
        QString errorMessage;
        if (!JsonUtils::writeJsonFile(outputPath, QJsonDocument(state), &errorMessage)) {
            return OperationResult::failure(QStringLiteral("infra"), errorMessage);
        }
        result.insert(QStringLiteral("statePath"), outputPath);
    } else {
        result.insert(QStringLiteral("state"), state);
    }

    return OperationResult::success(result);
}

OperationResult UiAutomation::capture(const QString &objectName, const QString &screenshotPath,
                                      const QString &statePath) const
{
    QWidget *root = nullptr;
    if (objectName.isEmpty()) {
        root = currentRootWidget(m_mainWindow.get());
    } else {
        const QList<QWidget *> matches = findWidgetsByObjectName(objectName);
        if (matches.size() != 1) {
            return OperationResult::failure(
                QStringLiteral("selector"),
                QStringLiteral("Unable to resolve a single widget for capture '%1'").arg(objectName));
        }
        root = matches.front();
    }

    if (root == nullptr || !root->isVisible()) {
        return OperationResult::failure(QStringLiteral("infra"),
                                        QStringLiteral("No visible widget is available for capture"));
    }

    QJsonObject result{
        {QStringLiteral("objectName"), root->objectName()},
        {QStringLiteral("timestampUtc"), QDateTime::currentDateTimeUtc().toString(Qt::ISODate)},
    };

    if (!screenshotPath.isEmpty()) {
        if (!QDir().mkpath(QFileInfo(screenshotPath).absolutePath())) {
            return OperationResult::failure(
                QStringLiteral("infra"),
                QStringLiteral("Unable to create screenshot directory for %1").arg(screenshotPath));
        }

        if (!root->grab().save(screenshotPath)) {
            return OperationResult::failure(
                QStringLiteral("infra"),
                QStringLiteral("Failed to save screenshot to %1").arg(screenshotPath));
        }
        result.insert(QStringLiteral("screenshotPath"), screenshotPath);
    }

    const OperationResult stateResult = dumpState(objectName, statePath);
    if (!stateResult.ok) {
        return stateResult;
    }

    for (auto it = stateResult.result.begin(); it != stateResult.result.end(); ++it) {
        result.insert(it.key(), it.value());
    }

    return OperationResult::success(result);
}

OperationResult UiAutomation::captureFailureArtifacts(const QString &artifactDir, int stepIndex) const
{
    const QString screenshotPath =
        QDir(artifactDir).filePath(QStringLiteral("failure-%1.png").arg(stepIndex));
    const QString statePath =
        QDir(artifactDir).filePath(QStringLiteral("failure-%1.json").arg(stepIndex));
    OperationResult result = capture({}, screenshotPath, statePath);
    if (!result.ok) {
        return result;
    }

    result.result.insert(QStringLiteral("stepIndex"), stepIndex);
    return result;
}

bool UiAutomation::isStarted() const
{
    return m_networkService != nullptr && m_apiManager != nullptr && m_dialogManager != nullptr &&
           m_startupController != nullptr;
}

QJsonObject UiAutomation::info() const
{
    QJsonArray topLevels;
    for (QWidget *topLevel : QApplication::topLevelWidgets()) {
        if (topLevel == nullptr) {
            continue;
        }

        topLevels.append(QJsonObject{{QStringLiteral("objectName"), topLevel->objectName()},
                                     {QStringLiteral("className"), topLevel->metaObject()->className()},
                                     {QStringLiteral("visible"), topLevel->isVisible()}});
    }

    QWidget *root = currentRootWidget(m_mainWindow.get());
    return QJsonObject{
        {QStringLiteral("started"), isStarted()},
        {QStringLiteral("qtPlatform"), m_application.platformName()},
        {QStringLiteral("mainWindowVisible"), m_mainWindow != nullptr && m_mainWindow->isVisible()},
        {QStringLiteral("currentRootObjectName"), root != nullptr ? root->objectName() : QString()},
        {QStringLiteral("topLevels"), topLevels},
    };
}

} // namespace DebugCli
