#include "MainWindow.hpp"

#include <QAction>
#include <QMenu>
#include <QScreen>
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
    setupUi();
    setupMenu();
    setupMVVM();
}

void MainWindow::setupUi()
{
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
        QMainWindow { background-color: #f4efe7; }
        QMenuBar { background-color: #34504f; color: #f5efe6; }
        QMenuBar::item { padding: 6px 10px; background: transparent; }
        QMenuBar::item:selected { background: #496a68; border-radius: 4px; }
        QMenu { background-color: #fffaf2; border: 1px solid #d8cdbd; }
        QMenu::item:selected { background-color: #e3d5bd; }
        QTabWidget::pane {
            border: 1px solid #d8cdbd;
            background-color: #fffaf2;
            border-radius: 8px;
            top: -1px;
        }
        QTabBar::tab {
            background: #d9cfbf;
            color: #2f3736;
            padding: 8px 14px;
            margin-right: 4px;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
        }
        QTabBar::tab:selected { background: #fffaf2; }
        QStatusBar { background: #e9dfd1; color: #4a433b; }
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
