#include "MainWindow.hpp"

#include <QAction>
#include <QMenu>
#include <QScreen>
#include <QStyleFactory>
#include <QVBoxLayout>

#include "Ui/Dialogs/DialogManager.hpp"
#include "Ui/MainMenuActions.hpp"
#include "Ui/UiScale.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
const QSize BASE_WINDOW_SIZE{1200, 800};
}

MainWindow::MainWindow(QApplication &app, ApiManager &apiClient, DialogManager &dialogManager,
                       QWidget *parent)
    : QMainWindow(parent), m_app(app), m_apiManager(apiClient), m_dialogManager(dialogManager),
      m_tabWidget(new QTabWidget(this)), m_statusBar(new QStatusBar(this))
{
    if (QStyleFactory::keys().contains(QStringLiteral("Fusion"))) {
        m_app.setStyle(QStringLiteral("Fusion"));
    }
    setupUi();
    setupMenu();
    setupMVVM();
}

void MainWindow::startUiProcess()
{
    show();
    raise();
    activateWindow();
    m_dialogManager.showLoginDialog();
}

void MainWindow::setupUi()
{
    setObjectName("mainWindow");
    const QScreen *primaryScreen = m_app.primaryScreen();
    const QRect availableGeometry =
        primaryScreen ? primaryScreen->availableGeometry() : QRect();

    resize(UiScale::scaledWindowSize(BASE_WINDOW_SIZE, availableGeometry));
    setMinimumSize(UiScale::scalePx(960), UiScale::scalePx(640));

    if (availableGeometry.isValid()) {
        const QPoint centeredPosition =
            availableGeometry.center() - QPoint(width() / 2, height() / 2);
        move(centeredPosition);
    }

    m_mainMenuBar = menuBar();
    connect(this, &MainWindow::menuActionTriggered, this, &MainWindow::handleMainMenuAction);

    m_tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_tabWidget->setObjectName("mainWindowTabs");
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setDocumentMode(true);
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, m_tabWidget, &QTabWidget::removeTab);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_tabWidget);
    layout->addWidget(m_statusBar, 0, Qt::AlignBottom);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
    setStatusBar(m_statusBar);

    setStyleSheet(R"(
        QMainWindow {
            background-color: #f4faf6;
            color: #0f172a;
        }
        QWidget {
            color: #0f172a;
            font-size: 14px;
        }
        QMenuBar {
            background-color: #ffffff;
            color: #0f172a;
            border-bottom: 1px solid #d7e6dc;
            padding: 6px 10px;
        }
        QMenuBar::item {
            padding: 8px 14px;
            margin: 2px 4px;
            background: transparent;
            border-radius: 10px;
            font-weight: 600;
        }
        QMenuBar::item:selected {
            background: #e8f7ed;
            color: #166534;
        }
        QMenu {
            background-color: #ffffff;
            border: 1px solid #d7e6dc;
            border-radius: 12px;
            padding: 6px;
        }
        QMenu::item {
            padding: 8px 12px;
            border-radius: 8px;
        }
        QMenu::item:selected {
            background-color: #ecfdf3;
            color: #166534;
        }
        QTabWidget::pane {
            border: 1px solid #d7e6dc;
            background-color: #ffffff;
            border-radius: 16px;
            top: -1px;
        }
        QTabBar::tab {
            background: #e7efe9;
            color: #4b6353;
            padding: 10px 16px;
            margin-right: 6px;
            border-top-left-radius: 12px;
            border-top-right-radius: 12px;
            font-weight: 600;
        }
        QTabBar::tab:selected {
            background: #ffffff;
            color: #166534;
        }
        QTabBar::tab:hover:!selected {
            background: #dbeee0;
        }
        QStatusBar {
            background: #ffffff;
            color: #64748b;
            border-top: 1px solid #d7e6dc;
        }
        QFrame[card='true'], QWidget[card='true'] {
            background-color: #ffffff;
            border: 1px solid #d7e6dc;
            border-radius: 18px;
        }
        QWidget[metricCard='true'] {
            background-color: #ffffff;
            border: 1px solid #d7e6dc;
            border-radius: 18px;
        }
        QLabel[muted='true'] {
            color: #64748b;
        }
        QLabel[sectionTitle='true'] {
            color: #244235;
            font-weight: 700;
        }
        QLabel[metricCaption='true'] {
            color: #64748b;
            font-size: 13px;
            font-weight: 600;
        }
        QPushButton {
            background-color: #16a34a;
            color: #ffffff;
            border: 0;
            border-radius: 11px;
            padding: 8px 15px;
            font-weight: 600;
        }
        QPushButton:hover {
            background-color: #15803d;
        }
        QPushButton:pressed {
            background-color: #166534;
        }
        QPushButton[destructive='true'],
        QPushButton#baseDeleteButton {
            background-color: #dc2626;
        }
        QPushButton[destructive='true']:hover,
        QPushButton#baseDeleteButton:hover {
            background-color: #b91c1c;
        }
        QPushButton:disabled {
            background-color: #cdd9d1;
            color: #ffffff;
        }
        QLineEdit, QComboBox, QDateEdit, QSpinBox, QDoubleSpinBox, QPlainTextEdit {
            background-color: #ffffff;
            border: 1px solid #d1e4d7;
            border-radius: 10px;
            padding: 7px 10px;
            selection-background-color: #dcfce7;
            selection-color: #0f172a;
        }
        QLineEdit:focus, QComboBox:focus, QDateEdit:focus, QSpinBox:focus,
        QDoubleSpinBox:focus, QPlainTextEdit:focus {
            border: 1px solid #22c55e;
        }
        QComboBox {
            min-height: 26px;
            padding: 4px 26px 4px 10px;
        }
        QComboBox QAbstractItemView {
            background: #ffffff;
            border: 1px solid #d1e4d7;
            border-radius: 10px;
            selection-background-color: #dcfce7;
            selection-color: #166534;
            outline: 0;
            padding: 2px;
        }
        QComboBox QAbstractItemView::item {
            min-height: 24px;
            padding: 4px 10px;
            margin: 1px 2px;
            border-radius: 6px;
        }
        QTableWidget {
            background-color: #ffffff;
            alternate-background-color: #f7fbf8;
            border: 1px solid #d7e6dc;
            border-radius: 14px;
            gridline-color: #e5efe8;
            selection-background-color: #dcfce7;
            selection-color: #0f172a;
        }
        QTableWidget::item {
            padding: 6px;
        }
        QTableWidget::item:selected {
            color: #14532d;
        }
        QTableWidget QLineEdit[tableEditor='true'],
        QTableWidget QAbstractSpinBox[tableEditor='true'] {
            margin: 0;
            min-height: 0px;
            padding: 0 8px;
            border: 1px solid transparent;
            border-radius: 8px;
            background: #f8fffa;
            selection-background-color: #bbf7d0;
            selection-color: #0f172a;
        }
        QTableWidget QLineEdit[tableEditor='true']:focus,
        QTableWidget QAbstractSpinBox[tableEditor='true']:focus {
            border: 1px solid #22c55e;
            background: #ffffff;
        }
        QTableWidget QAbstractSpinBox[tableEditor='true'] QLineEdit {
            margin: 0;
            padding: 0 6px;
            border: 0;
            border-radius: 0;
            background: transparent;
        }
        QTableWidget QComboBox[tableEditor='true'] {
            margin: 0;
            min-height: 0px;
            padding: 0 22px 0 8px;
            border: 1px solid #d7e6dc;
            border-radius: 8px;
            background: #f8fffa;
            selection-background-color: #bbf7d0;
            selection-color: #0f172a;
        }
        QTableWidget QComboBox[tableEditor='true']:focus,
        QTableWidget QComboBox[tableEditor='true']:on {
            border: 1px solid #22c55e;
            background: #ffffff;
        }
        QTableWidget QComboBox[tableEditor='true'] QAbstractItemView {
            background: #ffffff;
            border: 1px solid #d7e6dc;
            border-radius: 8px;
            selection-background-color: #dcfce7;
            selection-color: #166534;
            outline: 0;
            padding: 2px;
        }
        QAbstractItemView[tableComboPopup='true'] {
            background: #ffffff;
            border: 1px solid #d7e6dc;
            border-radius: 8px;
            selection-background-color: #dcfce7;
            selection-color: #166534;
            outline: 0;
            padding: 2px;
        }
        QFrame[tableComboPopupContainer='true'] {
            background: #ffffff;
            border: 0;
            margin: 0;
            padding: 0;
        }
        QAbstractItemView[tableComboPopup='true']::item {
            min-height: 24px;
            padding: 4px 10px;
            margin: 1px 2px;
            border-radius: 6px;
        }
        QHeaderView::section {
            background-color: #f2f8f3;
            color: #244235;
            padding: 8px;
            border: 0;
            border-bottom: 1px solid #d7e6dc;
            font-weight: 700;
        }
        QTableCornerButton::section {
            background-color: #f2f8f3;
            border: 0;
            border-bottom: 1px solid #d7e6dc;
            border-right: 1px solid #d7e6dc;
        }
        QProgressBar {
            border: 1px solid #d7e6dc;
            border-radius: 999px;
            background-color: #edf4ef;
            text-align: center;
            min-height: 18px;
        }
        QProgressBar::chunk {
            background-color: #16a34a;
            border-radius: 999px;
        }
    )");
}

void MainWindow::setupMenu()
{
    m_mainMenuBar->addMenu(
        createModuleMenu(tr("File"), {MainMenuActions::Type::SETTINGS, MainMenuActions::Type::EXIT}));
    m_mainMenuBar->addMenu(
        createModuleMenu(tr("Purchasing"), {MainMenuActions::Type::PURCHASE_ORDERS,
                                            MainMenuActions::Type::SUPPLIER_MANAGEMENT,
                                            MainMenuActions::Type::GOODS_RECEIPTS}));
    m_mainMenuBar->addMenu(
        createModuleMenu(tr("Sales"), {MainMenuActions::Type::SALES_ORDERS,
                                       MainMenuActions::Type::CUSTOMER_MANAGEMENT,
                                       MainMenuActions::Type::INVOICING}));
    m_mainMenuBar->addMenu(
        createModuleMenu(tr("Inventory"), {MainMenuActions::Type::PRODUCT_MANAGEMENT,
                                           MainMenuActions::Type::SUPPLIER_PRICELIST_UPLOAD,
                                           MainMenuActions::Type::STOCK_TRACKING}));
    m_mainMenuBar->addMenu(
        createModuleMenu(tr("Analytics"), {MainMenuActions::Type::SALES_ANALYTICS,
                                           MainMenuActions::Type::INVENTORY_ANALYTICS}));
    m_mainMenuBar->addMenu(createModuleMenu(tr("Users"), {MainMenuActions::Type::USERS,
                                                          MainMenuActions::Type::USER_ROLES,
                                                          MainMenuActions::Type::USER_LOGS}));
    m_mainMenuBar->addMenu(createModuleMenu(tr("Management"), {MainMenuActions::Type::EMPLOYEES,
                                                               MainMenuActions::Type::CUSTOMERS,
                                                               MainMenuActions::Type::SUPPLIERS}));
}

void MainWindow::handleMainMenuAction(MainMenuActions::Type actionType)
{
    SPDLOG_TRACE("MainWindow::handleMainMenuAction | actionType = {}",
                 MainMenuActions::displayName(actionType).toStdString());

    const auto openTab = [this](QWidget *widget, const QString &title) {
        auto tabIndex = m_tabWidget->indexOf(widget);
        const bool willOpenTab = tabIndex == -1;
        if (tabIndex == -1) {
            tabIndex = m_tabWidget->addTab(widget, title);
        } else {
            m_tabWidget->setTabText(tabIndex, title);
        }
        m_tabWidget->setCurrentIndex(tabIndex);
        return willOpenTab;
    };

    switch (actionType) {
    case MainMenuActions::Type::SETTINGS: {
        auto *view = ensureSettingsView();
        openTab(view, MainMenuActions::displayName(actionType));
        break;
    }
    case MainMenuActions::Type::PURCHASE_ORDERS:
    case MainMenuActions::Type::GOODS_RECEIPTS: {
        auto *view = ensurePurchaseView();
        view->showSection(actionType == MainMenuActions::Type::GOODS_RECEIPTS
                              ? PurchaseView::Section::Receipts
                              : PurchaseView::Section::Orders);
        if (openTab(view, tr("Purchasing"))) {
            m_purchaseViewModel->fetchAll();
        }
        break;
    }
    case MainMenuActions::Type::SUPPLIER_MANAGEMENT:
    case MainMenuActions::Type::CUSTOMER_MANAGEMENT:
    case MainMenuActions::Type::EMPLOYEES:
    case MainMenuActions::Type::CUSTOMERS:
    case MainMenuActions::Type::SUPPLIERS: {
        auto *view = ensureManagementView();
        if (actionType == MainMenuActions::Type::SUPPLIER_MANAGEMENT ||
            actionType == MainMenuActions::Type::SUPPLIERS) {
            view->showSection(ManagementView::Section::Suppliers);
        } else if (actionType == MainMenuActions::Type::EMPLOYEES) {
            view->showSection(ManagementView::Section::Employees);
        } else {
            view->showSection(ManagementView::Section::Contacts);
        }

        if (openTab(view, tr("Management"))) {
            m_managementViewModel->fetchContacts();
            m_managementViewModel->fetchSuppliers();
            m_managementViewModel->fetchEmployees();
        }
        break;
    }
    case MainMenuActions::Type::SALES_ORDERS:
    case MainMenuActions::Type::INVOICING: {
        auto *view = ensureSalesView();
        view->showSection(actionType == MainMenuActions::Type::INVOICING
                              ? SalesView::Section::Invoices
                              : SalesView::Section::Orders);
        if (openTab(view, tr("Sales"))) {
            m_salesViewModel->fetchAll();
        }
        break;
    }
    case MainMenuActions::Type::PRODUCT_MANAGEMENT: {
        auto *view = ensureProductTypesView();
        if (openTab(view, MainMenuActions::displayName(actionType))) {
            m_productTypesViewModel->fetchProductTypes();
        }
        break;
    }
    case MainMenuActions::Type::SUPPLIER_PRICELIST_UPLOAD: {
        auto *view = ensureSupplierCatalogView();
        if (openTab(view, MainMenuActions::displayName(actionType))) {
            m_supplierCatalogViewModel->fetchAll();
        }
        break;
    }
    case MainMenuActions::Type::STOCK_TRACKING: {
        auto *view = ensureStocksView();
        if (openTab(view, MainMenuActions::displayName(actionType))) {
            m_stocksViewModel->fetchProductTypes();
            m_stocksViewModel->fetchStocks();
        }
        break;
    }
    case MainMenuActions::Type::SALES_ANALYTICS:
    case MainMenuActions::Type::INVENTORY_ANALYTICS: {
        auto *view = ensureAnalyticsView();
        view->showSection(actionType == MainMenuActions::Type::SALES_ANALYTICS
                              ? AnalyticsView::Section::Sales
                              : AnalyticsView::Section::Inventory);
        if (openTab(view, tr("Analytics"))) {
            m_analyticsViewModel->fetchAll();
        }
        break;
    }
    case MainMenuActions::Type::USERS: {
        auto *view = ensureUsersView();
        if (openTab(view, MainMenuActions::displayName(actionType))) {
            m_usersManagementViewModel->fetchRoles();
            m_usersManagementViewModel->fetchUsers();
        }
        break;
    }
    case MainMenuActions::Type::USER_ROLES: {
        auto *view = ensureRolesView();
        if (openTab(view, MainMenuActions::displayName(actionType))) {
            m_usersManagementViewModel->fetchRoles();
        }
        break;
    }
    case MainMenuActions::Type::USER_LOGS: {
        auto *view = ensureLogsView();
        if (openTab(view, MainMenuActions::displayName(actionType))) {
            m_logsViewModel->fetchAll();
        }
        break;
    }
    case MainMenuActions::Type::EXIT:
        close();
        break;
    }
}

QMenu *MainWindow::createModuleMenu(const QString &menuTitle,
                                    const QVector<MainMenuActions::Type> &actions)
{
    auto menu = new QMenu(menuTitle, this);
    for (auto actionType : actions) {
        auto action = new QAction(MainMenuActions::displayName(actionType), menu);
        action->setObjectName(MainMenuActions::objectName(actionType));
        connect(action, &QAction::triggered,
                [this, actionType]() { emit menuActionTriggered(actionType); });
        menu->addAction(action);
    }
    return menu;
}

void MainWindow::setupMVVM()
{
    // Views are created lazily when the corresponding tab is first opened.
}

void MainWindow::ensureUsersManagementState()
{
    if (m_usersManagementModel == nullptr) {
        m_usersManagementModel = new UsersManagementModel(m_apiManager, this);
    }

    if (m_usersManagementViewModel == nullptr) {
        m_usersManagementViewModel = new UsersManagementViewModel(*m_usersManagementModel, this);
        connect(m_usersManagementViewModel, &UsersManagementViewModel::errorOccurred,
                &m_dialogManager, &DialogManager::showErrorDialog);
    }
}

ProductTypesView *MainWindow::ensureProductTypesView()
{
    if (m_productTypesView != nullptr) {
        return m_productTypesView;
    }

    m_productTypesModel     = new ProductTypesModel(m_apiManager, this);
    m_productTypesViewModel = new ProductTypesViewModel(*m_productTypesModel, this);
    m_productTypesView      = new ProductTypesView(*m_productTypesViewModel, this);
    connect(m_productTypesViewModel, &ProductTypesViewModel::errorOccurred, &m_dialogManager,
            &DialogManager::showErrorDialog);

    return m_productTypesView;
}

StocksView *MainWindow::ensureStocksView()
{
    if (m_stocksView != nullptr) {
        return m_stocksView;
    }

    m_stocksModel     = new StocksModel(m_apiManager, this);
    m_stocksViewModel = new StocksViewModel(*m_stocksModel, this);
    m_stocksView      = new StocksView(*m_stocksViewModel, this);
    connect(m_stocksViewModel, &StocksViewModel::errorOccurred, &m_dialogManager,
            &DialogManager::showErrorDialog);

    return m_stocksView;
}

UsersView *MainWindow::ensureUsersView()
{
    ensureUsersManagementState();

    if (m_usersView != nullptr) {
        return m_usersView;
    }

    m_usersView = new UsersView(*m_usersManagementViewModel, this);
    return m_usersView;
}

RolesView *MainWindow::ensureRolesView()
{
    ensureUsersManagementState();

    if (m_rolesView != nullptr) {
        return m_rolesView;
    }

    m_rolesView = new RolesView(*m_usersManagementViewModel, this);
    return m_rolesView;
}

SettingsView *MainWindow::ensureSettingsView()
{
    if (m_settingsView != nullptr) {
        return m_settingsView;
    }

    m_settingsView = new SettingsView(this);
    connect(m_settingsView, &SettingsView::settingsSaved, this, [this]() {
        m_statusBar->showMessage(
            tr("Settings saved. Restart the application to apply language or server changes."),
            5000);
    });

    return m_settingsView;
}

ManagementView *MainWindow::ensureManagementView()
{
    if (m_managementView != nullptr) {
        return m_managementView;
    }

    m_managementModel = new ManagementModel(m_apiManager, this);
    m_managementViewModel = new ManagementViewModel(*m_managementModel, this);
    m_managementView = new ManagementView(*m_managementViewModel, this);
    connect(m_managementViewModel, &ManagementViewModel::errorOccurred, &m_dialogManager,
            &DialogManager::showErrorDialog);

    return m_managementView;
}

SupplierCatalogView *MainWindow::ensureSupplierCatalogView()
{
    if (m_supplierCatalogView != nullptr) {
        return m_supplierCatalogView;
    }

    m_supplierCatalogModel = new SupplierCatalogModel(m_apiManager, this);
    m_supplierCatalogViewModel = new SupplierCatalogViewModel(*m_supplierCatalogModel, this);
    m_supplierCatalogView = new SupplierCatalogView(*m_supplierCatalogViewModel, this);
    connect(m_supplierCatalogViewModel, &SupplierCatalogViewModel::errorOccurred,
            &m_dialogManager, &DialogManager::showErrorDialog);

    return m_supplierCatalogView;
}

PurchaseView *MainWindow::ensurePurchaseView()
{
    if (m_purchaseView != nullptr) {
        return m_purchaseView;
    }

    m_purchaseModel = new PurchaseModel(m_apiManager, this);
    m_purchaseViewModel = new PurchaseViewModel(*m_purchaseModel, this);
    m_purchaseView = new PurchaseView(*m_purchaseViewModel, this);
    connect(m_purchaseViewModel, &PurchaseViewModel::errorOccurred, &m_dialogManager,
            &DialogManager::showErrorDialog);

    return m_purchaseView;
}

SalesView *MainWindow::ensureSalesView()
{
    if (m_salesView != nullptr) {
        return m_salesView;
    }

    m_salesModel = new SalesModel(m_apiManager, this);
    m_salesViewModel = new SalesViewModel(*m_salesModel, this);
    m_salesView = new SalesView(*m_salesViewModel, this);
    connect(m_salesViewModel, &SalesViewModel::errorOccurred, &m_dialogManager,
            &DialogManager::showErrorDialog);

    return m_salesView;
}

LogsView *MainWindow::ensureLogsView()
{
    if (m_logsView != nullptr) {
        return m_logsView;
    }

    m_logsModel = new LogsModel(m_apiManager, this);
    m_logsViewModel = new LogsViewModel(*m_logsModel, this);
    m_logsView = new LogsView(*m_logsViewModel, this);
    connect(m_logsViewModel, &LogsViewModel::errorOccurred, &m_dialogManager,
            &DialogManager::showErrorDialog);

    return m_logsView;
}

AnalyticsView *MainWindow::ensureAnalyticsView()
{
    if (m_analyticsView != nullptr) {
        return m_analyticsView;
    }

    m_analyticsModel = new AnalyticsModel(m_apiManager, this);
    m_analyticsViewModel = new AnalyticsViewModel(*m_analyticsModel, this);
    m_analyticsView = new AnalyticsView(*m_analyticsViewModel, this);
    connect(m_analyticsViewModel, &AnalyticsViewModel::errorOccurred, &m_dialogManager,
            &DialogManager::showErrorDialog);

    return m_analyticsView;
}
