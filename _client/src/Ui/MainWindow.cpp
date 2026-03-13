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
} // namespace

MainWindow::MainWindow(QApplication &app, ApiManager &apiClient, DialogManager &dialogManager,
                       QWidget *parent)
    : QMainWindow(parent), m_app(app), m_apiManager(apiClient), m_dialogManager(dialogManager),
      m_tabWidget(new QTabWidget(this)), m_statusBar(new QStatusBar(this))
{
    setupUi();
    setupMenu();
    setupMVVM();
}

void MainWindow::startUiProcess()
{
    m_dialogManager.showLoginDialog();
    show();
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

    // MainMenu
    m_mainMenuBar = menuBar();
    connect(this, &MainWindow::menuActionTriggered, this, &MainWindow::handleMainMenuAction);

    // TabWidget
    m_tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setDocumentMode(true);
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, m_tabWidget, &QTabWidget::removeTab);

    // MainWindow layout
    QVBoxLayout *layout = new QVBoxLayout;

    layout->addWidget(m_tabWidget);
    layout->addWidget(m_statusBar, 0, Qt::AlignBottom);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // StatusBar
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
    // File Menu
    m_mainMenuBar->addMenu(createModuleMenu(
        tr("File"), {MainMenuActions::Type::SETTINGS, MainMenuActions::Type::EXIT}));
    // Purchasing Module Menu
    m_mainMenuBar->addMenu(
        createModuleMenu(tr("Purchasing"), {MainMenuActions::Type::PURCHASE_ORDERS,
                                            MainMenuActions::Type::SUPPLIER_MANAGEMENT,
                                            MainMenuActions::Type::GOODS_RECEIPTS}));
    // Sales Module Menu
    m_mainMenuBar->addMenu(
        createModuleMenu(tr("Sales"), {MainMenuActions::Type::SALES_ORDERS,
                                       MainMenuActions::Type::CUSTOMER_MANAGEMENT,
                                       MainMenuActions::Type::INVOICING}));
    // Inventory Module Menu
    m_mainMenuBar->addMenu(
        createModuleMenu(tr("Inventory"), {MainMenuActions::Type::PRODUCT_MANAGEMENT,
                                           MainMenuActions::Type::SUPPLIER_PRICELIST_UPLOAD,
                                           MainMenuActions::Type::STOCK_TRACKING}));
    // Analytics Module Menu
    m_mainMenuBar->addMenu(
        createModuleMenu(tr("Analytics"), {MainMenuActions::Type::SALES_ANALYTICS,
                                           MainMenuActions::Type::INVENTORY_ANALYTICS}));
    // Users Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Users"), {MainMenuActions::Type::USERS,
                                                          MainMenuActions::Type::USER_ROLES,
                                                          MainMenuActions::Type::USER_LOGS}));
    // Management Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Management"), {MainMenuActions::Type::EMPLOYEES,
                                                               MainMenuActions::Type::CUSTOMERS,
                                                               MainMenuActions::Type::SUPPLIERS}));
}

void MainWindow::handleMainMenuAction(MainMenuActions::Type actionType)
{
    SPDLOG_TRACE("MainWindow::setupMVVMConnections | actionType = {}",
                 MainMenuActions::NAMES.contains(actionType)
                     ? MainMenuActions::NAMES.at(actionType).toStdString()
                     : std::to_string(static_cast<int>(actionType)));

    const auto openTab = [this](QWidget *widget, MainMenuActions::Type type) {
        const auto title = MainMenuActions::NAMES.at(type);
        auto tabIndex    = m_tabWidget->indexOf(widget);
        const bool willOpenTab = tabIndex == -1;
        if (tabIndex == -1) {
            tabIndex = m_tabWidget->addTab(widget, title);
        }
        m_tabWidget->setCurrentIndex(tabIndex);
        return willOpenTab;
    };

    // Performing action based on actionType
    switch (actionType) {
    case MainMenuActions::Type::PRODUCT_MANAGEMENT: {
        auto *view = ensureProductTypesView();
        if (openTab(view, MainMenuActions::Type::PRODUCT_MANAGEMENT)) {
            m_productTypesViewModel->fetchProductTypes();
        }
        break;
    }
    case MainMenuActions::Type::STOCK_TRACKING: {
        auto *view = ensureStocksView();
        if (openTab(view, MainMenuActions::Type::STOCK_TRACKING)) {
            m_stocksViewModel->fetchProductTypes();
            m_stocksViewModel->fetchStocks();
        }
        break;
    }
    case MainMenuActions::Type::USERS: {
        auto *view = ensureUsersView();
        if (openTab(view, MainMenuActions::Type::USERS)) {
            m_usersManagementViewModel->fetchRoles();
            m_usersManagementViewModel->fetchUsers();
        }
        break;
    }
    case MainMenuActions::Type::EXIT:
        close();
        break;
    default:
        SPDLOG_WARN("Unsupported MainMenuAction type.");
    }
}

QMenu *MainWindow::createModuleMenu(const QString &menuTitle,
                                    const QVector<MainMenuActions::Type> &actions)
{
    auto menu = new QMenu(menuTitle, this);
    for (auto actionType : actions) {
        auto action = new QAction(MainMenuActions::NAMES.at(actionType), menu);
        connect(action, &QAction::triggered,
                [this, actionType]() { handleMainMenuAction(actionType); });
        menu->addAction(action);
    }
    return menu;
}

void MainWindow::setupMVVM()
{
    // Views are created lazily when the corresponding tab is first opened.
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
    if (m_usersView != nullptr) {
        return m_usersView;
    }

    m_usersManagementModel     = new UsersManagementModel(m_apiManager, this);
    m_usersManagementViewModel = new UsersManagementViewModel(*m_usersManagementModel, this);
    m_usersView                = new UsersView(*m_usersManagementViewModel, this);
    connect(m_usersManagementViewModel, &UsersManagementViewModel::errorOccurred, &m_dialogManager,
            &DialogManager::showErrorDialog);

    return m_usersView;
}
